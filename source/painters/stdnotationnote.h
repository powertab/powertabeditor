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

#ifndef PAINTERS_STDNOTATIONNOTE_H
#define PAINTERS_STDNOTATIONNOTE_H

#include <boost/cstdint.hpp>
#include <painters/beamgroup.h>
#include <painters/notestem.h>
#include <QChar>
#include <vector>

struct LayoutInfo;
class KeySignature;
class Note;
class Position;
class Score;
class Staff;
class System;
class TimeSignature;
class Tuning;

class StdNotationNote
{
public:
    enum AccidentalType
    {
        NoAccidental,
        Natural,
        Sharp,
        DoubleSharp,
        Flat,
        DoubleFlat
    };

    StdNotationNote(const Position &pos, const Note &note,
                    const KeySignature &key, const Tuning &tuning, double y);

    static void getNotesInStaff(const Score &score, const System &system,
                                int systemIndex, const Staff &staff,
                                int staffIndex, const LayoutInfo &layout,
                                std::vector<StdNotationNote> &notes,
                                std::vector<BeamGroup> &groups);

    double getY() const;
    QChar getNoteHeadSymbol() const;
    int getPosition() const;
    AccidentalType getAccidentalType() const;
    QString getAccidentalText() const;
    bool isDotted() const;
    bool isDoubleDotted() const;

    /// Remove the note's accidental.
    void clearAccidental();
    /// Force the accidental to be shown, even if it's part of the key signature.
    void showAccidental();

private:
    /// Return the offset of the note from the top of the staff.
    static double getNoteLocation(const Staff &staff, const Note &note,
                                  const KeySignature &key, const Tuning &tuning);

    /// Returns the number of octaves (from -2 to 2) that the note is shifted by.
    static int getOctaveOffset(const Note &note);

    /// Computes the accidental for the note.
    /// @param explicitSymbol If true, an accidental or natural sign will be
    /// displayed even if the note is in the key signature.
    void computeAccidentalType(bool explicitSymbol);

    /// Returns the non-zero beaming patterns of the time signature.
    static std::vector<uint8_t> getBeamingPatterns(const TimeSignature &timeSig);

    /// Calculates the beaming for a set of note stems.
    static void computeBeaming(const LayoutInfo &layout,
                               const TimeSignature &timeSig,
                               const std::vector<NoteStem> &stems,
                               std::vector<BeamGroup> &groups);

    /// A group may be split into several beam groups if there are rests,
    /// whole notes, etc.
    static void computeBeamingGroups(const LayoutInfo &layout,
                                     const std::vector<NoteStem> &stems,
                                     std::vector<BeamGroup> &groups);

    double myY;
    QChar myNoteHeadSymbol;
    AccidentalType myAccidentalType;

    const Position *myPosition;
    const Note *myNote;
    const KeySignature *myKey;
    const Tuning *myTuning;
};

#endif
