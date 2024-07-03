#pragma once
#ifndef DNS_SERVER_DNS_H
#define DNS_SERVER_DNS_H

#include <string>
#include <vector>
#include <array>
#include <memory>
#include <regex>
#include <functional>
#include <map>

#include "log.h"

enum TYPE
{
    DNS_A = 1, // IPv4 地址
    DNS_NS = 2, // 名字服务器
    DNS_CNAME = 5, // 规范名称
    DNS_SOA = 6, // 开始授权
    DNS_PTR = 12, // 指针
    DNS_MX = 15, // 邮件交换
    DNS_TXT = 16, // 文本
    DNS_AAAA = 28, // IPv6 地址
    DNS_SRV = 33, // 服务
    DNS_EDNS = 41, // 扩展
    DNS_ANY = 255 // 任何类型
};

struct DNSHeader
{
    unsigned short id; // 标识
    unsigned char qr : 1; // 查询/响应，0 为查询，1 为响应
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

struct DNSQuery
{
    std::string qname; // 查询名
    unsigned short qtype; // 查询类型
    unsigned short qclass; // 查询类

    auto operator<=>(const DNSQuery &other) const = default;
};

struct DNSRecord
{
    std::string name; // 名字
    unsigned short type; // 类型
    unsigned short _class; // 类
    unsigned int ttl; // 生存时间
    unsigned short rdlength; // 数据长度
    std::string rdata; // 数据
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

class IP
{
private:
    std::array<unsigned char, 4> ip;
public:
    IP() = default;

    bool set_ip(const std::string &ip); // 设置 IP
    void set_ip(const std::array<unsigned char, 4> &ip); // 设置 IP

    std::string get_ip(); // 获取 IP
    std::string get_ip_dns_format(); // 获取 IP
    std::array<unsigned char, 4> get_ip_array(); // 获取 IP
};

class DNS
{
private:
    DNSHeader header; // 头部
    DNSQuery query; // 单查询
    std::vector<DNSRecord> record; // 记录
    std::vector<DNSRecord> ns_record; // 授权记录
    std::vector<DNSRecord> ar_record; // 附加记录

    std::shared_ptr<Log> log; // 日志
public:
    DNS(std::shared_ptr<Log> log); // 构造函数
    DNS(const DNS &dns); // 拷贝构造函数

    void set_header(const DNSHeader &header); // 设置头部
    void set_query(const DNSQuery &query); // 设置查询
    void add_record(const DNSRecord &record); // 添加记录
    void add_ns_record(const DNSRecord &record); // 添加授权记录
    void add_ar_record(const DNSRecord &record); // 添加附加记录

    const DNSHeader &get_header(); // 获取头部
    const DNSQuery &get_query(); // 获取查询
    const std::vector<DNSRecord> &get_record(); // 获取记录
    const std::vector<DNSRecord> &get_ns_record(); // 获取授权记录
    const std::vector<DNSRecord> &get_ar_record(); // 获取附加记录

    bool is_query(); // 是否是查询

    bool is_EDNS(); // 是否是 EDNS

    bool update_ttl(int time_used); // 更新 TTL

    void serialize(std::string &data); // 序列化
    bool deserilize(const std::string &data); // 反序列化
};
#endif // DNS_SERVER_DNS_H