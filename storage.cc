/**
 * Storage Tarantool
 */

#include <string>

#include <tarantool/tarantool.h>
#include <tarantool/tnt_net.h>
#include <tarantool/tnt_opt.h>

class Storage
{
public:
    Storage(std::string address) {
        this->address = address;
    }
private:
    std::string address;
};
