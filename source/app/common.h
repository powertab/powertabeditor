#ifndef COMMON_H
#define COMMON_H

#include <algorithm>

/// Returns the value, brought into the range [low, high]
template <typename T>
inline T clamp (const T& value, const T& low, const T& high)
{
    return std::max(std::min(value, high), low);
}

#endif // COMMON_H
