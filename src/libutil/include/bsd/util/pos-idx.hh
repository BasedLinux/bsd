#pragma once
///@file

#include <cinttypes>
#include <functional>

namespace bsd {

class PosIdx
{
    friend struct LazyPosAccessors;
    friend class PosTable;
    friend class std::hash<PosIdx>;

private:
    uint32_t id;

    explicit PosIdx(uint32_t id)
        : id(id)
    {
    }

public:
    PosIdx()
        : id(0)
    {
    }

    explicit operator bool() const
    {
        return id > 0;
    }

    auto operator<=>(const PosIdx other) const
    {
        return id <=> other.id;
    }

    bool operator==(const PosIdx other) const
    {
        return id == other.id;
    }

    size_t hash() const noexcept
    {
        return std::hash<uint32_t>{}(id);
    }
};

inline PosIdx noPos = {};

}

namespace std {

template<>
struct hash<bsd::PosIdx>
{
    std::size_t operator()(bsd::PosIdx pos) const noexcept
    {
        return pos.hash();
    }
};

} // namespace std
