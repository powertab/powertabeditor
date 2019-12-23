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

#ifndef SCORE_POSITION_H
#define SCORE_POSITION_H

#include <algorithm>
#include <boost/range/iterator_range_core.hpp>
#include <bitset>
#include "fileversion.h"
#include "note.h"
#include <vector>

class Position
{
public:
    typedef std::vector<Note>::iterator NoteIterator;
    typedef std::vector<Note>::const_iterator NoteConstIterator;

    enum DurationType
    {
        WholeNote = 1,
        HalfNote = 2,
        QuarterNote = 4,
        EighthNote = 8,
        SixteenthNote = 16,
        ThirtySecondNote = 32,
        SixtyFourthNote = 64
    };

    enum SimpleProperty
    {
        Dotted,
        DoubleDotted,
        Rest,
        Vibrato,
        WideVibrato,
        ArpeggioUp,
        ArpeggioDown,
        PickStrokeUp,
        PickStrokeDown,
        Staccato,
        Marcato,
        Sforzando,
        TremoloPicking,
        PalmMuting,
        Tap,
        Acciaccatura,
        TripletFeelFirst,
        TripletFeelSecond,
        LetRing,
        Fermata,
        NumSimpleProperties
    };

    Position();
    explicit Position(int position, DurationType duration = EighthNote);

    bool operator==(const Position &other) const;

    template <class Archive>
    void serialize(Archive &ar, const FileVersion version);

    /// Returns the position within the staff where the position is anchored.
    int getPosition() const;
    /// Sets the position within the staff where the position is anchored.
    void setPosition(int position);

    /// Returns the position's duration type (e.g. half note).
    DurationType getDurationType() const;
    /// Sets the position's duration type (e.g. half note).
    void setDurationType(DurationType duration);

    /// Returns whether the position has vibrato, palm muting, etc.
    bool hasProperty(SimpleProperty property) const;
    /// Sets whether the position has vibrato, palm muting, etc.
    void setProperty(SimpleProperty property, bool set = true);

    /// Returns whether the position is a rest.
    bool isRest() const;
    /// Sets whether the position is a rest. If a rest is being set, all
    /// existing notes will be removed.
    void setRest(bool set = true);

    /// Returns whether the position has a multi-bar rest.
    bool hasMultiBarRest() const;
    /// Returns the number of bars to rest for.
    int getMultiBarRestCount() const;
    /// Sets the number of bars to rest for.
    void setMultiBarRest(int count);
    /// Clears the multi-bar rest for this position.
    void clearMultiBarRest();

    /// Returns the set of notes in the position.
    boost::iterator_range<NoteIterator> getNotes();
    /// Returns the set of notes in the position.
    boost::iterator_range<NoteConstIterator> getNotes() const;

    /// Adds a new note to the position.
    void insertNote(const Note &note);
    /// Removes any notes that satisfy the given predicate.
    template <class Predicate>
    void removeNotes(Predicate p);
    /// Removes the specified note from the position.
    void removeNote(const Note &note);

private:
    int myPosition;
    DurationType myDurationType;
    std::bitset<NumSimpleProperties> mySimpleProperties;
    int myMultiBarRestCount;
    std::vector<Note> myNotes;
};

template <class Archive>
void Position::serialize(Archive &ar, const FileVersion /*version*/)
{
    ar("position", myPosition);
    ar("duration", myDurationType);
    ar("properties", mySimpleProperties);
    ar("multibar_rest", myMultiBarRestCount);
    ar("notes", myNotes);
}

template <class Predicate>
void Position::removeNotes(Predicate p)
{
    myNotes.erase(std::remove_if(myNotes.begin(), myNotes.end(), p),
                  myNotes.end());
}

namespace Utils {
    const Note *findByString(const Position &pos, int string);
    bool hasNoteWithTappedHarmonic(const Position &pos);
    bool hasNoteWithArtificialHarmonic(const Position &pos);
    bool hasNoteWithTrill(const Position &pos);
    bool hasNoteWithBend(const Position &pos);
    bool hasNoteWithProperty(const Position &pos, Note::SimpleProperty property);
}

#endif
