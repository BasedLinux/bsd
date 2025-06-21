#include "bsd_api_store.h"
#include "bsd_api_store_internal.h"
#include "bsd_api_util.h"
#include "bsd_api_util_internal.h"

#include "bsd/store/path.hh"
#include "bsd/store/store-api.hh"
#include "bsd/store/store-open.hh"
#include "bsd/store/build-result.hh"

#include "bsd/store/globals.hh"

bsd_err bsd_libstore_init(bsd_c_context * context)
{
    if (context)
        context->last_err_code = NIX_OK;
    try {
        bsd::initLibStore();
    }
    NIXC_CATCH_ERRS
}

bsd_err bsd_libstore_init_no_load_config(bsd_c_context * context)
{
    if (context)
        context->last_err_code = NIX_OK;
    try {
        bsd::initLibStore(false);
    }
    NIXC_CATCH_ERRS
}

Store * bsd_store_open(bsd_c_context * context, const char * uri, const char *** params)
{
    if (context)
        context->last_err_code = NIX_OK;
    try {
        std::string uri_str = uri ? uri : "";

        if (uri_str.empty())
            return new Store{bsd::openStore()};

        if (!params)
            return new Store{bsd::openStore(uri_str)};

        bsd::Store::Config::Params params_map;
        for (size_t i = 0; params[i] != nullptr; i++) {
            params_map[params[i][0]] = params[i][1];
        }
        return new Store{bsd::openStore(uri_str, params_map)};
    }
    NIXC_CATCH_ERRS_NULL
}

void bsd_store_free(Store * store)
{
    delete store;
}

bsd_err bsd_store_get_uri(bsd_c_context * context, Store * store, bsd_get_string_callback callback, void * user_data)
{
    if (context)
        context->last_err_code = NIX_OK;
    try {
        auto res = store->ptr->getUri();
        return call_bsd_get_string_callback(res, callback, user_data);
    }
    NIXC_CATCH_ERRS
}

bsd_err
bsd_store_get_storedir(bsd_c_context * context, Store * store, bsd_get_string_callback callback, void * user_data)
{
    if (context)
        context->last_err_code = NIX_OK;
    try {
        return call_bsd_get_string_callback(store->ptr->storeDir, callback, user_data);
    }
    NIXC_CATCH_ERRS
}

bsd_err
bsd_store_get_version(bsd_c_context * context, Store * store, bsd_get_string_callback callback, void * user_data)
{
    if (context)
        context->last_err_code = NIX_OK;
    try {
        auto res = store->ptr->getVersion();
        return call_bsd_get_string_callback(res.value_or(""), callback, user_data);
    }
    NIXC_CATCH_ERRS
}

bool bsd_store_is_valid_path(bsd_c_context * context, Store * store, StorePath * path)
{
    if (context)
        context->last_err_code = NIX_OK;
    try {
        return store->ptr->isValidPath(path->path);
    }
    NIXC_CATCH_ERRS_RES(false);
}

bsd_err bsd_store_real_path(
    bsd_c_context * context, Store * store, StorePath * path, bsd_get_string_callback callback, void * user_data)
{
    if (context)
        context->last_err_code = NIX_OK;
    try {
        auto res = store->ptr->toRealPath(path->path);
        return call_bsd_get_string_callback(res, callback, user_data);
    }
    NIXC_CATCH_ERRS
}

StorePath * bsd_store_parse_path(bsd_c_context * context, Store * store, const char * path)
{
    if (context)
        context->last_err_code = NIX_OK;
    try {
        bsd::StorePath s = store->ptr->parseStorePath(path);
        return new StorePath{std::move(s)};
    }
    NIXC_CATCH_ERRS_NULL
}

bsd_err bsd_store_realise(
    bsd_c_context * context,
    Store * store,
    StorePath * path,
    void * userdata,
    void (*callback)(void * userdata, const char *, const char *))
{
    if (context)
        context->last_err_code = NIX_OK;
    try {

        const std::vector<bsd::DerivedPath> paths{bsd::DerivedPath::Built{
            .drvPath = bsd::makeConstantStorePathRef(path->path), .outputs = bsd::OutputsSpec::All{}}};

        const auto bsdStore = store->ptr;
        auto results = bsdStore->buildPathsWithResults(paths, bsd::bmNormal, bsdStore);

        if (callback) {
            for (const auto & result : results) {
                for (const auto & [outputName, realisation] : result.builtOutputs) {
                    auto op = store->ptr->printStorePath(realisation.outPath);
                    callback(userdata, outputName.c_str(), op.c_str());
                }
            }
        }
    }
    NIXC_CATCH_ERRS
}

void bsd_store_path_name(const StorePath * store_path, bsd_get_string_callback callback, void * user_data)
{
    std::string_view name = store_path->path.name();
    callback(name.data(), name.size(), user_data);
}

void bsd_store_path_free(StorePath * sp)
{
    delete sp;
}

StorePath * bsd_store_path_clone(const StorePath * p)
{
    return new StorePath{p->path};
}

bsd_err bsd_store_copy_closure(bsd_c_context * context, Store * srcStore, Store * dstStore, StorePath * path)
{
    if (context)
        context->last_err_code = NIX_OK;
    try {
        bsd::RealisedPath::Set paths;
        paths.insert(path->path);
        bsd::copyClosure(*srcStore->ptr, *dstStore->ptr, paths);
    }
    NIXC_CATCH_ERRS
}
