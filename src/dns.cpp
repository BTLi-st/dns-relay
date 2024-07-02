#include "dns.h"

DomainName::DomainName(std::string domain_name)
{
    set_domain_name(domain_name);
}

void DomainName::set_domain_name(std::string domain_name)
{
    this->domain_name.clear();
    this->domain_name.reserve(10); // 预留空间
    std::string temp;
    for (int i = 0; i < domain_name.size(); i++)
    {
        if (domain_name[i] == '.')
        {
            this->domain_name.push_back(temp);
            temp.clear();
        }
        else
        {
            temp += domain_name[i];
        }
    }
    if (temp.size() != 0)
        this->domain_name.push_back(temp);
}

void DomainName::set_domain_name_dns_format(std::string domain_name)
{
    this->domain_name.clear();
    this->domain_name.reserve(10); // 预留空间
    std::string temp;
    int count = domain_name[0];
    for (int i = 1; i < domain_name.size(); i++)
    {
        if (count == 0)
        {
            this->domain_name.push_back(temp);
            temp.clear();
            count = domain_name[i];
        }
        else
        {
            temp += domain_name[i];
            count--;
        }
    }
    if (temp.size() != 0)
        this->domain_name.push_back(temp);
}

std::string DomainName::get_domain_name()
{
    std::string temp;
    for (int i = 0; i < domain_name.size(); i++)
    {
        temp += domain_name[i];
        if (i != domain_name.size() - 1)
        {
            temp += '.';
        }
    }
    return temp;
}

std::string DomainName::get_domain_name_dns_format()
{
    std::string temp;
    for (int i = 0; i < domain_name.size(); i++)
    {
        temp += (char)domain_name[i].size();
        temp += domain_name[i];
    }
    temp += (char)0;
    return temp;
}

DNS::DNS(Log &log) : log(log) {}

DNS::DNS(const DNS &dns): log(dns.log)
{
    header = dns.header;
    query = dns.query;
    record = dns.record;
    ns_record = dns.ns_record;
    ar_record = dns.ar_record;
}

void DNS::set_header(const DNSHeader &header)
{
    this->header = header;
}

void DNS::set_query(const DNSQuery &query)
{
    this->query = query;
}

void DNS::add_record(const DNSRecord &record)
{
    this->record.push_back(record);
}

void DNS::add_ns_record(const DNSRecord &record)
{
    this->ns_record.push_back(record);
}

void DNS::add_ar_record(const DNSRecord &record)
{
    this->ar_record.push_back(record);
}

const DNSHeader &DNS::get_header()
{
    return header;
}

const DNSQuery &DNS::get_query()
{
    return query;
}

const std::vector<DNSRecord> &DNS::get_record()
{
    return record;
}

const std::vector<DNSRecord> &DNS::get_ns_record()
{
    return ns_record;
}

const std::vector<DNSRecord> &DNS::get_ar_record()
{
    return ar_record;
}

bool DNS::is_query()
{
    return !header.qr;
}

bool DNS::is_EDNS()
{
    for (int i = 0; i < ar_record.size(); i++)
    {
        if (ar_record[i].type == DNS_EDNS)
        {
            return true;
        }
    }
    return false;
}

bool DNS::update_ttl(int time_used)
{
    for (int i = 0; i < record.size(); i++)
    {
        if (record[i].ttl > time_used)
            record[i].ttl -= time_used;
        else
            return false;
    }
    for (int i = 0; i < ns_record.size(); i++)
    {
        if (ns_record[i].ttl > time_used)
            ns_record[i].ttl -= time_used;
        else
            return false;
    }
    for (int i = 0; i < ar_record.size(); i++)
    {
        if (ar_record[i].ttl > time_used)
            ar_record[i].ttl -= time_used;
        else
            return false;
    }
    return true;
}

void DNS::serialize(std::string &data)
{
    log.info("Serialize DNS Data"); // 序列化 DNS 数据
    data.clear();                    // 清空数据
    data += (char)(header.id >> 8);                                                                    // 标识高位
    data += (char)(header.id & 0xff);                                                                  // 标识低位
    data += (char)(header.qr << 7 | header.opcode << 3 | header.aa << 2 | header.tc << 1 | header.rd); // QR, OPCODE, AA, TC, RD
    data += (char)(header.ra << 7 | header.z << 4 | header.rcode);                                     // RA, Z, RCODE
    data += (char)(header.qdcount >> 8);                                                               // 问题数高位
    data += (char)(header.qdcount & 0xff);                                                             // 问题数低位
    data += (char)(header.ancount >> 8);                                                               // 回答数高位
    data += (char)(header.ancount & 0xff);                                                             // 回答数低位
    data += (char)(header.nscount >> 8);                                                               // 授权数高位
    data += (char)(header.nscount & 0xff);                                                             // 授权数低位
    data += (char)(header.arcount >> 8);                                                               // 附加数高位
    data += (char)(header.arcount & 0xff);                                                             // 附加数低位

    std::map<std::string, unsigned short> ptr_map; // 指针映射
    // 查询
    if (header.qdcount == 1) // 问题数为1
    {
        ptr_map[query.qname] = data.size(); // 记录指针
        data += query.qname;
        data += (char)(query.qtype >> 8);    // 查询类型高位
        data += (char)(query.qtype & 0xff);  // 查询类型低位
        data += (char)(query.qclass >> 8);   // 查询类高位
        data += (char)(query.qclass & 0xff); // 查询类低位
    }

    auto add_record = [&](const std::vector<DNSRecord> &record)
    {
        for (int i = 0; i < record.size(); i++)
        {
            if (ptr_map.find(record[i].name) == ptr_map.end())
            {
                ptr_map[record[i].name] = data.size(); // 记录指针
                data += record[i].name;
            }
            else
            {
                data += (char)(0xc0 | (ptr_map[record[i].name] >> 8)); // 指针高位
                data += (char)(ptr_map[record[i].name] & 0xff);        // 指针低位
            }
            data += (char)(record[i].type >> 8);       // 类型高位
            data += (char)(record[i].type & 0xff);     // 类型低位
            data += (char)(record[i]._class >> 8);     // 类高位
            data += (char)(record[i]._class & 0xff);   // 类低位
            data += (char)(record[i].ttl >> 24);       // 生存时间高位
            data += (char)(record[i].ttl >> 16);       // 生存时间次高位
            data += (char)(record[i].ttl >> 8);        // 生存时间次低位
            data += (char)(record[i].ttl & 0xff);      // 生存时间低位
            data += (char)(record[i].rdlength >> 8);   // 数据长度高位
            data += (char)(record[i].rdlength & 0xff); // 数据长度低位
            data += record[i].rdata;
        }
    };

    // 记录
    add_record(record);
    // 授权记录
    add_record(ns_record);
    // 附加记录
    add_record(ar_record);
    log.debug("DNS Data Length: {}", data.size()); // DNS 数据长度
}

bool DNS::deserilize(const std::string &data)
{
    log.info("Deserilize DNS Data"); // 反序列化 DNS 数据
    log.debug("DNS Data Length: {}", data.size()); // DNS 数据长度
    if (data.size() < 12) // 数据长度小于12
    {
        log.error("DNS Data Length is less than 12"); // 数据长度小于12
        return false;
    }
    try
    {
        header.id = (data[0] << 8) | data[1];        // 标识
        header.qr = data[2] >> 7;                    // QR
        header.opcode = (data[2] >> 3) & 0xf;        // OPCODE
        header.aa = (data[2] >> 2) & 0x1;            // AA
        header.tc = (data[2] >> 1) & 0x1;            // TC
        header.rd = data[2] & 0x1;                   // RD
        header.ra = data[3] >> 7;                    // RA
        header.z = (data[3] >> 4) & 0x7;             // Z
        header.rcode = data[3] & 0xf;                // RCODE
        header.qdcount = (data[4] << 8) | data[5];   // 问题数
        header.ancount = (data[6] << 8) | data[7];   // 回答数
        header.nscount = (data[8] << 8) | data[9];   // 授权数
        header.arcount = (data[10] << 8) | data[11]; // 附加数
        // 查询

        int index = 12; // 头部长度

        std::function<std::string()> get_domain_name = [&]() -> std::string
        {
            std::string temp;
            int ptr_tmp = index;
            bool use_ptr = false;             // 使用指针
            if ((data[index] & 0xc0) == 0xc0) // 指针
            {
                ptr_tmp = ((data[index] & 0x3f) << 8) | data[index + 1];
                use_ptr = true;
            }
            while (data[ptr_tmp] != 0)
            {
                int count = data[ptr_tmp];
                temp += data[ptr_tmp];
                ptr_tmp++;
                for (int i = 0; i < count; i++)
                {
                    temp += data[ptr_tmp];
                    ptr_tmp++;
                }
            }
            temp += char(0);
            if (!use_ptr)
            {
                index = ptr_tmp + 1;
            }
            else
            {
                index += 2;
            }
            return temp;
        };

        auto get_record = [&](std::vector<DNSRecord> &record, unsigned short &count)
        {
            record.clear();
            for (int i = 0; i < count; i++)
            {
                DNSRecord temp;
                temp.name = get_domain_name();
                temp.type = (data[index] << 8) | data[index + 1];                                                    // 类型
                index += 2;                                                                                          // 类型长度
                temp._class = (data[index] << 8) | data[index + 1];                                                  // 类
                index += 2;                                                                                          // 类长度
                temp.ttl = (data[index] << 24) | (data[index + 1] << 16) | (data[index + 2] << 8) | data[index + 3]; // 生存时间
                index += 4;                                                                                          // 生存时间长度
                temp.rdlength = (data[index] << 8) | data[index + 1];                                                // 数据长度
                index += 2;                                                                                          // 数据长度长度
                for (int j = 0; j < temp.rdlength; j++)
                {
                    temp.rdata += data[index];
                    index++;
                }
                if (temp.type == DNS_EDNS) // EDNS 记录
                {
                    log.warning("DNS OPT Record is not supported，but it will be stored."); // 不支持 OPT 记录，但会被存储
                    log.info("EDNS is not supported on this server.");                      // 服务器不支持 EDNS
                }
                record.push_back(temp);
            }
        };

        if (header.qdcount == 1) // 问题数为1
        {
            query.qname = get_domain_name();
            query.qtype = (data[index] << 8) | data[index + 1];  // 查询类型
            index += 2;                                          // 查询类型长度
            query.qclass = (data[index] << 8) | data[index + 1]; // 查询类
            index += 2;                                          // 查询类长度
        }
        else if (header.qdcount > 1)
        {
            log.error("DNS Query Count is over 1"); // 查询数量超过1
            return false;
        }

        // 记录
        get_record(record, header.ancount);

        // 授权记录
        get_record(ns_record, header.nscount);

        // 附加记录
        get_record(ar_record, header.arcount);
    }
    catch (const std::exception &e)
    {
        log.error("DNS Data is invalid: {}", e.what()); // DNS 数据无效
        return false;
    }
    return true;
}

bool IP::set_ip(const std::string &ip)
{
    std::string temp;
    std::regex ipv4Regex("^(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\."
                         "(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\."
                         "(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\."
                         "(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)$");
    if (!std::regex_match(ip, ipv4Regex))
        return false;
    std::istringstream iss(ip);
    for (int i = 0; i < 4; i++)
    {
        int temp;
        iss >> temp;
        this->ip[i] = temp;
        if (i != 3)
        {
            char dot;
            iss >> dot;
        }
    }
    return true;
}

void IP::set_ip(const std::array<unsigned char, 4> &ip)
{
    this->ip = ip;
}

std::string IP::get_ip()
{
    std::string temp;
    for (int i = 0; i < 4; i++)
    {
        temp += std::to_string(ip[i]);
        if (i != 3)
        {
            temp += '.';
        }
    }
    return temp;
}

std::string IP::get_ip_dns_format()
{
    std::string temp;
    for (int i = 0; i < 4; i++)
    {
        temp += (char)ip[i];
    }
    return temp;
}

std::array<unsigned char, 4> IP::get_ip_array()
{
    return ip;
}
