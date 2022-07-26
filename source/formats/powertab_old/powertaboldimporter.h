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

#include <formats/fileformat.h>
#include <memory>

#include <unordered_map>

namespace PowerTabDocument {
class AlternateEnding;
class Barline;
class ChordText;
class Direction;
class Dynamic;
class Guitar;
class KeySignature;
class Note;
class Position;
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
class ChordDiagram;
class ChordName;
class ChordText;
class Direction;
class Dynamic;
class KeySignature;
class Note;
class Position;
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
    virtual void load(const std::filesystem::path &filename,
                      Score &score) override;

private:
    static void convert(const PowerTabDocument::PowerTabFileHeader &header,
                        ScoreInfo &info);
    static void convert(const PowerTabDocument::Score &oldScore,
                        Score &score);

    static void convert(const PowerTabDocument::Guitar &guitar, Score &score);
    static void convert(const PowerTabDocument::Tuning &oldTuning,
                        Tuning &tuning);

    using ChordDiagramMap = std::unordered_map<ChordName, ChordDiagram>;
    static void convert(
        const PowerTabDocument::Score &oldScore,
        std::shared_ptr<const PowerTabDocument::System> oldSystem,
        const ChordDiagramMap &chord_diagrams,
        Score &score, System &system);

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
    static void convert(const PowerTabDocument::ChordText &oldChord,
                        ChordText &chord);

    typedef std::shared_ptr<PowerTabDocument::Dynamic> DynamicPtr;
    static int convert(const PowerTabDocument::Staff &oldStaff,
                        const std::vector<DynamicPtr> &dynamics,
                        Staff &staff);

    static void convert(const PowerTabDocument::Position &oldPosition,
                        Position &position);
    static void convert(const PowerTabDocument::Note &oldNote,
                        Note &note);

    static void convertGuitarIns(const PowerTabDocument::Score &oldScore,
                                 Score &score);
    static void convertInitialVolumes(const PowerTabDocument::Score &oldScore,
                                      Score &score);
    static void convertFloatingText(const PowerTabDocument::Score &oldScore,
                                    Score &score);

    static void merge(Score &score1, Score &score2);
};

#endif
