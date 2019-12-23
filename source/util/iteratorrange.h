/*
  * Copyright (C) 2019 Cameron White
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

#ifndef UTIL_ITERATORRANGE_H
#define UTIL_ITERATORRANGE_H

#include <cassert>
#include <iterator>

namespace Util
{
template <typename IterT>
class IteratorRange
{
public:
    /// Initialize the range from a container.
    template <typename Container>
    IteratorRange(Container &c)
        : myBegin(std::begin(c)), myEnd(std::end(c))
    {
    }

    /// Initialize the range from a const container.
    template <typename Container>
    IteratorRange(const Container &c)
        : myBegin(std::cbegin(c)), myEnd(std::cend(c))
    {
    }

    /// Initialize from a pair of iterators.
    IteratorRange(IterT begin, IterT end)
        : myBegin(begin), myEnd(end)
    {
    }

    IterT begin() const { return myBegin; }
    IterT end() const { return myEnd; }

    bool empty() const { return myBegin == myEnd; }

    auto &front() const { assert(!empty()); return *myBegin; }
    auto &front() { assert(!empty());return *myBegin; }

    auto &back() const { assert(!empty()); return *std::prev(myEnd); }
    auto &back() { assert(!empty()); return *std::prev(myEnd); }

    auto &operator[](int i) { return *(myBegin + i); }
    auto &operator[](int i) const { return *(myBegin + i); }

    ssize_t size() const
    {
        static_assert(std::is_same<
                      typename std::iterator_traits<IterT>::iterator_category,
                      std::random_access_iterator_tag>::value);
        return myEnd - myBegin;
    }

    template <typename OtherRange>
    bool operator==(const OtherRange &other) const
    {
        auto it1 = myBegin;
        auto it2 = std::begin(other);

        while (true)
        {
            if (it1 == myEnd && it2 == std::end(other))
                return true;

            if (it1 == myEnd || it2 == std::end(other))
                return false;

            if (!(*it1 == *it2))
                return false;

            ++it1;
            ++it2;
        }

        return false;
    }

private:
    IterT myBegin;
    IterT myEnd;
};

/// Deduction guides
/// @{
template <typename Container>
IteratorRange(Container &) -> IteratorRange<typename Container::iterator>;

template <typename Container>
IteratorRange(const Container &) ->
    IteratorRange<typename Container::const_iterator>;
/// @}

} // namespace Util

#endif

