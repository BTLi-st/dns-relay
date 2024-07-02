#include "server.h"

DNSRelayServer::DNSRelayServer(Log &log, std::string ip, int port, FilePath map_file, int pool_size) : log(log), socket_io(log, ip, port, [this](std::string ip, int port, std::string data)
                                                                                                                           { handle_info(ip, port, data); }),
                                                                                                       pool(pool_size), cache(log), map(log, map_file)
{
}

DNSRelayServer::~DNSRelayServer()
{
    stop();
}

void DNSRelayServer::set_server(std::string ip, int port)
{
    server_ip = ip;
    server_port = port;
}

void DNSRelayServer::run()
{
    running.store(true);
    socket_io.run();
    clean_thread = std::jthread([this]
                                { clean(); });
}

void DNSRelayServer::stop()
{
    running.store(false);
    socket_io.stop();
    pool.stop();
    clean_thread.join();
}

void DNSRelayServer::clean()
{
    while (running.load()) // 运行状态
    {
        std::this_thread::sleep_for(std::chrono::seconds(60));
        std::unique_lock<std::shared_mutex> lock(queries_mutex);
        for (auto it = queries.begin(); it != queries.end();)
        {
            if (it->second.is_timeout(std::chrono::seconds(10)))
            {
                log.info("Query timeout: {}", it->first);
                queries.erase(it++);
            }
            else
            {
                ++it;
            }
        }
    }
}

void DNSRelayServer::handle_info(std::string ip, int port, std::string data)
{
    auto dns = std::make_shared<DNS>(log);
    if (!dns->deserilize(data))
    {
        log.error("deserilize error");
        return;
    }
    if (dns->is_query())
    {
        pool.add_task(std::bind(&DNSRelayServer::handle_query, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3), ip, port, dns);
    }
    else
    {
        pool.add_task(std::bind(&DNSRelayServer::handle_response, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3), ip, port, dns);
    }
}

void DNSRelayServer::handle_query(std::string ip, int port, std::shared_ptr<DNS> dns)
{
    if (dns->is_EDNS()) // EDNS 查询
    {
        handle_edns_query(ip, port, dns); // 处理 EDNS 查询
    }
    else // 非 EDNS 查询
    {
        auto query = dns->get_query();
        if (query.qtype == DNS_A) // 查询 A
        {
            handle_query_A(ip, port, dns);
        }
        else if (query.qtype == DNS_PTR) // 查询 PTR
        {
            handle_query_ptr(ip, port, dns);
        }
        else
        {
            handle_query_other(ip, port, dns);
        }
    }
}

void DNSRelayServer::handle_response(std::string ip, int port, std::shared_ptr<DNS> dns)
{
    if (ip != server_ip || port != server_port) // 不是服务器响应
    {
        log.error("response from unknown server");
        return;
    }
    std::unique_lock<std::shared_mutex> lock(queries_mutex);
    auto header = dns->get_header();
    if (queries.find(header.id) == queries.end()) // ID 不在查询列表
    {
        log.error("response id is not in queries");
        return;
    }
    auto query_store = queries[header.id];
    queries.erase(header.id);
    lock.unlock(); // 解锁
    header.id = query_store.dns->get_header().id;
    dns->set_header(header);
    ip = query_store.ip;
    port = query_store.port;
    if (dns->is_EDNS()) // EDNS 响应
    {
        log.info("EDNS response, will not cache");
    }
    else
    {
        auto query = query_store.dns->get_query();
        if (dns.get()->get_header().rcode == 0) // 无错误
            cache.add(query, *dns); // 添加缓存
    }
    std::string data;
    dns->serialize(data);
    socket_io.write(ip, port, data);
}

void DNSRelayServer::relay(std::string ip, int port, std::shared_ptr<DNS> dns)
{
    std::unique_lock<std::shared_mutex> lock(queries_mutex);
    auto new_id = id_gen.generate();
    queries[new_id] = QueryStore(dns, ip, port);
    auto header = dns->get_header();
    header.id = new_id;
    dns->set_header(header);
    std::string data;
    dns->serialize(data);
    socket_io.write(server_ip, server_port, data);
    return;
}

void DNSRelayServer::handle_query_A(std::string ip, int port, std::shared_ptr<DNS> dns)
{
    auto query = dns->get_query();
    DomainName domain_name;
    domain_name.set_domain_name_dns_format(query.qname);
    auto ip_list = map.get(domain_name.get_domain_name());
    if (!ip_list.empty()) // 有 IP 映射
    {
        if (ip_list.size() == 1 && ip_list[0].get_ip() == "0.0.0.0") // 拦截
        {
            auto response = std::make_shared<DNS>(log);
            auto header = dns->get_header();
            header.qr = 1; // 响应
            header.rcode = 3; // 拒绝
            response->set_header(header);
            response->set_query(query);
            std::string data;
            response->serialize(data);
            socket_io.write(ip, port, data);
            return;
        }
        else // 有 IP 映射
        {
            auto response = std::make_shared<DNS>(log);
            auto header = dns->get_header();
            header.qr = 1; // 响应
            header.ancount = ip_list.size(); // 回答数
            response->set_header(header);
            response->set_query(query);
            for (auto &ip : ip_list)
            {
                DNSRecord record;
                record.name = query.qname;
                record.type = DNS_A;
                record._class = 1;
                record.ttl = 60;
                record.rdlength = 4;
                record.rdata = ip.get_ip_dns_format();
                response->add_record(record);
            }
            std::string data;
            response->serialize(data);
            socket_io.write(ip, port, data);
            return;
        }
    }
    handle_query_other(ip, port, dns); // 处理查询其他
}

void DNSRelayServer::handle_query_ptr(std::string ip, int port, std::shared_ptr<DNS> dns)
{
    auto query = dns->get_query();
    auto domain_name = DomainName(query.qname);
    if (domain_name.get_domain_name() == "1.0.0.127.in-addr.arpa") // 本地查询
    {
        auto response = std::make_shared<DNS>(log);
        auto header = dns->get_header();
        header.qr = 1; // 响应
        header.aa = 0; // 不是权威服务器
        header.ra = 1; // 允许递归
        header.ancount = 1; // 回答数
        response->set_header(header);
        response->set_query(query);
        DNSRecord record;
        record.name = query.qname;
        record.type = DNS_PTR;
        record._class = 1;
        record.ttl = 60;
        record.rdlength = 13;
        record.rdata = "localhost.local";
        response->add_record(record);
        std::string data;
        response->serialize(data);
        socket_io.write(ip, port, data);
        return;
    }
    handle_query_other(ip, port, dns); // 处理查询其他
}

void DNSRelayServer::handle_query_other(std::string ip, int port, std::shared_ptr<DNS> dns)
{
    if (!cache.exist(dns->get_query())) // 不存在缓存
    {
        relay(ip, port, dns); // 转发
        return;
    }
    auto query = dns->get_query();
    auto [insert_time, cache_dns] = cache.get(query);
    auto time_passed = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::steady_clock::now() - insert_time); // 时间差
    auto response = std::make_shared<DNS>(cache_dns);
    auto header = response->get_header();
    header.qr = 1; // 响应
    header.ra = 1; // 允许递归
    header.aa = 0; // 不是权威服务器
    header.id = dns->get_header().id; // ID
    response->set_header(header);
    if (!response->update_ttl(time_passed.count())) // 更新 TTL
    {
        log.error("update ttl error");
        cache.remove(query);
        relay(ip, port, dns); // 转发
        return;
    }
    std::string data;
    response->serialize(data);
    socket_io.write(ip, port, data);
}

void DNSRelayServer::handle_edns_query(std::string ip, int port, std::shared_ptr<DNS> dns)
{
    relay(ip, port, dns); // 转发
}