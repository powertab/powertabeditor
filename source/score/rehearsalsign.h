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

#ifndef SCORE_REHEARSALSIGN_H
#define SCORE_REHEARSALSIGN_H

#include "fileversion.h"
#include <string>

class RehearsalSign
{
public:
    RehearsalSign();
    RehearsalSign(const std::string &letters, const std::string &description);

    bool operator==(const RehearsalSign &other) const;

    template <class Archive>
    void serialize(Archive &ar, const FileVersion version);

    /// Returns the letter(s) of the sign (e.g. "A", "B", ... "Z", "AA", ...).
    const std::string &getLetters() const;
    /// Sets the letter(s) of the rehearsal sign.
    void setLetters(const std::string &letters);

    /// Returns a description of the rehearsal sign (e.g. "Chorus").
    const std::string &getDescription() const;
    /// Sets the description of the rehearsal sign.
    void setDescription(const std::string &description);

private:
    std::string myLetters;
    std::string myDescription;
};

template <class Archive>
void RehearsalSign::serialize(Archive &ar, const FileVersion /*version*/)
{
    ar("letters", myLetters);
    ar("description", myDescription);
}

#endif
