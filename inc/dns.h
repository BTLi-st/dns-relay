#pragma once
#ifndef DNS_SERVER_DNS_H
#define DNS_SERVER_DNS_H

#include <string>
#include <vector>

#include "log.h"

enum DNS_OPCODE
{
    QUERY = 0, // 标准查询
    IQUERY = 1, // 反向查询
    STATUS = 2 // 服务器状态请求
};

enum DNS_RCODE
{
    NO_ERROR = 0, // 没有错误
    FORMAT_ERROR = 1, // 格式错误
    SERVER_FAILURE = 2, // 服务器错误
    NAME_ERROR = 3, // 名字错误
    NOT_IMPLEMENTED = 4, // 不支持
    REFUSED = 5 // 拒绝
};

enum TYPE
{
    A = 1, // IPv4 地址
    NS = 2, // 名字服务器
    MD = 3, // 实际的邮件中转站
    MF = 4, // 邮件中转站
    CNAME = 5, // 规范名称
    SOA = 6, // 开始授权
    MB = 7, // 负责邮箱的邮箱报文
    MG = 8, // 邮箱组成员
    MR = 9, // 邮箱重命名
    NULL_R = 10, // 空
    WKS = 11, // 熟知服务
    PTR = 12, // 指针
    HINFO = 13, // 主机信息
    MINFO = 14, // 邮件信息
    MX = 15, // 邮件交换
    TXT = 16, // 文本
    AAAA = 28, // IPv6 地址
    ANY = 255 // 任意类型
};

struct DNSHeader
{
    unsigned short id; // 标识
    unsigned char qr : 1; // 查询/响应
    unsigned char opcode : 4; // 操作码
    unsigned char aa : 1; // 授权回答
    unsigned char tc : 1; // 截断
    unsigned char rd : 1; // 期望递归
    unsigned char ra : 1; // 可用递归
    unsigned char z : 3; // 保留
    unsigned char rcode : 4; // 响应码
    unsigned short qdcount; // 问题数
    unsigned short ancount; // 回答数
    unsigned short nscount; // 授权数
    unsigned short arcount; // 附加数
};

struct DNSQuestion
{
    unsigned short qtype; // 查询类型
    unsigned short qclass; // 查询类
};

struct DNSRecord
{
    unsigned short type; // 类型
    unsigned short _class; // 类
    unsigned int ttl; // 生存时间
    unsigned short rdlength; // 数据长度
};

class DomainName
{
private:
    std::vector<std::string> domain_name;

public:
    DomainName(std::string domain_name);
    DomainName() = default;

    void set_domain_name(std::string domain_name); // 设置域名
    void set_domain_name_dns_format(std::string domain_name); // 设置域名

    std::string get_domain_name(); // 获取域名
    std::string get_domain_name_dns_format(); // 获取域名
};
#endif // DNS_SERVER_DNS_H