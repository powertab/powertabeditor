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

#ifndef UTIL_ITERATORRANGE_FILTER_H
#define UTIL_ITERATORRANGE_FILTER_H

#include <iterator>

namespace Util
{
namespace detail
{
    template <typename BaseIter, typename Predicate>
    struct FilterIterator
    {
        using difference_type = typename BaseIter::difference_type;
        using value_type = typename BaseIter::value_type;
        using pointer = typename BaseIter::pointer;
        using reference = typename BaseIter::reference;
        using iterator_category = std::bidirectional_iterator_tag;

        FilterIterator(BaseIter it, BaseIter end, Predicate predicate)
            : myIter(it), myEnd(end), myPredicate(predicate)
        {
            // Ensure that we don't start on an invalid element.
            if (!myPredicate(*myIter))
                ++(*this);
        }

        FilterIterator &operator++()
        {
            do
            {
                ++myIter;
            } while (!myPredicate(*myIter) && myIter != myEnd);
            return *this;
        }

        FilterIterator &operator--()
        {
            do
            {
                --myIter;
            } while (!myPredicate(*myIter));
            return *this;
        }

        bool operator==(const FilterIterator &other) const
        {
            return myIter == other.myIter;
        }

        bool operator!=(const FilterIterator &other) const
        {
            return !operator==(other);
        }

        reference operator*() { return *myIter; }
        const reference operator*() const { return *myIter; }

        BaseIter myIter;
        BaseIter myEnd;
        Predicate myPredicate;
    };
} // namespace detail

template <typename Range, typename Predicate>
auto filterRange(Range &&input, Predicate f)
{
    return IteratorRange(detail::FilterIterator(input.begin(), input.end(), f),
                         detail::FilterIterator(input.end(), input.end(), f));
}
}
#endif
