#pragma once
///@file

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "bsd/store/store-api.hh"
#include "bsd/store/store-open.hh"

namespace bsd {

class LibStoreTest : public virtual ::testing::Test
{
public:
    static void SetUpTestSuite()
    {
        initLibStore(false);
    }

protected:
    LibStoreTest()
        : store(openStore({
              .variant =
                  StoreReference::Specified{
                      .scheme = "dummy",
                  },
              .params = {},
          }))
    {
    }

    ref<Store> store;
};

} /* namespace bsd */
