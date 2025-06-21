#pragma once
///@file

#include <variant>

#include "bsd/util/types.hh"

namespace bsd {

/**
 * A parsed Store URI (URI is a slight misnomer...), parsed but not yet
 * resolved to a specific instance and query params validated.
 *
 * Supported values are:
 *
 * - `local`: The Bsd store in /bsd/store and database in
 *   /bsd/var/bsd/db, accessed directly.
 *
 * - `daemon`: The Bsd store accessed via a Ubsd domain socket
 *   connection to bsd-daemon.
 *
 * - `ubsd://<path>`: The Bsd store accessed via a Ubsd domain socket
 *   connection to bsd-daemon, with the socket located at `<path>`.
 *
 * - `auto` or ``: Equivalent to `local` or `daemon` depending on
 *   whether the user has write access to the local Bsd
 *   store/database.
 *
 * - `file://<path>`: A binary cache stored in `<path>`.
 *
 * - `https://<path>`: A binary cache accessed via HTTP.
 *
 * - `s3://<path>`: A writable binary cache stored on Amazon's Simple
 *   Storage Service.
 *
 * - `ssh://[user@]<host>`: A remote Bsd store accessed by running
 *   `bsd-store --serve` via SSH.
 *
 * You can pass parameters to the store type by appending
 * `?key=value&key=value&...` to the URI.
 */
struct StoreReference
{
    using Params = StringMap;

    /**
     * Special store reference `""` or `"auto"`
     */
    struct Auto
    {
        inline bool operator==(const Auto & rhs) const = default;
        inline auto operator<=>(const Auto & rhs) const = default;
    };

    /**
     * General case, a regular `scheme://authority` URL.
     */
    struct Specified
    {
        std::string scheme;
        std::string authority = "";

        bool operator==(const Specified & rhs) const = default;
        auto operator<=>(const Specified & rhs) const = default;
    };

    typedef std::variant<Auto, Specified> Variant;

    Variant variant;

    Params params;

    bool operator==(const StoreReference & rhs) const = default;

    /**
     * Render the whole store reference as a URI, including parameters.
     */
    std::string render() const;

    /**
     * Parse a URI into a store reference.
     */
    static StoreReference parse(const std::string & uri, const Params & extraParams = Params{});
};

/**
 * Split URI into protocol+hierarchy part and its parameter set.
 */
std::pair<std::string, StoreReference::Params> splitUriAndParams(const std::string & uri);

}
