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

#ifndef FORMATS_MIDIEXPORTER_H
#define FORMATS_MIDIEXPORTER_H

#include <formats/fileformatmanager.h>

class MidiEventList;
class MidiFile;

class MidiExporter : public FileFormatExporter
{
public:
    MidiExporter(const SettingsManager &settings_manager);

    virtual void save(const std::filesystem::path &filename,
                      const Score &score) override;

private:
    static void writeHeader(std::ostream &os, const MidiFile &file);
    static void writeTrack(std::ostream &os, const MidiEventList &events);

    const SettingsManager &mySettingsManager;
};

#endif
