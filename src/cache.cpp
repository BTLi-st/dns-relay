#include "cache.h"

DNSCache::DNSCache(Log &log, size_t max_size) : log(log), max_size(max_size) {}

void DNSCache::add(const DNSQuery &query, const DNS &dns)
{
    DomainName domain_name;
    domain_name.set_domain_name_dns_format(query.qname);
    log.debug("Add cache: {}", domain_name.get_domain_name());
    std::unique_lock lock(mutex);
    auto it = cache.find(query);
    if (it != cache.end())
    {
        cache_list.erase(it->second);
        cache.erase(it);
    }
    cache_list.push_front(CacheValue(query, dns));
    if (cache.size() > max_size) // 超过最大大小
    {
        log.info("Cache size exceed, clean cache.");
        cache.erase(cache_list.back().get_query()); // 移除最后一个
        cache_list.pop_back();                      // 移除最后一个
    }
    cache[query] = cache_list.begin();
}

bool DNSCache::exist(const DNSQuery &query)
{
    std::shared_lock lock(mutex);
    if (cache.find(query) == cache.end())
    {
        log.info("Cache not found");
        return false;
    }
    return true;
}

std::pair<std::chrono::time_point<std::chrono::steady_clock>, DNS> DNSCache::get(const DNSQuery &query)
{
    DomainName domain_name;
    domain_name.set_domain_name_dns_format(query.qname);
    log.info("Get cache name: {} type: {}", domain_name.get_domain_name(), query.qtype);
    std::unique_lock lock(mutex);
    auto it = cache.find(query);
    if (it == cache.end())
    {
        log.info("Cache not found");
        return std::make_pair(std::chrono::time_point<std::chrono::steady_clock>(), DNS(log));
    }
    cache_list.splice(cache_list.begin(), cache_list, it->second); // 将元素移动到链表头部
    cache[query] = cache_list.begin();
    return std::make_pair(it->second->get_insert_time(), it->second->get_dns());
}

void DNSCache::remove(const DNSQuery &query)
{
    DomainName domain_name;
    domain_name.set_domain_name_dns_format(query.qname);
    log.info("Remove cache name: {} type: {}", domain_name.get_domain_name(), query.qtype);
    std::unique_lock lock(mutex);
    auto it = cache.find(query);
    if (it == cache.end())
    {
        log.info("Cache not found");
        return;
    }
    cache_list.erase(it->second);
    cache.erase(it);
}

void DNSCache::clear()
{
    log.info("Clear cache");
    std::unique_lock lock(mutex);
    cache.clear();
    cache_list.clear();
}

void DNSCache::clean()
{
    log.info("Clean cache");
    std::unique_lock lock(mutex);
    auto it = cache_list.begin();
    while (it != cache_list.end())
    {
        if (std::chrono::duration_cast<std::chrono::seconds>(std::chrono::steady_clock::now() - it->get_insert_time()).count() > 60)
        {
            log.info("Clean cache name: {} type: {}", it->get_query().qname, it->get_query().qtype);
            cache.erase(it->get_query());
            it = cache_list.erase(it);
        }
        else
        {
            it++;
        }
    }
}

const DNSQuery &CacheValue::get_query()
{
    return query;
}

const DNS &CacheValue::get_dns()
{
    return dns;
}

const std::chrono::time_point<std::chrono::steady_clock> &CacheValue::get_insert_time()
{
    return insert_time;
}

CacheValue::CacheValue(const DNSQuery &query, const DNS &dns) : query(query), dns(dns), insert_time(std::chrono::steady_clock::now()) {}
