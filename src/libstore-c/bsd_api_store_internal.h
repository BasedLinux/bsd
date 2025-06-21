#ifndef NIX_API_STORE_INTERNAL_H
#define NIX_API_STORE_INTERNAL_H
#include "bsd/store/store-api.hh"

struct Store
{
    bsd::ref<bsd::Store> ptr;
};

struct StorePath
{
    bsd::StorePath path;
};

#endif
