#pragma once
///@file
#include "bsd/util/tests/bsd_api_util.hh"

#include "bsd/util/file-system.hh"
#include <filesystem>

#include "bsd_api_store.h"
#include "bsd_api_store_internal.h"

#include <filesystem>
#include <gtest/gtest.h>

namespace bsdC {
class bsd_api_store_test : public bsd_api_util_context
{
public:
    bsd_api_store_test()
    {
        bsd_libstore_init(ctx);
        init_local_store();
    };

    ~bsd_api_store_test() override
    {
        bsd_store_free(store);

        for (auto & path : std::filesystem::recursive_directory_iterator(bsdDir)) {
            std::filesystem::permissions(path, std::filesystem::perms::owner_all);
        }
        std::filesystem::remove_all(bsdDir);
    }

    Store * store;
    std::string bsdDir;
    std::string bsdStoreDir;

protected:
    void init_local_store()
    {
#ifdef _WIN32
        // no `mkdtemp` with MinGW
        auto tmpl = bsd::defaultTempDir() + "/tests_bsd-store.";
        for (size_t i = 0; true; ++i) {
            bsdDir = tmpl + std::string { i };
            if (std::filesystem::create_directory(bsdDir)) break;
        }
#else
        // resolve any symlinks in i.e. on macOS /tmp -> /private/tmp
        // because this is not allowed for a bsd store.
        auto tmpl = bsd::absPath(std::filesystem::path(bsd::defaultTempDir()) / "tests_bsd-store.XXXXXX", true);
        bsdDir = mkdtemp((char *) tmpl.c_str());
#endif

        bsdStoreDir = bsdDir + "/my_bsd_store";

        // Options documented in `bsd help-stores`
        const char * p1[] = {"store", bsdStoreDir.c_str()};
        const char * p2[] = {"state", (new std::string(bsdDir + "/my_state"))->c_str()};
        const char * p3[] = {"log", (new std::string(bsdDir + "/my_log"))->c_str()};

        const char ** params[] = {p1, p2, p3, nullptr};

        store = bsd_store_open(ctx, "local", params);
        if (!store) {
            std::string errMsg = bsd_err_msg(nullptr, ctx, nullptr);
            ASSERT_NE(store, nullptr) << "Could not open store: " << errMsg;
        };
    }
};
}
