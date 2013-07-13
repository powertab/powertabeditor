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

#ifndef APP_OPTIONS_H
#define APP_OPTIONS_H

#include <string>
#include <vector>

class QStringList;

/// Handles command-line argument parsing.
class Options
{
public:
    Options();

    /// Parses the command lines arguments.
    /// This function may cause the program to exit if e.g. the "--help"
    /// argument is provided.
    /// @return True if the parsing was successful, false otherwise.
    bool parse(const QStringList &argList);

    /// Returns a list of files that should be opened.
    std::vector<std::string> filesToOpen();

private:
    /// Converts a QStringList to a vector of std::string.
    static std::vector<std::string> toStdStringList(const QStringList &args);

    std::vector<std::string> myFilenames;
};

#endif
