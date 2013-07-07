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

#ifndef SCORE_BARLINE_H
#define SCORE_BARLINE_H

#include <boost/optional.hpp>
#include <boost/serialization/access.hpp>
#include "rehearsalsign.h"

namespace Score {

class Barline
{
public:
    Barline();

    bool operator==(const Barline &other) const;

    /// Returns the rehearsal sign for the bar, if one exists.
    const boost::optional<RehearsalSign> &getRehearsalSign() const;
    /// Sets (or clears) the rehearsal sign for the bar.
    void setRehearsalSign(const boost::optional<RehearsalSign> &sign);

private:
    boost::optional<RehearsalSign> myRehearsalSign;

    friend class boost::serialization::access;
    template <class Archive>
    void serialize(Archive &ar, const unsigned int /*version*/)
    {
        ar & myRehearsalSign;
    }
};

}

#endif

