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

#include "dynamic.h"
#include "fileversion.h"
#include "note.h"

#include <algorithm>
#include <boost/range/iterator_range_core.hpp>
#include <bitset>
#include <optional>
#include <vector>

class VolumeSwell
{
public:
    VolumeSwell() = default;
    VolumeSwell(VolumeLevel start, VolumeLevel end, int duration = 0);

    bool operator==(const VolumeSwell &other) const;

    template <class Archive>
    void serialize(Archive &ar, const FileVersion /*version*/);

    VolumeLevel getStartVolume() const { return myStartVolume; }
    VolumeLevel getEndVolume() const { return myEndVolume; }
    int getDuration() const { return myDuration; }

private:
    VolumeLevel myStartVolume = VolumeLevel::Off;
    VolumeLevel myEndVolume = VolumeLevel::Off;
    /// Number of additional notes the swell is played over.
    int myDuration = 0;
};

template <class Archive>
void
VolumeSwell::serialize(Archive &ar, const FileVersion /*version*/)
{
    ar("start_volume", myStartVolume);
    ar("end_volume", myEndVolume);
    ar("duration", myDuration);
}

class TremoloBar
{
public:
    enum class Type
    {
        Dip,
        DiveAndRelease,
        DiveAndHold,
        Release,
        ReturnAndRelease,
        ReturnAndHold,
        InvertedDip
    };

    TremoloBar() = default;
    TremoloBar(Type type, int pitch, int duration = 0);

    bool operator==(const TremoloBar &other) const;

    Type getType() const { return myType; }
    int getPitch() const { return myPitch; }
    int getDuration() const { return myDuration; }

    template <class Archive>
    void serialize(Archive &ar, const FileVersion /*version*/)
    {
        ar("type", myType);
        ar("pitch", myPitch);
        ar("duration", myDuration);
    }

private:
    Type myType = Type::Dip;
    int myPitch = 4;
    int myDuration = 0;
};

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

    /// Returns whether the position has a volume swell.
    bool hasVolumeSwell() const;
    /// Returns the volume swell for this position.
    const VolumeSwell &getVolumeSwell() const;
    /// Adds a volume swell to this position.
    void setVolumeSwell(const VolumeSwell &swell);
    /// Removes the volume swell for this position.
    void clearVolumeSwell();

    /// Returns whether the position has a tremolo bar event.
    bool hasTremoloBar() const;
    /// Returns the tremolo bar event for this position.
    const TremoloBar &getTremoloBar() const;
    /// Adds a tremolo bar event for this position.
    void setTremoloBar(const TremoloBar &bar);
    /// Removes the tremolo bar for this position.
    void clearTremoloBar();

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
    std::optional<VolumeSwell> myVolumeSwell;
    std::optional<TremoloBar> myTremoloBar;
    std::vector<Note> myNotes;
};

template <class Archive>
void Position::serialize(Archive &ar, const FileVersion version)
{
    ar("position", myPosition);
    ar("duration", myDurationType);
    ar("properties", mySimpleProperties);
    ar("multibar_rest", myMultiBarRestCount);

    if (version >= FileVersion::VOLUME_SWELLS)
        ar("volume_swell", myVolumeSwell);

    if (version >= FileVersion::TREMOLO_BAR)
        ar("tremolo_bar", myTremoloBar);

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
    Note *findByString(Position &pos, int string);
    bool hasNoteWithTappedHarmonic(const Position &pos);
    bool hasNoteWithArtificialHarmonic(const Position &pos);
    bool hasNoteWithTrill(const Position &pos);
    bool hasNoteWithBend(const Position &pos);
    bool hasNoteWithProperty(const Position &pos, Note::SimpleProperty property);
}

#endif
