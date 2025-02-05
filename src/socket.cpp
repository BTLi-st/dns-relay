#include "socket.h"

#ifdef _WIN32
std::atomic<int> UDP_SOCKET::instance_count = std::atomic<int>(0);
#endif

UDP_SOCKET::UDP_SOCKET(std::shared_ptr<Log> log) : log(log)
{
    log->debug("Create UDP socket.");
#ifdef _WIN32
    if (instance_count.load() == 0)
    {
        if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
        {
            log->fatal("WSAStartup failed: {}", std::to_string(WSAGetLastError()));
            throw std::runtime_error("WSAStartup failed.");
        }
    }
    instance_count.store(instance_count.load() + 1);
    s.store(socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP));
    if (s.load() == -1)
    {
        log->fatal("Create socket failed: {}", std::to_string(WSAGetLastError()));
        WSACleanup();
        throw std::runtime_error("Create socket failed.");
    }
#else
    s.store(socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP));
    if (s.load() == -1)
    {
        log->fatal("Create socket failed: {}", std::to_string(errno));
        throw std::runtime_error("Create socket failed.");
    }
#endif
}

UDP_SOCKET::~UDP_SOCKET()
{
    if (s.load() != -1)
    {
        close();
    }
#ifdef _WIN32
    instance_count.store(instance_count.load() - 1);
    if (instance_count.load() == 0)
    {
        WSACleanup();
    }
#endif
}

void UDP_SOCKET::bind(const char *ip, int port)
{
    log->info("Bind to {}:{}", ip, port);
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = inet_addr(ip);
    if (::bind(s.load(), (struct sockaddr *)&addr, sizeof(addr)) == -1)
    {
#ifdef _WIN32
        log->fatal("Bind failed: {}", std::to_string(WSAGetLastError()));
#else
        log->fatal("Bind failed: {}", std::to_string(errno));
#endif
        throw std::runtime_error("Bind failed.");
    }
}

void UDP_SOCKET::bind(std::string ip, int port)
{
    bind(ip.c_str(), port);
}

bool UDP_SOCKET::sendto(const char *ip, int port, const char *buf, int len)
{
    log->debug("Send to {}:{}", ip, port);
    struct sockaddr_in addr;                                                    // 地址
    addr.sin_family = AF_INET;                                                  // IPv4
    addr.sin_port = htons(port);                                                // 端口
    addr.sin_addr.s_addr = inet_addr(ip);                                       // IP 地址
    if (::sendto(s.load(), buf, len, 0, (struct sockaddr *)&addr, sizeof(addr)) == -1) // 发送数据
    {
        #ifdef _WIN32
        log->error("Socket sendto failed: {}", std::to_string(WSAGetLastError()));
        #else
        log->error("Socket sendto failed: {}", std::to_string(errno));
        #endif
        return false;
    }
    return true;
}

bool UDP_SOCKET::sendto(const std::string ip, int port, const char *buf, int len)
{
    return sendto(ip.c_str(), port, buf, len);
}

int UDP_SOCKET::recvfrom(char *ip, int *port, char *buf, int len)
{

    struct sockaddr_in addr;                                                   // 地址
    socklen_t addr_len = sizeof(addr);                                         // 地址长度
    int ret = ::recvfrom(s.load(), buf, len, 0, (struct sockaddr *)&addr, &addr_len); // 接收数据
    if (ret == -1)
    {
        #ifdef _WIN32
        log->error("Socket recvfrom failed: {}", std::to_string(WSAGetLastError()));
        #else
        log->error("Socket recvfrom failed: {}", std::to_string(errno));
        #endif
        return -1;
    }
    *port = ntohs(addr.sin_port);                                              // 端口
    strcpy(ip, inet_ntoa(addr.sin_addr));                                      // IP 地址
    log->debug("Recv from {}:{}", ip, *port);
    
    return ret;
}

int UDP_SOCKET::recvfrom(std::string &ip, int &port, char *buf, int len)
{
    char _ip[16];                              // IP 地址
    int _port;                                 // 端口
    int ret = recvfrom(_ip, &_port, buf, len); // 接收数据
    ip = _ip;                                  // IP 地址
    port = _port;                              // 端口
    return ret;
}

void UDP_SOCKET::close()
{
    log->info("Close UDP socket.");
    if (s.load() == -1)
    {
        return;
    }
#ifdef _WIN32
    closesocket(s.load());
    s.store(-1);
#else
    ::close(s.load());
    s.store(-1);
#endif
}

bool UDP_SOCKET::is_valid() const
{
    return s.load() != -1;
}
