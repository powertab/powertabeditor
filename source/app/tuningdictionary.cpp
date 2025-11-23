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

#include "tuningdictionary.h"

#include <algorithm>
#include <app/appinfo.h>
#include <app/paths.h>
#include <exception>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <score/serialization.h>
#include <stdexcept>
#include <unordered_set>

static const char *theTuningDictFilename = "tunings.json";

std::vector<TuningDictionary::Entry>
TuningDictionary::load()
{
    // Accumulate from all tunings.json files in the search path. This is in
    // reverse order, from the installation folder to the user prefs dir, so
    // that factory tunings appear first.
    auto search_paths = Paths::getDataDirs();
    std::reverse(search_paths.begin(), search_paths.end());

    auto user_data_dir = Paths::getUserDataDir();

    std::unordered_set<Tuning> known_tunings;
    std::vector<TuningDictionary::Entry> entries;
    for (std::filesystem::path dir : search_paths)
    {
        auto path = dir / theTuningDictFilename;
        if (!std::filesystem::exists(path))
            continue;

        std::ifstream file(path);

        std::vector<Tuning> tunings;
        try
        {
            ScoreUtils::load(file, "tunings", tunings);
        }
        catch (const std::exception &e)
        {
            std::cerr << "Failed to load tunings from file " << path << std::endl;
            std::cerr << "Error: " << e.what() << std::endl;
            continue;
        }

        const bool writeable = (dir == user_data_dir);
        for (const Tuning &tuning: tunings)
        {
            // Avoid adding duplicate tunings from multiple files.
            // This is important for old tunings.json files in the user prefs
            // dir, which contained the whole tuning dictionary contents before
            // bug #367.
            if (known_tunings.find(tuning) != known_tunings.end())
                continue;

            known_tunings.insert(tuning);
            entries.push_back({ tuning, writeable });
        }
    }

    if (entries.empty())
    {
        std::cerr << "Could not locate tuning dictionary." << std::endl;
        std::cerr << "Candidate paths:" << std::endl;

        for (std::filesystem::path dir : Paths::getDataDirs())
            std::cerr << (dir / theTuningDictFilename) << std::endl;
    }

    return entries;
}

void TuningDictionary::save() const
{
    ensureLoaded();

    auto dir = Paths::getUserDataDir();
    std::filesystem::create_directories(dir);

    auto path = dir / theTuningDictFilename;
    std::ofstream file(path);
    if (!file)
        throw std::runtime_error("Error opening tuning dictionary for writing.");

    std::vector<Tuning> tunings;
    for (const Entry &entry : myEntries)
    {
        // Only save out tunings that are marked as writeable (i.e.
        // newly-added, or were loaded from the tunings.json file in the user
        // prefs dir). Other tunings are from the factory tuning dictionary.
        if (entry.myWriteable)
            tunings.push_back(entry.myTuning);
    }

    ScoreUtils::save(file, "tunings", tunings);
}

void TuningDictionary::loadInBackground()
{
    myFuture = std::async(std::launch::async, &TuningDictionary::load);
}

void TuningDictionary::findTunings(int numStrings,
                                   std::vector<Entry *> &tunings)
{
    ensureLoaded();
    for (Entry &entry : myEntries)
    {
        if (entry.myTuning.getStringCount() == numStrings)
            tunings.push_back(&entry);
    }
}

void TuningDictionary::findTunings(int numStrings,
                                   std::vector<const Entry *> &tunings) const
{
    ensureLoaded();
    for (const Entry &entry : myEntries)
    {
        if (entry.myTuning.getStringCount() == numStrings)
            tunings.push_back(&entry);
    }
}

void TuningDictionary::addTuning(const Tuning &tuning)
{
    ensureLoaded();
    myEntries.push_back(Entry{ tuning, /* writeable */ true });
}

void TuningDictionary::removeTuning(const Tuning &tuning)
{
    ensureLoaded();
    auto it = std::find_if(myEntries.begin(), myEntries.end(),
                           [&](const Entry &entry)
                           { return entry.myTuning == tuning; });
    if (it == myEntries.end())
        return;

    assert(it->myWriteable);
    myEntries.erase(it);
}

void TuningDictionary::ensureLoaded() const
{
    if (myFuture.valid())
    {
        // myEntries shouldn't be mutable in general.
        auto me = const_cast<TuningDictionary *>(this);
        me->myEntries = myFuture.get();
    }
}
