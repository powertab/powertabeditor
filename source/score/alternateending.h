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

#ifndef SCORE_ALTERNATEENDING_H
#define SCORE_ALTERNATEENDING_H

#include <bitset>
#include "fileversion.h"
#include <iosfwd>
#include <vector>

class AlternateEnding
{
public:
    enum SpecialAlternateEndings
    {
        DaCapo,
        DalSegno,
        DalSegnoSegno,
        NumSpecialAlternateEndings
    };

    AlternateEnding();
    explicit AlternateEnding(int position);

    bool operator==(const AlternateEnding &other) const;

	template <class Archive>
	void serialize(Archive &ar, const FileVersion version);

    /// Returns the position within the system where the ending is anchored.
    int getPosition() const;
    /// Sets the position within the system where the ending is anchored.
    void setPosition(int position);

    /// Add a new number to the alternate ending.
    void addNumber(int number);
    /// Removes a number from the alternate ending.
    void removeNumber(int number);
    /// Returns a list of the alternate ending's numbers.
    const std::vector<int> &getNumbers() const;

    bool hasDaCapo() const;
    void setDaCapo(bool set = true);
    bool hasDalSegno() const;
    void setDalSegno(bool set = true);
    bool hasDalSegnoSegno() const;
    void setDalSegnoSegno(bool set = true);

    /// Minimum valid repeat number.
    static const int MIN_NUMBER;
    /// Maximum valid repeat number.
    static const int MAX_NUMBER;

private:
    int myPosition;
    std::vector<int> myNumbers;
    std::bitset<NumSpecialAlternateEndings> mySpecialEndings;
};

template <class Archive>
void AlternateEnding::serialize(Archive &ar, const FileVersion /*version*/)
{
	ar("position", myPosition);
	ar("numbers", myNumbers);
	ar("special_endings", mySpecialEndings);
}

/// Gets the alternate ending text (numbers + D.C./D.S./D.S.S.).
std::ostream &operator<<(std::ostream &os, const AlternateEnding &ending);

#endif
