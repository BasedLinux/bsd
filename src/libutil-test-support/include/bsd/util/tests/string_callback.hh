#pragma once
#include <string>

namespace bsd::testing {

void observe_string_cb(const char * start, unsigned int n, void * user_data);

inline void * observe_string_cb_data(std::string & out)
{
    return (void *) &out;
};

#define OBSERVE_STRING(str) bsd::testing::observe_string_cb, bsd::testing::observe_string_cb_data(str)

}
