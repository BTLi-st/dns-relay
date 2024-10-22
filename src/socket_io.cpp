#include "socket_io.h"

void SocketIO::do_read()
{
    while (running)
    {
        std::string ip;
        int port;
        char buf[4096];
        int len = socket.recvfrom(ip, port, buf, sizeof(buf));
        if (len > 0)
        {
            log->debug("read from {}:{}, len: {}", ip, port, len);
            read_callback(ip, port, std::string(buf, len));
        }
        if (len == -1)
        {
            if (!socket.is_valid()) // socket 关闭或无效
                return;
            log->info("continue to read...");
        }
    }
}

void SocketIO::do_write()
{
    while (1)
    {
        std::unique_lock<std::mutex> lock(write_queue_mutex);
        write_queue_cv.wait(lock, [this] { return !write_queue.empty() || !running; });
        if (!running && write_queue.empty())
        {
            return;
        }
        auto [ip, port, data] = write_queue.front();
        write_queue.pop();
        lock.unlock();
        if (!socket.sendto(ip, port, data.c_str(), data.size()))
        {
            if (!socket.is_valid()) // socket 关闭或无效
                return;
            log->info("continue to write...");
        }
    }
}

SocketIO::SocketIO(std::shared_ptr<Log> log, std::string ip, int port, std::function<void(std::string, int, std::string)> read_callback) : log(log), socket(log)
{
    socket.bind(ip, port);
    this->read_callback = read_callback;
}

SocketIO::~SocketIO()
{
    if (running.load())
        stop();
}

void SocketIO::write(std::string ip, int port, std::string data)
{
    std::unique_lock<std::mutex> lock(write_queue_mutex);
    write_queue.push({ip, port, data});
    lock.unlock();
    write_queue_cv.notify_one();
}

void SocketIO::run()
{
    read_thread = std::jthread([this] { do_read(); });
    write_thread = std::jthread([this] { do_write(); });
}

void SocketIO::stop()
{
    running = false;
    write_queue_cv.notify_one();
    socket.close();
    read_thread.join();
    write_thread.join();
    log->info("SocketIO stopped successfully.");
}
