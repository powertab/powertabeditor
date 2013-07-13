/*
  * Copyright (C) 2011 Cameron White
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
  
#ifndef COMMON_H
#define COMMON_H

#include <algorithm>
#include <vector>

#include <boost/function.hpp>
#include <boost/iterator/indirect_iterator.hpp>
#include <boost/make_shared.hpp>

/// Helper function - clones a shared_ptr
template <typename T>
boost::shared_ptr<T> clone_shared_ptr(const boost::shared_ptr<T>& ptr)
{
    return boost::make_shared<T>(*ptr);
}

template <typename T>
struct CompareSharedPtr
{
    bool operator()(const boost::shared_ptr<const T>& ptr1,
                    const boost::shared_ptr<const T>& ptr2)
    {
        return *ptr1 < *ptr2;
    }
};

/// Performs a deep copy of a vector of shared_ptr
template <typename T>
void deepCopy(const std::vector<boost::shared_ptr<T> >& src,
              std::vector<boost::shared_ptr<T> >& dest)
{
    dest.clear();
    dest.reserve(src.size());
    
    std::transform(src.begin(), src.end(),
                   std::back_inserter(dest),
                   clone_shared_ptr<T>);
}

/// Tests equality for a vector of pointers, by comparing the pointed-to elements
template <typename T>
bool isDeepEqual(const std::vector<T>& vec1, const std::vector<T>& vec2)
{
    if (vec1.size() != vec2.size())
    {
        return false;
    }

    using boost::make_indirect_iterator;
    return std::equal(make_indirect_iterator(vec1.begin()),
                      make_indirect_iterator(vec1.end()),
                      make_indirect_iterator(vec2.begin()));
}

/// Shifts the position for an object (must provide GetPosition() and
/// SetPosition(uint32_t) functions).
/// The object is constructed with a position comparison function, a position
/// index, and an offset.
/// The object's position is compared with the supplied index using the given
/// function, and is adjusted by the offset if the comparison function is true.
template <class T>
struct ShiftPosition
{
    typedef boost::function<bool (uint32_t, uint32_t)> PositionIndexComparison;

    ShiftPosition(PositionIndexComparison comparePositions, uint32_t positionIndex, int offset) :
        comparePositions(comparePositions),
        positionIndex(positionIndex),
        offset(offset)
    {
    }

    void operator()(const T& item)
    {
        if (comparePositions(item->GetPosition(), positionIndex))
        {
            item->SetPosition(item->GetPosition() + offset);
        }
    }

    PositionIndexComparison comparePositions;
    uint32_t positionIndex;
    int offset;
};

#endif // COMMON_H
