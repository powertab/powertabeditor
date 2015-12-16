/*
  * Copyright (C) 2015 Cameron White
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

#include "settingsmanager.h"

#include <boost/filesystem/fstream.hpp>
#include <boost/filesystem/operations.hpp>
#include <iostream>

static const char *theSettingsFilename = "settings.json";

void SettingsManager::load(const boost::filesystem::path &dir)
{
#ifdef __APPLE__
    if (!boost::filesystem::exists(dir))
        return;

    auto settings = getWriteHandle();
    settings->loadFromPlist();
#else
    auto path = dir / theSettingsFilename;

    if (!boost::filesystem::exists(path))
        return;

    try
    {
        boost::filesystem::ifstream is(path);

        auto settings = getWriteHandle();
        settings->loadFromJSON(is);
    }
    catch (const std::exception &e)
    {
        std::cerr << "Error loading " << path << ": " << e.what() << std::endl;
    }
#endif
}

void SettingsManager::save(const boost::filesystem::path &dir) const
{
    // Ensure the directory exists.
    boost::filesystem::create_directories(dir);

    auto settings = getReadHandle();

#ifdef __APPLE__
    settings->saveToPlist();
#else
    // Save the settings to disk in JSON format.
    auto path = dir / theSettingsFilename;
    boost::filesystem::ofstream os(path);
    os.exceptions(std::ios::failbit | std::ios::badbit);

    settings->saveToJSON(os);
#endif
}
