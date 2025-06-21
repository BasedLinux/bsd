#pragma once
///@file
#include "bsd_api_util.h"

#include <gtest/gtest.h>
#include <string_view>

namespace bsdC {

class bsd_api_util_context : public ::testing::Test
{
protected:

    bsd_api_util_context()
    {
        ctx = bsd_c_context_create();
        bsd_libutil_init(ctx);
    };

    ~bsd_api_util_context() override
    {
        bsd_c_context_free(ctx);
        ctx = nullptr;
    }

    bsd_c_context * ctx;

    inline std::string loc(const char * file, int line)
    {
        return std::string(file) + ":" + std::to_string(line);
    }

    inline void assert_ctx_ok(const char * file, int line)
    {
        if (bsd_err_code(ctx) == NIX_OK) {
            return;
        }
        unsigned int n;
        const char * p = bsd_err_msg(nullptr, ctx, &n);
        std::string msg(p, n);
        throw std::runtime_error(loc(file, line) + ": bsd_err_code(ctx) != NIX_OK, message: " + msg);
    }
#define assert_ctx_ok() assert_ctx_ok(__FILE__, __LINE__)

    inline void assert_ctx_err(const char * file, int line)
    {
        if (bsd_err_code(ctx) != NIX_OK) {
            return;
        }
        throw std::runtime_error(loc(file, line) + ": Got NIX_OK, but expected an error!");
    }
#define assert_ctx_err() assert_ctx_err(__FILE__, __LINE__)
};

}
