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

#ifndef FORMATS_POWERTABOLDIMPORTER_H
#define FORMATS_POWERTABOLDIMPORTER_H

#include <boost/shared_ptr.hpp>
#include <formats/fileformatmanager.h>

namespace PowerTabDocument {
class AlternateEnding;
class Barline;
class Direction;
class Guitar;
class KeySignature;
class PowerTabFileHeader;
class RehearsalSign;
class Score;
class Staff;
class System;
class TempoMarker;
class TimeSignature;
class Tuning;
}

class AlternateEnding;
class Barline;
class Direction;
class KeySignature;
class RehearsalSign;
class ScoreInfo;
class Staff;
class System;
class TempoMarker;
class TimeSignature;
class Tuning;

class PowerTabOldImporter : public FileFormatImporter
{
public:
    PowerTabOldImporter();
    virtual void load(const std::string &filename, Score &score);

private:
    static void convert(const PowerTabDocument::PowerTabFileHeader &header,
                        ScoreInfo &info);
    static void convert(const PowerTabDocument::Score &oldScore,
                        Score &score);

    static void convert(const PowerTabDocument::Guitar &guitar, Score &score);
    static void convert(const PowerTabDocument::Tuning &oldTuning,
                        Tuning &tuning);

    static void convert(const PowerTabDocument::Score &oldScore,
                        boost::shared_ptr<const PowerTabDocument::System> oldSystem,
                        System &system);

    static void convert(const PowerTabDocument::Barline &oldBar, Barline &bar);
    static void convert(const PowerTabDocument::RehearsalSign &oldSign,
                        RehearsalSign &sign);
    static void convert(const PowerTabDocument::KeySignature &oldKey,
                        KeySignature &key);
    static void convert(const PowerTabDocument::TimeSignature &oldTime,
                        TimeSignature &time);

    static void convert(const PowerTabDocument::TempoMarker &oldTempo,
                        TempoMarker &tempo);
    static void convert(const PowerTabDocument::AlternateEnding &oldEnding,
                        AlternateEnding &ending);
    static void convert(const PowerTabDocument::Direction &oldDirection,
                        Direction &direction);

    static void convert(const PowerTabDocument::Staff &oldStaff,
                        Staff &staff);
};

#endif
