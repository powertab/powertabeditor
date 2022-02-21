/*
  * Copyright (C) 2022 Cameron White
  *
  * This program is free software: you can redistribute it and/or modify
  * it under the terms of the GNU General Public License as published by
  * the Free Software Foundation, either version 3 of the License, or
  * (at your option) any later version.
  *
  * This program is distributed in the hope that it will be useful,
  * but WITHOUT ANY WARRANTY; without even the implied warranty of
  * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  * GNU General Public License for more details.
  *
  * You should have received a copy of the GNU General Public License
  * along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef UTIL_ENUMFLAGS_H
#define UTIL_ENUMFLAGS_H

#include <bitset>

namespace Util
{
/// Wrapper around std::bitset for bit flags defined by an enum.
template <typename EnumT>
class EnumFlags
{
public:
    static constexpr size_t NumFlags = static_cast<size_t>(EnumT::NumFlags);

    bool operator==(const EnumFlags<EnumT> &other) const
    {
        return myFlags == other.myFlags;
    }

    bool getFlag(EnumT flag) const
    {
        return myFlags.test(static_cast<size_t>(flag));
    }

    void setFlag(EnumT flag, bool enable)
    {
        myFlags.set(static_cast<size_t>(flag), enable);
    }

    /// Convert to an integer representation.
    unsigned long long toUInt() const { return myFlags.to_ullong(); }

    /// Set the flags from a string containing 0's and 1's
    void setFromString(const std::string &s)
    {
        myFlags = std::bitset<NumFlags>(s);
    }

private:
    std::bitset<NumFlags> myFlags;
};
} // namespace Util

#endif
