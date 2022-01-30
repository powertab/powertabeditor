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
#include <filesystem>
#include <fstream>
#include <iostream>
#include <score/serialization.h>
#include <stdexcept>

static const char *theTuningDictFilename = "tunings.json";

std::vector<TuningDictionary::Entry>
TuningDictionary::load()
{
    for (std::filesystem::path dir : Paths::getDataDirs())
    {
        auto path = dir / theTuningDictFilename;
        if (!std::filesystem::exists(path))
            continue;

        std::ifstream file(path);

        std::vector<Tuning> tunings;
        ScoreUtils::load(file, "tunings", tunings);

        std::vector<TuningDictionary::Entry> entries;
        for (const Tuning &tuning: tunings)
            entries.push_back({ tuning, /* writeable */ true });

        return entries;
    }

    std::cerr << "Could not locate tuning dictionary." << std::endl;
    std::cerr << "Candidate paths:" << std::endl;

    for (std::filesystem::path dir : Paths::getDataDirs())
        std::cerr << (dir / theTuningDictFilename) << std::endl;

    return {};
}

void TuningDictionary::save() const
{
    ensureLoaded();

    // If the tuning dictionary is empty, it presumably failed to load for some
    // reason. So, we shouldn't be trying to save an empty dictionary in the
    // config folder.
    if (myEntries.empty())
        return;

    auto dir = Paths::getUserDataDir();
    std::filesystem::create_directories(dir);

    auto path = dir / theTuningDictFilename;
    std::ofstream file(path);
    if (!file)
        throw std::runtime_error("Error opening tuning dictionary for writing.");

    std::vector<Tuning> tunings;
    for (const Entry &entry : myEntries)
        tunings.push_back(entry.myTuning);

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
