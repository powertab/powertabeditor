/*
  * Copyright (C) 2011 Cameron White
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

#ifndef FORMATS_GUITARPROIMPORTER_H
#define FORMATS_GUITARPROIMPORTER_H

#include <formats/fileformat.h>

namespace Gp
{
    struct Header;
}

class ScoreInfo;

/// Imports Guitar Pro files.
class GuitarProImporter : public FileFormatImporter
{
public:
    GuitarProImporter();

    virtual void load(const std::string &filename, Score &score) override;

private:
    static void convert(Gp::Header &header, ScoreInfo &info);

#if 0
    /// Read the song information (title, artist, etc).
    void readHeader(Gp::InputStream &stream, ScoreInfo &info);

    /// Read the initial tempo for the score.
    void readStartTempo(Gp::InputStream &stream, Score &score);

    /// Reads all of the measures in the score, and any alternate endings that
    /// occur.
    void readBarlines(Gp::InputStream &stream, uint32_t numMeasures,
                      std::vector<Gp::Bar> &bars);

    RehearsalSign readRehearsalSign(Gp::InputStream &stream);

    void readColor(Gp::InputStream &stream);

    /// Converts key accidentals from the Guitar Pro format to Power Tab.
    int convertKeyAccidentals(int8_t gpKey);

    /// Read and convert all tracks (players).
    void readTracks(Gp::InputStream &stream, Score &score, uint32_t numTracks,
                    const std::vector<Gp::Channel> &channels);

    /// Imports a Guitar Pro tuning and converts it into a Power Tab tuning.
    Tuning readTuning(Gp::InputStream &stream);

    void readSystems(Gp::InputStream &stream, Score &score,
                     std::vector<Gp::Bar> bars);

    /// Reads a beat (Guitar Pro equivalent of a Position in Power Tab).
    void readBeat(Gp::InputStream &stream, const Tuning &tuning, Position &pos,
                  boost::optional<TempoMarker> &tempoMarker);

    /// Reads a duration value and converts it into PTB format.
    Position::DurationType readDuration(Gp::InputStream &stream);

    void readChordDiagram(Gp::InputStream &stream, const Tuning &tuning);

    /// Reads an old-style "simple" chord from earlier versions.
    void readOldStyleChord(Gp::InputStream &stream, const Tuning &tuning);

    void readPositionEffects(Gp::InputStream &stream, Position &position);

    void readTremoloBar(Gp::InputStream &stream, Position &position);

    void readMixTableChangeEvent(Gp::InputStream &stream,
                                 boost::optional<TempoMarker> &tempoMarker);

#if 0
    static uint8_t convertTremoloEventType(uint8_t gpEventType);
    static uint8_t convertBendPitch(int32_t gpBendPitch);
#endif

    void readNotes(Gp::InputStream &stream, Position &position);

    void readNoteEffects(Gp::InputStream &stream, Position &position,
                         Note &note);
    void readNoteEffectsGp3(Gp::InputStream &stream, Position &position,
                            Note &note);

    void readSlide(Gp::InputStream &stream, Note &note);
    void readHarmonic(Gp::InputStream &stream, Note &note);
    void readBend(Gp::InputStream &stream, Note &note);
#endif
};

#endif
