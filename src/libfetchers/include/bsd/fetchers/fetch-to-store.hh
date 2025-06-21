#pragma once

#include "bsd/util/source-path.hh"
#include "bsd/store/store-api.hh"
#include "bsd/util/file-system.hh"
#include "bsd/util/repair-flag.hh"
#include "bsd/util/file-content-address.hh"
#include "bsd/fetchers/cache.hh"

namespace bsd {

enum struct FetchMode { DryRun, Copy };

/**
 * Copy the `path` to the Bsd store.
 */
StorePath fetchToStore(
    const fetchers::Settings & settings,
    Store & store,
    const SourcePath & path,
    FetchMode mode,
    std::string_view name = "source",
    ContentAddressMethod method = ContentAddressMethod::Raw::BsdArchive,
    PathFilter * filter = nullptr,
    RepairFlag repair = NoRepair);

fetchers::Cache::Key makeFetchToStoreCacheKey(
    const std::string & name, const std::string & fingerprint, ContentAddressMethod method, const std::string & path);

}
