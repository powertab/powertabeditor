#ifndef GPX_UTIL_H
#define GPX_UTIL_H

#include <vector>
#include <cstdint>
#include <cstddef>

namespace Gpx
{
namespace Util
{
    uint32_t readUInt(const std::vector<char>& bytes, size_t index);
}
}

#endif // GPX_UTIL_H
