/*
  * Copyright (C) 2013 Cameron White
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

#ifndef SCORE_UTILS_H
#define SCORE_UTILS_H

#include <algorithm>
#include <boost/range/adaptor/filtered.hpp>
#include <boost/range/algorithm/lower_bound.hpp>
#include <boost/range/iterator_range_core.hpp>

namespace ScoreUtils {

namespace Detail
{
    /// Comparison functor to sort objects by their positions in the system.
    struct OrderByPosition
    {
        template <typename T>
        bool operator()(const T &obj1, const T &obj2) const
        {
            return obj1.getPosition() < obj2.getPosition();
        }

        template <typename T>
        bool operator()(const T &obj1, int position) const
        {
            return obj1.getPosition() < position;
        }
    };

    /// Equivalent of std::binary_search, but returning an iterator.
    /// The positions are required to be unique so equal_range() is not needed.
    template <typename Range>
    auto binarySearch(const Range &range, int position)
    {
        auto it = boost::range::lower_bound(range, position, OrderByPosition());
        if (it != range.end() && it->getPosition() == position)
            return it;
        else
            return range.end();
    }

    /// Functor to filter within a range of positions.
    struct InPositionRange
    {
        InPositionRange(int left, int right) : myLeft(left), myRight(right)
        {
        }

        template <typename T>
        bool operator()(const T &obj) const
        {
            return obj.getPosition() >= myLeft && obj.getPosition() <= myRight;
        }

    private:
        const int myLeft;
        const int myRight;
    };
} // namespace Detail

    /// Returns the object at the given position, or nullptr.
    template <typename T>
    typename T::pointer
    findByPosition(const boost::iterator_range<T> &range, int position)
    {
        auto it = Detail::binarySearch(range, position);
        return it != range.end() ? &*it : nullptr;
    }

    /// Returns the index of the object with the given position, or -1.
    template <typename T>
    int
    findIndexByPosition(const boost::iterator_range<T> &range, int position)
    {
        auto it = Detail::binarySearch(range, position);
        return it != range.end()
                   ? static_cast<int>(std::distance(range.begin(), it))
                   : -1;
    }

    /// Returns the objects within the specified position range (inclusive).
    template <typename Range>
    boost::filtered_range<Detail::InPositionRange, Range>
    findInRange(Range range, int left, int right)
    {
        return boost::adaptors::filter(range,
                                       Detail::InPositionRange(left, right));
    }

    /// Inserts the object, sorted by position.
    template <typename T, typename Y>
    void insertObject(std::vector<T> &objects, Y &&obj)
    {
        auto it = boost::range::lower_bound(objects, obj.getPosition(),
                                            Detail::OrderByPosition());
        if (it != objects.end() && it->getPosition() == obj.getPosition())
        {
            // Shouldn't insert duplicates!
            // However, it seems possible for some v1.7 files to have duplicate
            // objects so this assert produces some false positives.
            //assert(false);
            return;
        }

        objects.insert(it, std::forward<Y>(obj));
    }

    /// Removes the object from a sorted list.
    template <typename T>
    void removeObject(std::vector<T> &objects, const T &obj)
    {
        auto it = Detail::binarySearch(objects, obj.getPosition());
        assert(it != objects.end());
        assert(*it == obj);
        objects.erase(it);
    }
}

#endif
