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
    return temp;
}
