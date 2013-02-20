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

#ifndef OPTIONS_H
#define OPTIONS_H

#include <string>
#include <vector>

class QStringList;

/// Handles command-line argument parsing.
class Options
{
public:
    Options();

    bool parse(const QStringList &argList);

    std::vector<std::string> filesToOpen();

private:
    static std::vector<std::string> toStdStringList(const QStringList &args);

    std::vector<std::string> filenames;
};

#endif // OPTIONS_H
