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

#ifndef SCORE_SYSTEM_H
#define SCORE_SYSTEM_H

#include "alternateending.h"
#include "barline.h"
#include "chordtext.h"
#include "direction.h"
#include "fileversion.h"
#include "playerchange.h"
#include "staff.h"
#include "tempomarker.h"
#include "textitem.h"
#include <span>
#include <vector>

class System
{
public:
    System();

    bool operator==(const System &other) const;

    template <class Archive>
    void serialize(Archive &ar, const FileVersion version);

    /// Returns the set of staves in the system.
    std::span<Staff> getStaves() { return myStaves; }
    /// Returns the set of staves in the system.
    std::span<const Staff> getStaves() const { return myStaves; }

    /// Adds a new staff to the system.
    void insertStaff(const Staff &staff);
    void insertStaff(Staff &&staff);
    void insertStaff(const Staff &staff, int index);
    /// Removes the specified staff from the system.
    void removeStaff(int index);

    /// Returns the set of barlines in the system.
    std::span<Barline> getBarlines() { return myBarlines; }
    /// Returns the set of barlines in the system.
    std::span<const Barline> getBarlines() const { return myBarlines; }

    /// Adds a new barline to the system.
    void insertBarline(const Barline &barline);
    /// Removes the specified barline from the system.
    void removeBarline(const Barline &barline);

    /// Returns the last barline before the given position.
    const Barline *getPreviousBarline(int position) const;
    /// Returns the first barline after the given position.
    const Barline *getNextBarline(int position) const;
    Barline *getNextBarline(int position);

    /// Returns the set of tempo markers in the system.
    std::span<TempoMarker> getTempoMarkers() { return myTempoMarkers; }
    /// Returns the set of tempo markers in the system.
    std::span<const TempoMarker> getTempoMarkers() const { return myTempoMarkers; }

    /// Adds a new tempo marker to the system.
    void insertTempoMarker(const TempoMarker &marker);
    /// Removes the specified tempo marker from the system.
    void removeTempoMarker(const TempoMarker &marker);

    /// Returns the set of alternate endings in the system.
    std::span<AlternateEnding> getAlternateEndings() { return myAlternateEndings; }
    /// Returns the set of alternate endings in system.
    std::span<const AlternateEnding> getAlternateEndings() const { return myAlternateEndings; }

    /// Adds a new alternate ending to the system.
    void insertAlternateEnding(const AlternateEnding &ending);
    /// Removes the specified alternate ending from the system.
    void removeAlternateEnding(const AlternateEnding &ending);

    /// Returns the set of musical directions in the system.
    std::span<Direction> getDirections() { return myDirections; }
    /// Returns the set of musical directions in system.
    std::span<const Direction> getDirections() const { return myDirections; }

    /// Adds a new musical direction to the system.
    void insertDirection(const Direction &direction);
    /// Removes the specified musical direction from the system.
    void removeDirection(const Direction &direction);

    /// Returns the set of player changes in the system.
    std::span<PlayerChange> getPlayerChanges() { return myPlayerChanges; }
    /// Returns the set of player changes in system.
    std::span<const PlayerChange> getPlayerChanges() const { return myPlayerChanges; }

    /// Adds a new player change to the system.
    void insertPlayerChange(const PlayerChange &change);
    /// Removes the specified player change from the system.
    void removePlayerChange(const PlayerChange &change);

    /// Returns the set of chord symbols in the system.
    std::span<ChordText> getChords() { return myChords; }
    /// Returns the set of chord symbols in system.
    std::span<const ChordText> getChords() const { return myChords; }

    /// Adds a new chord symbol to the system.
    void insertChord(const ChordText &chord);
    /// Removes the specified chord symbol from the system.
    void removeChord(const ChordText &chord);

    /// Returns the set of text items in the system.
    std::span<TextItem> getTextItems() { return myTextItems; }
    /// Returns the set of text items in system.
    std::span<const TextItem> getTextItems() const { return myTextItems; }

    /// Adds a new text item to the system.
    void insertTextItem(const TextItem &text);
    /// Removes the specified text item from the system.
    void removeTextItem(const TextItem &text);

private:
    std::vector<Staff> myStaves;
    /// List of the barlines in the system. This will always contain at least
    /// two barlines - the start and end bars.
    std::vector<Barline> myBarlines;
    std::vector<TempoMarker> myTempoMarkers;
    std::vector<AlternateEnding> myAlternateEndings;
    std::vector<Direction> myDirections;
    std::vector<PlayerChange> myPlayerChanges;
    std::vector<ChordText> myChords;
    std::vector<TextItem> myTextItems;
};

template <class Archive>
void System::serialize(Archive &ar, const FileVersion version)
{
    ar("staves", myStaves);
    ar("barlines", myBarlines);
    ar("tempo_markers", myTempoMarkers);
    ar("alternate_endings", myAlternateEndings);
    ar("directions", myDirections);
    ar("player_changes", myPlayerChanges);
    ar("chords", myChords);
    if (version >= FileVersion::TEXT_ITEMS)
        ar("text_items", myTextItems);
}

namespace SystemUtils {

/// Shifts everything forward starting from the given position.
void shiftForward(System &system, int position);
/// Shifts everything backward starting from the given position.
void shiftBackward(System &system, int position);
/// Shifts everything by the given offset.
void shift(System &system, int position, int offset);

/// Returns the start and end barline around the given position, assuming that
/// the position index is valid.
std::pair<const Barline &, const Barline &> getSurroundingBarlines(
    const System &system, int position);
}

#endif
