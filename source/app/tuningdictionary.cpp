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

#include <app/appinfo.h>
#include <app/paths.h>
#include <boost/filesystem/fstream.hpp>
#include <boost/filesystem/operations.hpp>
#include <score/serialization.h>
#include <stdexcept>

static const char *theTuningDictFilename = "tunings.json";

std::vector<Tuning> TuningDictionary::load()
{
    for (boost::filesystem::path dir : Paths::getDataDirs())
    {
        auto path = dir / theTuningDictFilename;
        if (!boost::filesystem::exists(path))
            continue;

        boost::filesystem::ifstream file(path);

        std::vector<Tuning> tunings;
        ScoreUtils::load(file, "tunings", tunings);
        return tunings;
    }

    throw std::runtime_error("Could not locate tuning dictionary.");
}

void TuningDictionary::save() const
{
    auto dir = Paths::getUserDataDir();
    boost::filesystem::create_directories(dir);

    auto path = dir / theTuningDictFilename;
    boost::filesystem::ofstream file(path);
    if (!file)
        throw std::runtime_error("Error opening file for writing.");

    ensureLoaded();
    ScoreUtils::save(file, "tunings", myTunings);
}

void TuningDictionary::loadInBackground()
{
    myFuture = std::async(std::launch::async, &TuningDictionary::load);
}

void TuningDictionary::findTunings(int numStrings,
                                   std::vector<Tuning *> &tunings)
{
    ensureLoaded();
    for (Tuning &tuning : myTunings)
    {
        if (tuning.getStringCount() == numStrings)
            tunings.push_back(&tuning);
    }
}

void TuningDictionary::findTunings(int numStrings,
                                   std::vector<const Tuning *> &tunings) const
{
    ensureLoaded();
    for (const Tuning &tuning : myTunings)
    {
        if (tuning.getStringCount() == numStrings)
            tunings.push_back(&tuning);
    }
}

void TuningDictionary::addTuning(const Tuning &tuning)
{
    ensureLoaded();
    myTunings.push_back(tuning);
}

void TuningDictionary::removeTuning(const Tuning &tuning)
{
    ensureLoaded();
    myTunings.erase(std::remove(myTunings.begin(), myTunings.end(), tuning));
}

void TuningDictionary::ensureLoaded() const
{
    if (myFuture.valid())
    {
        // myTunings shouldn't be mutable in general.
        auto me = const_cast<TuningDictionary *>(this);
        me->myTunings = myFuture.get();
    }
}
