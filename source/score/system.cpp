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

#include "system.h"

#include <boost/foreach.hpp>

namespace Score {

System::System()
{
    // Add the start and end bars.
    myBarlines.push_back(Barline());
    Barline endBar;
    endBar.setPosition(std::numeric_limits<int>::max());
    myBarlines.push_back(endBar);
}

boost::iterator_range<System::BarlineIterator> System::getBarlines()
{
    return boost::make_iterator_range(myBarlines);
}

boost::iterator_range<System::BarlineConstIterator> System::getBarlines() const
{
    return boost::make_iterator_range(myBarlines);
}

struct OrderByPosition
{
    bool operator()(const Barline &bar1, const Barline &bar2) const
    {
        return bar1.getPosition() < bar2.getPosition();
    }
};

void System::insertBarline(const Barline &barline)
{
    myBarlines.push_back(barline);
    std::sort(myBarlines.begin(), myBarlines.end(), OrderByPosition());
}

void System::removeBarline(const Barline &barline)
{
    myBarlines.erase(std::remove(myBarlines.begin(), myBarlines.end(), barline),
                     myBarlines.end());
}

const Barline *System::getBarlineAtPosition(int position) const
{
    BOOST_FOREACH(const Barline &barline, myBarlines)
    {
        if (barline.getPosition() == position)
            return &barline;
    }

    return NULL;
}

Barline *System::getBarlineAtPosition(int position)
{
    return const_cast<Barline *>(
            const_cast<const System &>(*this).getBarlineAtPosition(position));
}

const Barline *System::getPreviousBarline(int position) const
{
    BOOST_REVERSE_FOREACH(const Barline &barline, myBarlines)
    {
        if (barline.getPosition() <= position)
            return &barline;
    }

    return NULL;
}

const Barline *System::getNextBarline(int position) const
{
    BOOST_FOREACH(const Barline &barline, myBarlines)
    {
        if (barline.getPosition() >= position)
            return &barline;
    }

    return NULL;
}

}
