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

#include "midiexporter.h"

#include <app/settingsmanager.h>
#include <audio/settings.h>
#include <midi/midifile.h>
#include <score/generalmidi.h>

#include <array>
#include <boost/endian/conversion.hpp>
#include <fstream>
#include <cstdint>

template <typename T>
static void write(std::ostream &os, T val)
{
    val = boost::endian::native_to_big(val);
    os.write(reinterpret_cast<const char *>(&val), sizeof(T));
}

static void writeVariableLength(std::ostream &os, uint32_t val)
{
    std::array<uint8_t, 5> bytes;
    for (int i = 0; i < 5; ++i)
        bytes[i] = (val >> ((4 - i) * 7)) & 0x7f;

    // Find the first byte that needs to be written.
    int i = 0;
    for (i = 0; i < 4; ++i)
    {
        if (bytes[i] != 0)
            break;
    }

    for (; i < 5; ++i)
    {
        // Set the top bit to indicate that more bytes will follow it.
        if (i < 4)
            bytes[i] |= 0x80;

        write(os, bytes[i]);
    }
}

MidiExporter::MidiExporter(const SettingsManager &settings_manager)
    : FileFormatExporter(FileFormat("MIDI File", { "mid" })),
      mySettingsManager(settings_manager)
{
}

void MidiExporter::save(const std::filesystem::path &filename, const Score &score)
{
    std::ofstream os(filename, std::ios::out | std::ios::binary);
    os.exceptions(std::ios::failbit | std::ios::badbit | std::ios::eofbit);

    MidiFile::LoadOptions options;
    options.myEnableMetronome = false;
    options.myRecordPositionChanges = false;
    {
        auto settings = mySettingsManager.getReadHandle();
        options.myMetronomePreset = settings->get(Settings::MetronomePreset) +
                                    Midi::MIDI_PERCUSSION_PRESET_OFFSET;
        options.myStrongAccentVel =
            settings->get(Settings::MetronomeStrongAccent);
        options.myWeakAccentVel = settings->get(Settings::MetronomeWeakAccent);
        options.myVibratoStrength = settings->get(Settings::MidiVibratoLevel);
        options.myWideVibratoStrength =
            settings->get(Settings::MidiWideVibratoLevel);
    }

    MidiFile file;
    file.load(score, options);
    writeHeader(os, file);

    for (const MidiEventList &track : file.getTracks())
        writeTrack(os, track);
}

void MidiExporter::writeHeader(std::ostream &os, const MidiFile &file)
{
    // Chunk ID for the header chunk.
    os << "MThd";
    // 6 bytes will follow the chunk size.
    write(os, static_cast<uint32_t>(6));

    // A format type of 1 indicates that we'll have multiple tracks.
    write(os, static_cast<uint16_t>(1));
    write(os, static_cast<uint16_t>(file.getTracks().size()));

    // Time division.
    write(os, static_cast<uint16_t>(file.getTicksPerBeat()));
}

void MidiExporter::writeTrack(std::ostream & os, const MidiEventList & events)
{
    // Chunk ID for a track chunk.
    os << "MTrk";

    // Size in bytes of the track chunk. This will be updated after writing out
    // all of the data.
    const std::iostream::pos_type chunk_len_pos = os.tellp();
    write(os, static_cast<uint32_t>(0));

    // Write out the MIDI events.
    const std::iostream::pos_type chunk_start_pos = os.tellp();
    for (const MidiEvent &event : events)
    {
        writeVariableLength(os, event.getTicks());
        os.write(reinterpret_cast<const char *>(event.getData().data()),
                 event.getData().size());
    }

    const std::iostream::pos_type chunk_end_pos = os.tellp();

    // Record the size in bytes of the track chunk.
    os.seekp(chunk_len_pos);
    write(os, static_cast<uint32_t>(chunk_end_pos - chunk_start_pos));
    os.seekp(chunk_end_pos);
}
