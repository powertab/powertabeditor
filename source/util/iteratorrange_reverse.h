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

#ifndef UTIL_ITERATORRANGE_REVERSE_H
#define UTIL_ITERATORRANGE_REVERSE_H

namespace Util
{
namespace detail
{
    template <typename BaseIter>
    struct ReverseIterator
    {
        ReverseIterator(BaseIter it) : myIter(it)
        {
        }

        auto operator++()
        {
            // Reverse.
            return --myIter;
        }

        bool operator==(const ReverseIterator &other) const
        {
            return myIter == other.myIter;
        }

        bool operator!=(const ReverseIterator &other) const
        {
            return !operator==(other);
        }

        auto &operator*() { return *myIter; }
        auto &operator*() const { return *myIter; }

        BaseIter myIter;
    };
} // namespace detail

template <typename Range>
auto reverseRange(Range &&input)
{
    return IteratorRange(detail::ReverseIterator(std::prev(input.end())),
                         detail::ReverseIterator(std::prev(input.begin())));
}
}
#endif
