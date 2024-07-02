#pragma once
#ifndef DNS_SERVER_SOCKET_H
#define DNS_SERVER_SOCKET_H

#include <iostream>
#include <string>
#include <cstring>

#include "log.h"

#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#ifdef _MSC_VER
#pragma comment(lib, "ws2_32.lib")
#endif
#else
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <unistd.h>
#endif

class UDP_SOCKET
{
private:
    Log &log;

#ifdef _WIN32
    WSADATA wsaData;
    SOCKET s;
    struct sockaddr_in addr;

    static int instance_count;
#else
    int s;
    struct sockaddr_in addr;
#endif

public:
    UDP_SOCKET(Log &log);
    ~UDP_SOCKET();

    void bind(const char *ip, int port); // 绑定
    void bind(std::string ip, int port); // 绑定
    bool sendto(const char *ip, int port, const char *buf, int len); // 发送
    bool sendto(const std::string ip, int port, const char *buf, int len); // 发送
    int recvfrom(char *ip, int *port, char *buf, int len); // 接收
    int recvfrom(std::string &ip, int &port, char *buf, int len); // 接收，不需要给 IP 预留空间

    void close(); // 关闭
};

#endif // DNS_SERVER_SOCKET_H