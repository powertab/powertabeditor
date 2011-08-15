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
  
#ifndef REPEAT_H
#define REPEAT_H

#include <boost/cstdint.hpp>
#include <powertabdocument/systemlocation.h>
#include <boost/unordered_map.hpp>
#include <boost/shared_ptr.hpp>

class AlternateEnding;

/// Represents a repeat end bar
class RepeatEnd
{
public:
    RepeatEnd();
    RepeatEnd(uint8_t repeatCount);

    bool performRepeat();

    void reset();

private:
    uint8_t repeatCount;
    uint8_t remainingRepeats;
};

/// Contains all information about a repeat - start bar, end bar(s), alternate endings, repeat count, etc
class Repeat
{
public:
    Repeat();
    Repeat(const SystemLocation& startBarLocation);

    void addRepeatEnd(const SystemLocation& location, const RepeatEnd& endBar);
    void addAlternateEnding(boost::shared_ptr<const AlternateEnding> altEnding);

    uint8_t getActiveRepeat() const;

    void reset();

    SystemLocation performRepeat(const SystemLocation& location);

private:
    typedef boost::unordered_map<SystemLocation, RepeatEnd, boost::hash<SystemLocation> > EndBarsMap;
    EndBarsMap endBars;

    typedef boost::unordered_map<uint8_t, SystemLocation> AltEndingsMap;
    AltEndingsMap alternateEndings;

    SystemLocation startBarLocation;
    uint8_t activeRepeat;
};

#endif // REPEAT_H
