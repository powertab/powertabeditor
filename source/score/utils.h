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
#include <boost/range/iterator_range_core.hpp>

namespace ScoreUtils {

    /// Returns the object at the given position index, or null.
    template <typename T>
    typename T::pointer findByPosition(const boost::iterator_range<T> &range,
                                       int position)
    {
        size_t n = range.size();
        for (size_t i = 0; i < n; ++i)
        {
            if (range[i].getPosition() == position)
                return &range[i];
        }

        return nullptr;
    }

    template <typename T>
    int findIndexByPosition(const boost::iterator_range<T> &range, int position)
    {
        const int n = static_cast<int>(range.size());
        for (int i = 0; i < n; ++i)
        {
            if (range[i].getPosition() == position)
                return i;
        }

        return -1;
    }

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

    template <typename Range>
    boost::filtered_range<InPositionRange, Range> findInRange(Range range,
                                                              int left,
                                                              int right)
    {
        return boost::adaptors::filter(
            range, InPositionRange(left, right));
    }

    // Some helper methods to reduce code duplication.

    /// Sorts objects by their positions in the system.
    template <typename T>
    struct OrderByPosition
    {
        bool operator()(const T &obj1, const T &obj2) const
        {
            return obj1.getPosition() < obj2.getPosition();
        }
    };

    template <typename T>
    void insertObject(std::vector<T> &objects, const T &obj)
    {
        // Avoid sorting unless we actually need to. This improves performance
        // quite a bit when, for example, we are importing from other file
        // formats and inserting objects in order.
        const bool needsSort = !objects.empty() &&
                objects.back().getPosition() > obj.getPosition();

        objects.push_back(obj);
        if (needsSort)
            std::sort(objects.begin(), objects.end(), OrderByPosition<T>());
    }

    template <typename T>
    void removeObject(std::vector<T> &objects, const T &obj)
    {
        objects.erase(std::remove(objects.begin(), objects.end(), obj),
                      objects.end());
    }
}

#endif
