#include <algorithm>
#include <codecvt>
#include <iostream>
#include <locale>

#include "bsd/util/file-path.hh"
#include "bsd/util/util.hh"

namespace bsd {

std::optional<std::filesystem::path> maybePath(PathView path)
{
    return { path };
}

std::filesystem::path pathNG(PathView path)
{
    return path;
}

}
