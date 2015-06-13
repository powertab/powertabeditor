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

#include <array>
#include <painters/beamgroup.h>
#include <painters/notestem.h>
#include <QChar>
#include <score/staff.h>
#include <vector>

struct LayoutInfo;
class KeySignature;
class Score;
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

    StdNotationNote(const Voice &voice, const Position &pos, const Note &note,
                    const KeySignature &key, const Tuning &tuning, double y,
                    const boost::optional<int> &tie);

    static void getNotesInStaff(
        const Score &score, const System &system, int systemIndex,
        const Staff &staff, int staffIndex, const LayoutInfo &layout,
        std::vector<StdNotationNote> &notes,
        std::array<std::vector<NoteStem>, Staff::NUM_VOICES> &stemsByVoice,
        std::array<std::vector<BeamGroup>, Staff::NUM_VOICES> &groupsByVoice);

    double getY() const;
    QChar getNoteHeadSymbol() const;
    bool isGraceNote() const;
    int getPosition() const;
    AccidentalType getAccidentalType() const;
    QString getAccidentalText() const;
    bool isDotted() const;
    bool isDoubleDotted() const;

    /// Remove the note's accidental.
    void clearAccidental();
    /// Force the accidental to be shown, even if it's part of the key signature.
    void showAccidental();

    const boost::optional<int> &getTie() const;
    const Voice &getVoice() const;

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
    static void computeBeaming(const TimeSignature &timeSig,
                               std::vector<NoteStem> &stems,
                               size_t firstStemIndex,
                               std::vector<BeamGroup> &groups);

    /// A group may be split into several beam groups if there are rests,
    /// whole notes, etc.
    static void computeBeamingGroups(
        std::vector<NoteStem> &stems, const std::vector<double> &durations,
        const boost::optional<double> &subgroupLength,
        size_t firstStemIndexInBar, size_t firstStemIndex, size_t lastStemIndex,
        std::vector<BeamGroup> &groups);

    double myY;
    QChar myNoteHeadSymbol;
    AccidentalType myAccidentalType;

    const Voice &myVoice;
    const Position *myPosition;
    const Note *myNote;
    const KeySignature *myKey;
    const Tuning *myTuning;
    const boost::optional<int> myTie;
};

#endif
