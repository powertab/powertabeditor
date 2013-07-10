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

#include "staff.h"

#include "utils.h"

namespace Score {

using namespace Detail;

Staff::Staff()
{
}

bool Staff::operator==(const Staff &other) const
{
    return myDynamics == other.myDynamics;
}

boost::iterator_range<Staff::DynamicIterator> Staff::getDynamics()
{
    return boost::make_iterator_range(myDynamics);
}

boost::iterator_range<Staff::DynamicConstIterator> Staff::getDynamics() const
{
    return boost::make_iterator_range(myDynamics);
}

void Staff::insertDynamic(const Dynamic &dynamic)
{
    insertObject(myDynamics, dynamic);
}

void Staff::removeDynamic(const Dynamic &dynamic)
{
    removeObject(myDynamics, dynamic);
}

}
