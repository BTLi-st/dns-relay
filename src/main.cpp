#include <thread>
#include <chrono>
#include "log.h"
#include "dns.h"

int main()
{
    Log log(FilePath().add_dir("log").add_file("server.log"), DBG);
    DomainName domain_name("www.baidu.com");
    log.debug("Domain name: {}", domain_name.get_domain_name());
    DomainName domain_name_dns_format;
    domain_name_dns_format.set_domain_name_dns_format(domain_name.get_domain_name_dns_format());
    log.debug("Domain name DNS format: {}", domain_name_dns_format.get_domain_name());
    return 0;
}