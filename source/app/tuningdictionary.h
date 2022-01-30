/*
  * Copyright (C) 2012 Cameron White
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

#ifndef APP_TUNINGDICTIONARY_H
#define APP_TUNINGDICTIONARY_H

#include <future>
#include <score/tuning.h>
#include <vector>

class TuningDictionary
{
public:
    struct Entry
    {
        Tuning myTuning;
        bool myWriteable = true;
    };

    /// Saves the tuning dictionary to a file.
    void save() const;

    /// Loads the tuning dictionary in a separate thread.
    void loadInBackground();

    /// Returns all tunings with the specified number of strings.
    void findTunings(int numStrings, std::vector<Entry *> &tunings);
    void findTunings(int numStrings,
                     std::vector<const Entry *> &tunings) const;

    /// Adds a new tuning to the tuning dictionary.
    void addTuning(const Tuning &tuning);

    /// Removes the specified tuning from the dictionary.
    void removeTuning(const Tuning &tuning);

private:
    /// Loads the tuning dictionary from a file.
    static std::vector<Entry> load();

    void ensureLoaded() const;

    mutable std::future<std::vector<Entry>> myFuture;
    std::vector<Entry> myEntries;
};

#endif
