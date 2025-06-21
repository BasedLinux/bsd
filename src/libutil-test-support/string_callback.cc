#include "bsd/util/tests/string_callback.hh"

namespace bsd::testing {

void observe_string_cb(const char * start, unsigned int n, void * user_data)
{
    auto user_data_casted = reinterpret_cast<std::string *>(user_data);
    *user_data_casted = std::string(start);
}

}
