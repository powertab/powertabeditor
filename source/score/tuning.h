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

#ifndef SCORE_TUNING_H
#define SCORE_TUNING_H

#include <boost/cstdint.hpp>
#include <iosfwd>
#include "fileversion.h"
#include <string>
#include <vector>

class Tuning
{
public:
    Tuning();

    bool operator==(const Tuning &other) const;

	template <class Archive>
	void serialize(Archive &ar, const FileVersion version);

    /// Returns the tuning name (e.g. "Open G").
    const std::string &getName() const;
    /// Sets the tuning name (e.g. "Open G").
    void setName(const std::string &name);

    /// Determines if a number of tuning strings is valid.
    static bool isValidStringCount(int count);
    /// Returns the number of strings in the tuning.
    int getStringCount() const;
    /// Returns the tuning of a specific string.
    uint8_t getNote(int string, bool includeMusicNotationOffset) const;
    /// Returns all of the tuning notes.
    std::vector<uint8_t> getNotes() const;
    /// Sets the tuning of a specific string.
    void setNote(int string, uint8_t note);
    /// Sets all of the notes of the tuning.
    void setNotes(const std::vector<uint8_t> &notes);

    /// Returns the music notation offset for the tuning.
    int8_t getMusicNotationOffset() const;
    /// Sets the music notation offset for the tuning.
    void setMusicNotationOffset(int8_t offset);

    /// Returns whether the tuning is displayed using sharps or flats.
    bool usesSharps() const;
    /// Sets whether the tuning is displayed using sharps or flats.
    void setSharps(bool set = true);

    /// Returns the capo for the tuning (0 == no capo).
    int getCapo() const;
    /// Sets the capo for the tuning.
    void setCapo(int capo);

    /// Minimum allowed number of strings in a tuning.
    static const int MIN_STRING_COUNT;
    /// Maximum allowed number of strings in a tuning.
    static const int MAX_STRING_COUNT;

    /// Minimum valid capo fret.
    static const int MIN_CAPO;
    /// Maximum valid capo fret.
    static const int MAX_CAPO;

    static const int8_t MIN_MUSIC_NOTATION_OFFSET;
    static const int8_t MAX_MUSIC_NOTATION_OFFSET;

private:
    std::string myName;
    std::vector<uint8_t> myNotes;
    int8_t myMusicNotationOffset;
    bool myUsesSharps;
    int myCapo;
};

template <class Archive>
void Tuning::serialize(Archive &ar, const FileVersion version)
{
	ar("name", myName);
	ar("notes", myNotes);
	ar("offset", myMusicNotationOffset);
	ar("sharps", myUsesSharps);
	ar("capo", myCapo);
}

/// Returns a string representation of the tuning from low to
/// high (e.g. "E A D G B E").
std::ostream &operator<<(std::ostream &os, const Tuning &t);

#endif
