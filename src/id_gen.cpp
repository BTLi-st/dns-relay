#include "id_gen.h"

IDGenerator::IDGenerator() : gen(rd()), dis(0, 65535)
{
}

unsigned short IDGenerator::generate()
{
    std::lock_guard<std::mutex> lock(mutex);
    unsigned short id = dis(gen);
    while (ids.find(id) != ids.end())
    {
        id = dis(gen);
    }
    ids[id] = true;
    return id;
}

void IDGenerator::release(unsigned short id)
{
    std::lock_guard<std::mutex> lock(mutex);
    ids.erase(id);
}
