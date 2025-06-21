#pragma once
/**
 * @file
 *
 * Hashing utilities for use with `std::unordered_map`, etc. (i.e. low
 * level implementation logic, not domain logic like Bsd hashing).
 */

#include <functional>

namespace bsd {

/**
 * `hash_combine()` from Boost. Hash several hashable values together
 * into a single hash.
 */
inline void hash_combine(std::size_t & seed) {}

template<typename T, typename... Rest>
inline void hash_combine(std::size_t & seed, const T & v, Rest... rest)
{
    std::hash<T> hasher;
    seed ^= hasher(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
    hash_combine(seed, rest...);
}

} // namespace bsd
