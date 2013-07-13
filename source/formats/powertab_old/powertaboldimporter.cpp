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

#include "powertaboldimporter.h"

#include <boost/assign/list_of.hpp>
#include "powertabdocument/guitar.h"
#include "powertabdocument/powertabdocument.h"
#include "powertabdocument/score.h"
#include <score/score.h>
#include <score/generalmidi.h>

PowerTabOldImporter::PowerTabOldImporter()
    : FileFormatImporter(FileFormat("Power Tab Document",
                                    boost::assign::list_of("ptb")))
{
}

void PowerTabOldImporter::load(const std::string &filename, Score &score)
{
    PowerTabDocument::Document document;
    document.Load(filename);

    // TODO - handle line spacing, font settings, etc.
    ScoreInfo info;
    convert(document.GetHeader(), info);
    score.setScoreInfo(info);

    // TODO - merge bass score.
    convert(*document.GetScore(0), score);
}

void PowerTabOldImporter::convert(const PowerTabDocument::PowerTabFileHeader &header,
                                  ScoreInfo &info)
{
    using namespace PowerTabDocument;

    if (header.GetFileType() == PowerTabFileHeader::FILETYPE_SONG)
    {
        SongData data;

        data.setTitle(header.GetSongTitle());
        data.setArtist(header.GetSongArtist());

        const uint8_t releaseType = header.GetSongReleaseType();
        if (releaseType == PowerTabFileHeader::RELEASETYPE_PUBLIC_AUDIO)
        {
            data.setAudioReleaseInfo(SongData::AudioData(
                static_cast<SongData::AudioData::AudioReleaseType>(header.GetSongAudioReleaseType()),
                header.GetSongAudioReleaseTitle(), header.GetSongAudioReleaseYear(),
                header.IsSongAudioReleaseLive()));
        }
        else if (releaseType == PowerTabFileHeader::RELEASETYPE_PUBLIC_VIDEO)
        {
            data.setVideoReleaseInfo(SongData::VideoData(header.GetSongVideoReleaseTitle(),
                                                         header.IsSongVideoReleaseLive()));
        }
        else if (releaseType == PowerTabFileHeader::RELEASETYPE_BOOTLEG)
        {
            data.setBootlegInfo(SongData::BootlegData(header.GetSongBootlegTitle(),
                                                      header.GetSongBootlegDate()));
        }
        else
        {
            data.setUnreleased();
        }

        if (header.GetSongAuthorType() == PowerTabFileHeader::AUTHORTYPE_TRADITIONAL)
            data.setTraditionalAuthor();
        else
        {
            data.setAuthorInfo(SongData::AuthorData(header.GetSongComposer(),
                                                    header.GetSongLyricist()));
        }

        data.setArranger(header.GetSongArranger());
        data.setTranscriber(header.GetSongGuitarScoreTranscriber());
        data.setCopyright(header.GetSongCopyright());
        data.setLyrics(header.GetSongLyrics());
        data.setPerformanceNotes(header.GetSongGuitarScoreNotes());

        info.setSongData(data);
    }
    else
    {
        LessonData data;

        data.setTitle(header.GetLessonTitle());
        data.setSubtitle(header.GetLessonSubtitle());
        data.setMusicStyle(static_cast<LessonData::MusicStyle>(
                               header.GetLessonMusicStyle()));
        data.setDifficultyLevel(static_cast<LessonData::DifficultyLevel>(
                                    header.GetLessonLevel()));
        data.setAuthor(header.GetLessonAuthor());
        data.setNotes(header.GetLessonNotes());
        data.setCopyright(header.GetLessonCopyright());

        info.setLessonData(data);
    }
}

void PowerTabOldImporter::convert(const PowerTabDocument::Score &oldScore, Score &score)
{
    // Convert guitars to players and instruments.
    for (size_t i = 0; i < oldScore.GetGuitarCount(); ++i)
    {
        convert(*oldScore.GetGuitar(i), score);
    }
}

void PowerTabOldImporter::convert(const PowerTabDocument::Guitar &guitar, Score &score)
{
    Player player;
    player.setDescription(guitar.GetDescription());
    player.setMaxVolume(guitar.GetInitialVolume());
    player.setPan(guitar.GetPan());

    Tuning tuning;
    convert(guitar.GetTuning(), tuning);
    tuning.setCapo(guitar.GetCapo());
    player.setTuning(tuning);

    score.insertPlayer(player);

    Instrument instrument;
    instrument.setMidiPreset(guitar.GetPreset());

    // Use the MIDI preset name as the description.
    std::vector<std::string> presetNames;
    Midi::getMidiPresetNames(presetNames);
    instrument.setDescription(presetNames.at(guitar.GetPreset()));

    score.insertInstrument(instrument);
}

void PowerTabOldImporter::convert(const PowerTabDocument::Tuning &oldTuning, Tuning &tuning)
{
    tuning.setName(oldTuning.GetName());
    tuning.setNotes(oldTuning.GetTuningNotes());
    tuning.setMusicNotationOffset(oldTuning.GetMusicNotationOffset());
    tuning.setSharps(oldTuning.UsesSharps());
    // The capo is set from the Guitar object.
}
