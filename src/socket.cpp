#include "socket.h"

#ifdef _WIN32
int UDP_SOCKET::instance_count = 0;
#endif

UDP_SOCKET::UDP_SOCKET(Log &log) : log(log)
{
    log.debug("Create UDP socket.");
#ifdef _WIN32
    if (instance_count == 0)
    {
        if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
        {
            log.fatal("WSAStartup failed.");
            throw std::runtime_error("WSAStartup failed.");
        }
    }
    instance_count++;
    s = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (s == -1)
    {
        log.fatal("Create socket failed.");
        WSACleanup();
        throw std::runtime_error("Create socket failed.");
    }
#else
    s = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (s == -1)
    {
        log.fatal("Create socket failed.");
        throw std::runtime_error("Create socket failed.");
    }
#endif
}

UDP_SOCKET::~UDP_SOCKET()
{
    if (s != -1)
    {
        close();
    }
#ifdef _WIN32
    instance_count--;
    if (instance_count == 0)
    {
        WSACleanup();
    }
#endif
}

void UDP_SOCKET::bind(const char *ip, int port)
{
    log.info("Bind to {}:{}", ip, port);
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = inet_addr(ip);
    if (::bind(s, (struct sockaddr *)&addr, sizeof(addr)) == -1)
    {
        log.fatal("Bind failed.");
        throw std::runtime_error("Bind failed." + std::to_string(errno));
    }
}

void UDP_SOCKET::bind(std::string ip, int port)
{
    bind(ip.c_str(), port);
}

void UDP_SOCKET::sendto(const char *ip, int port, const char *buf, int len)
{
    log.debug("Send to {}:{}", ip, port);
    struct sockaddr_in addr; // 地址
    addr.sin_family = AF_INET; // IPv4
    addr.sin_port = htons(port); // 端口
    addr.sin_addr.s_addr = inet_addr(ip); // IP 地址
    if (::sendto(s, buf, len, 0, (struct sockaddr *)&addr, sizeof(addr)) == -1) // 发送数据
    {
        log.fatal("Socket sendto failed.");
        throw std::runtime_error("Socket sendto failed.");
    }
}

void UDP_SOCKET::sendto(const std::string ip, int port, const char *buf, int len)
{
    sendto(ip.c_str(), port, buf, len);
}

int UDP_SOCKET::recvfrom(char *ip, int *port, char *buf, int len)
{
    
    struct sockaddr_in addr; // 地址
    socklen_t addr_len = sizeof(addr); // 地址长度
    int ret = ::recvfrom(s, buf, len, 0, (struct sockaddr *)&addr, &addr_len); // 接收数据
    *port = ntohs(addr.sin_port); // 端口
    strcpy(ip, inet_ntoa(addr.sin_addr)); // IP 地址
    log.debug("Recv from {}:{}", ip, *port);
    return ret;
}

int UDP_SOCKET::recvfrom(std::string &ip, int &port, char *buf, int len)
{
    char _ip[16]; // IP 地址
    int _port; // 端口
    int ret = recvfrom(_ip, &_port, buf, len); // 接收数据
    ip = _ip; // IP 地址
    port = _port; // 端口
    return ret;
}

void UDP_SOCKET::close()
{
    log.info("Close UDP socket.");
#ifdef _WIN32
    closesocket(s);
    s = INVALID_SOCKET;
#else
    ::close(s);
    s = -1;
#endif
}
