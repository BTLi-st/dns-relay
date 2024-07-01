#include <thread>
#include <chrono>
#include "log.h"
#include "dns.h"

int main()
{
    Log log(FilePath().add_dir("log").add_file("server.log"), DBG);
    // DomainName domain_name("www.baidu.com");
    // log.debug("Domain name: {}", domain_name.get_domain_name());
    // DNSHeader header;
    // header.id = 0x1234; // 4660
    // header.qr = 0; // 查询
    // header.opcode = 0; // 标准查询
    // header.aa = 0; // 非授权回答
    // header.tc = 0; // 未截断
    // header.rd = 1; // 期望递归
    // header.ra = 0; // 未使用递归
    // header.z = 0; // 保留
    // header.rcode = 0; // 无错误
    // header.qdcount = 1; // 问题数
    // header.ancount = 0; // 回答数
    // header.nscount = 0; // 授权数
    // header.arcount = 0; // 附加数
    // DNSQuery query;
    // query.qname = domain_name.get_domain_name_dns_format();
    // query.qtype = A; // A 类型
    // query.qclass = 1; // IN 类
    // DNS dns(log);
    // dns.set_header(header);
    // dns.set_query(query);
    // UDP_SOCKET socket(log);
    // dns.send(socket, "8.8.8.8", 53);
    // std::string ip;
    // int port;
    // DNS dns_recv(log);
    // dns_recv.recv(socket, ip, port);
    // log.debug("IP: {}", ip);
    // log.debug("Port: {}", port);
    // auto record = dns_recv.get_record();
    // for (auto &r : record)
    // {
    //     log.debug("Record {}:", &r - &record[0] + 1); // 记录序号（从 1 开始）
    //     DomainName tmp;
    //     tmp.set_domain_name_dns_format(r.name);
    //     log.debug("Name: {}", tmp.get_domain_name());
    //     log.debug("Type: {}", r.type);
    //     log.debug("Class: {}", r._class);
    //     log.debug("TTL: {}", r.ttl);
    //     log.debug("RDLength: {}", r.rdlength);
    //     if (r.type == A)
    //     {
    //         log.debug("IP: {}.{}.{}.{}", (unsigned char)r.rdata[0], (unsigned char)r.rdata[1], (unsigned char)r.rdata[2], (unsigned char)r.rdata[3]);
    //     }
    //     else if (r.type == CNAME)
    //     {
    //         DomainName tmp;
    //         tmp.set_domain_name_dns_format(r.rdata);
    //         log.debug("CNAME: {}", tmp.get_domain_name());
    //     }
    // }
    UDP_SOCKET socket(log);
    socket.bind("localhost", 53);
    while (true)
    {
        std::string ip;
        int port;
        std::string client_ip;
        int client_port;
        DNS dns_from_client(log);
        dns_from_client.recv(socket, ip, port);
        client_ip = ip;
        client_port = port;
        log.debug("IP: {}", ip);
        log.debug("Port: {}", port);
        std::string server_ip = "8.8.8.8";
        int server_port = 53;
        DNS dns_from_server(log);
        dns_from_client.send(socket, server_ip, server_port);
        dns_from_server.recv(socket, ip, port);
        log.debug("IP: {}", ip);
        log.debug("Port: {}", port);
        if (ip == server_ip && port == server_port)
        {
            dns_from_server.send(socket, client_ip, client_port);
        }
        else
        {
            log.warning("Invalid response.");
        }
        return 0;
    }
}