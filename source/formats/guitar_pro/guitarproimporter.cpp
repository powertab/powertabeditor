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
  
#include "guitarproimporter.h"

#include <boost/algorithm/clamp.hpp>
#include <boost/assign/list_of.hpp>
#include <boost/date_time/gregorian/gregorian_types.hpp>
#include <fstream>
#include "inputstream.h"
#include <score/generalmidi.h>
#include <score/score.h>

const std::map<std::string, Gp::Version> GuitarProImporter::versionStrings = boost::assign::map_list_of
    ("FICHIER GUITAR PRO v3.00", Gp::Version3)
    ("FICHIER GUITAR PRO v4.00", Gp::Version4)
    ("FICHIER GUITAR PRO v4.06", Gp::Version4)
    ("FICHIER GUITAR PRO L4.06", Gp::Version4)
    ("FICHIER GUITAR PRO v5.00", Gp::Version5_0)
    ("FICHIER GUITAR PRO v5.10", Gp::Version5_1);

GuitarProImporter::GuitarProImporter()
    : FileFormatImporter(FileFormat(
          "Guitar Pro 3, 4, 5", boost::assign::list_of("gp3")("gp4")("gp5")))
{
}

void GuitarProImporter::load(const std::string &filename, Score &score)
{
    std::ifstream in(filename, std::ios::binary | std::ios::in);
    Gp::InputStream stream(in);

    findFileVersion(stream);

    ScoreInfo info;
    readHeader(stream, info);
    score.setScoreInfo(info);

    score.insertSystem(System());
    readStartTempo(stream, score);

    if (stream.version == Gp::Version5_1)
        stream.skip(1);

    stream.skip(4); // Initial key -- ignore.

    if (stream.version >= Gp::Version4)
        stream.skip(1); // Octave 8va -- ignore.

    const std::vector<Gp::Channel> channels = readChannels(stream);

    if (stream.version > Gp::Version4)
        stream.skip(42); // RSE data?

    const uint32_t numMeasures = stream.read<uint32_t>();
    const uint32_t numTracks = stream.read<uint32_t>();

    std::vector<Gp::Bar> bars;
    readBarlines(stream, numMeasures, bars);

    readTracks(stream, score, numTracks, channels);
#if 0
    readSystems(stream, score, bars);

    return ptbDoc;
#endif
}

void GuitarProImporter::findFileVersion(Gp::InputStream &stream)
{
    const std::string versionString = stream.readVersionString();

    auto it = versionStrings.find(versionString);
    if (it != versionStrings.end())
        stream.version = it->second;
    else
        throw FileFormatException("Unsupported file version: " + versionString);
}

void GuitarProImporter::readHeader(Gp::InputStream &stream, ScoreInfo &info)
{
    SongData song;

    song.setTitle(stream.readString());
    stream.readString(); // Ignore subtitle.

    song.setArtist(stream.readString());
    song.setAudioReleaseInfo(SongData::AudioReleaseInfo(
        SongData::AudioReleaseInfo::ReleaseType::Single, stream.readString(),
        boost::gregorian::day_clock::local_day().year(), false));
    song.setAuthorInfo(SongData::AuthorInfo(stream.readString(), ""));

    if (stream.version > Gp::Version4)
        stream.readString();

    song.setCopyright(stream.readString());
    song.setTranscriber(stream.readString());

    const std::string instructions = stream.readString();
    std::string comments;
    if (!instructions.empty())
        comments += instructions + "\n";

    // Read several lines of comments.
    const uint32_t numComments = stream.read<uint32_t>();
    for (uint32_t i = 0; i < numComments; ++i)
        comments += stream.readString();
    song.setPerformanceNotes(comments);

    if (stream.version <= Gp::Version4)
        stream.skip(1); // triplet feel attribute -- ignore.

    if (stream.version >= Gp::Version4)
    {
        // Read lyrics.
        std::string lyrics;
        // Track number that the lyrics are associated with -- ignore.
        stream.skip(4);

        // Read several lines of lyrics.
        for (int i = 0; i < Gp::NumberOfLinesOfLyrics; ++i)
        {
            // Measure associated with the line -- ignore.
            stream.skip(4);
            lyrics += stream.readIntString();
        }

        song.setLyrics(lyrics);
    }

    // Read page setup (ignore).
    if (stream.version > Gp::Version4)
    {
        if (stream.version == Gp::Version5_0)
            stream.skip(30);
        else if (stream.version == Gp::Version5_1)
            stream.skip(49);

        for (int i = 0; i < 11; ++i)
        {
            stream.skip(4);
            stream.readFixedLengthString(0);
        }
    }

    info.setSongData(song);
}

void GuitarProImporter::readStartTempo(Gp::InputStream &stream, Score &score)
{
    const uint32_t bpm = stream.read<uint32_t>();

    TempoMarker marker(0);
    marker.setBeatsPerMinute(static_cast<int>(bpm));
    score.getSystems().front().insertTempoMarker(marker);
}

std::vector<Gp::Channel> GuitarProImporter::readChannels(
    Gp::InputStream &stream)
{
    std::vector<Gp::Channel> channels;

    for (int i = 0; i < Gp::NumberOfMidiChannels; ++i)
    {
        Gp::Channel channel;

        channel.instrument = boost::algorithm::clamp<int32_t>(
            stream.read<int32_t>(), 0, Midi::NUM_MIDI_PRESETS);
        channel.volume = Gp::Channel::readChannelProperty(stream);
        channel.balance = Gp::Channel::readChannelProperty(stream);
        channel.chorus = Gp::Channel::readChannelProperty(stream);
        channel.reverb = Gp::Channel::readChannelProperty(stream);
        channel.phaser = Gp::Channel::readChannelProperty(stream);
        channel.tremolo = Gp::Channel::readChannelProperty(stream);

        // Ignored (compatibility with older versions).
        stream.skip(2);

        channels.push_back(channel);
    }

    return channels;
}

void GuitarProImporter::readBarlines(Gp::InputStream &stream,
                                     uint32_t numMeasures,
                                     std::vector<Gp::Bar> &bars)
{
    for (uint32_t measure = 0; measure < numMeasures; ++measure)
    {
        if (stream.version > Gp::Version4 && measure > 0)
            stream.skip(1);

        Gp::Bar bar;
        Barline &barline = bar.myBarline;

        // Clone time/key signature from previous barline if possible.
        if (measure != 0)
        {
            const Barline &prevBar = bars.back().myBarline;

            TimeSignature timeSig = prevBar.getTimeSignature();
            timeSig.setVisible(false);
            barline.setTimeSignature(timeSig);

            KeySignature keySig = prevBar.getKeySignature();
            keySig.setVisible(false);
            barline.setKeySignature(keySig);
        }

        TimeSignature timeSignature = barline.getTimeSignature();

        const Gp::Flags flags = stream.read<uint8_t>();

        if (flags.test(Gp::Numerator))
        {
            timeSignature.setVisible(true);
            timeSignature.setBeatsPerMeasure(stream.read<uint8_t>());
            timeSignature.setNumPulses(timeSignature.getBeatsPerMeasure());
        }
        if (flags.test(Gp::Denominator))
        {
            timeSignature.setVisible(true);
            timeSignature.setBeatValue(stream.read<uint8_t>());
        }

        barline.setTimeSignature(timeSignature);

        if (flags.test(Gp::RepeatBegin))
            barline.setBarType(Barline::RepeatStart);

        // TODO - handle this.
        if (flags.test(Gp::RepeatEnd))
        {
#if 0
            // we don't set the repeatEnd type here - see the 'fixRepeatEnds'
            // function for details
            barline->SetRepeatCount(stream.read<uint8_t>() + 1);
#endif
        }

        if (flags.test(Gp::Marker) && stream.version == Gp::Version5_1)
            barline.setRehearsalSign(readRehearsalSign(stream));

        if (flags.test(Gp::AltEnding))
        {
            AlternateEnding ending;
            ending.addNumber(stream.read<uint8_t>());
            bar.myAlternateEnding = ending;
        }

        if (flags.test(Gp::Marker) && stream.version != Gp::Version5_1)
            barline.setRehearsalSign(readRehearsalSign(stream));

        if (flags.test(Gp::KeySignatureChange))
        {
            KeySignature keySignature = barline.getKeySignature();
            keySignature.setVisible(true);

            keySignature.setNumAccidentals(
                convertKeyAccidentals(stream.read<int8_t>()));
            keySignature.setKeyType(
                static_cast<KeySignature::KeyType>(stream.read<uint8_t>()));
            barline.setKeySignature(keySignature);
        }

        if (flags.test(Gp::DoubleBar))
            barline.setBarType(Barline::DoubleBar);

        // More unknown GP5 data ...
        if (stream.version > Gp::Version4)
        {
            if (flags.test(Gp::Numerator) || flags.test(Gp::Denominator))
                stream.skip(4);
            if (!flags.test(Gp::AltEnding))
                stream.skip(1);

            stream.skip(1); // Triplet feel -- ignore.
        }

        bars.push_back(bar);
    }
}

RehearsalSign GuitarProImporter::readRehearsalSign(Gp::InputStream &stream)
{
    RehearsalSign sign;

    const std::string description = stream.readString();
    sign.setDescription(description);

    // Set the letter to A for now - the proper letter will be set after
    // importing the whole document.
    sign.setLetters("A");

    readColor(stream);

    return sign;
}

void GuitarProImporter::readColor(Gp::InputStream &stream)
{
    // Ignore, since PowerTab doesn't currently have any use for the colors in
    // GP files.
    stream.skip(4);
}

int GuitarProImporter::convertKeyAccidentals(int8_t gpKey)
{
    // Guitar Pro uses 0 for C, 1 for G, ..., -1 for F, -2 for Bb, ..., whereas
    // Power Tab uses 0 for 1, 1 for G, 1 for F, etc.
    return std::abs(gpKey);
}

void GuitarProImporter::readTracks(Gp::InputStream &stream, Score &score,
                                   uint32_t numTracks,
                                   const std::vector<Gp::Channel> &channels)
{
    for (uint32_t i = 0; i < numTracks; ++i)
    {
        if (stream.version > Gp::Version4)
        {
            if (i == 0 || stream.version == Gp::Version5_0)
                stream.skip(1);
        }

        // Ignore flags used for indicating drum tracks, banjo tracks, etc.
        stream.skip(1);

        Player player;
        Instrument instrument;
        player.setDescription(stream.readFixedLengthString(
                                  Gp::TrackDescriptionLength));
        instrument.setDescription(player.getDescription());
        Tuning tuning = readTuning(stream);

        // MIDI port used -- ignore (Power Tab handles this automatically).
        stream.skip(4);

        // Index of MIDI channel used.
        const uint32_t channelIndex = stream.read<uint32_t>();

        // Find the specified channel and copy its information into an
        // instrument.
        if (channelIndex < channels.size())
        {
            const Gp::Channel &channel = channels[channelIndex];

            instrument.setMidiPreset(channel.instrument);
            player.setMaxVolume(channel.volume);
            player.setPan(channel.balance);
        }
        else
        {
            throw FileFormatException("Invalid channel index: " +
                                      std::to_string(channelIndex) +
                                      " for track: " + std::to_string(i));
        }

        // MIDI channel used for effects -- ignore.
        stream.skip(4);

        // Number of frets -- ignore.
        stream.skip(4);

        tuning.setCapo(stream.read<uint32_t>());
        player.setTuning(tuning);

        // Ignore track color.
        readColor(stream);

        // RSE data???
        if (stream.version == Gp::Version5_0)
            stream.skip(44);
        else if (stream.version == Gp::Version5_1)
        {
            stream.skip(49);
            std::cerr << stream.readString() << std::endl;
            std::cerr << stream.readString() << std::endl;
        }

        score.insertPlayer(player);
        score.insertInstrument(instrument);
    }

    // Not sure what this data is used for ...
    if (stream.version == Gp::Version5_0)
        stream.skip(2);
    else if (stream.version == Gp::Version5_1)
        stream.skip(1);
}

Tuning GuitarProImporter::readTuning(Gp::InputStream &stream)
{
    Tuning tuning;

    const uint32_t numStrings = stream.read<uint32_t>();

    std::vector<uint8_t> tuningNotes;
    // Gp::NumberOfStrings integers are in the file, but only the first
    // "numStrings" are actually used
    for (uint32_t i = 0; i < Gp::NumberOfStrings; i++)
    {
        const uint32_t tuningNote = stream.read<uint32_t>();

        if (i < numStrings)
            tuningNotes.push_back(tuningNote);
    }

    tuning.setNotes(tuningNotes);
    return tuning;
}

#if 0
void GuitarProImporter::readSystems(Gp::InputStream &stream, Score *score,
                                    std::vector<BarData> bars)
{
    BOOST_FOREACH(BarData & bar, bars)
    {
        std::vector<std::vector<PositionData>> positionLists(
            score->GetGuitarCount());

        for (uint32_t track = 0; track < score->GetGuitarCount(); track++)
        {
            std::vector<PositionData> &positionList = positionLists.at(track);
            const Tuning &tuning = score->GetGuitar(track)->GetTuning();

            const uint32_t numBeats =
                stream.read<uint32_t>(); // number of beats in measure

            for (uint32_t k = 0; k < numBeats; k++)
            {
                positionList.push_back(readBeat(stream, tuning));
            }

            // TODO - support second voice for GP5
            if (stream.version > Gp::Version4)
            {
                const uint32_t beats = stream.read<uint32_t>();
                for (uint32_t k = 0; k < beats; k++)
                {
                    readBeat(stream, tuning);
                }
            }

            if (stream.version > Gp::Version4)
            {
                stream.skip(1); // not sure what this is used for ...
            }
        }

        bar.positionLists = positionLists;
    }

    arrangeScore(score, bars, true);
}

/// Reads a beat (Guitar Pro equivalent of a Position in Power Tab)
PositionData GuitarProImporter::readBeat(Gp::InputStream &stream,
                                         const Tuning &tuning)
{
    const Gp::Flags flags = stream.read<uint8_t>();

    Position *pos = new Position();
    PositionData data(pos);

    pos->SetDotted(flags.test(Gp::Dotted));

    if (flags.test(Gp::BeatStatus))
    {
        const uint8_t status = stream.read<uint8_t>();

        if (status == Gp::BeatEmpty)
        {
            // TODO - handle empty position?
        }
        else if (status == Gp::BeatRest)
        {
            pos->SetRest(true);
        }
    }

    pos->SetDurationType(readDuration(stream));

    if (flags.test(Gp::IrregularGrouping))
    {
        const uint32_t notesPlayed =
            stream.read<uint32_t>(); // notes played in the irregular grouping

        // the "denominator" of the irregular grouping is the nearest power of 2
        // (from below)
        const uint32_t notesOver = pow(2, floor(Common::log2(notesPlayed)));

        pos->SetIrregularGroupingTiming(notesPlayed, notesOver);
        pos->SetIrregularGroupingStart(
            true); // TODO - need to group with other notes properly
    }

    if (flags.test(Gp::ChordDiagram))
    {
        readChordDiagram(stream, tuning);
    }

    // Floating text - not handled yet.
    if (flags.test(Gp::Text))
    {
        stream.readString();
    }

    if (flags.test(Gp::NoteEffects))
    {
        readPositionEffects(stream, *pos);
    }

    if (flags.test(Gp::MixTableChangeEvent))
    {
        readMixTableChangeEvent(stream, data);
    }

    readNotes(stream, *pos);

    // unknown GP5 data ...
    if (stream.version > Gp::Version4)
    {
        stream.skip(1);
        const int x = stream.read<uint8_t>();
        if ((x & 0x08) != 0)
        {
            stream.skip(1);
        }
    }

    return data;
}

/// Reads a duration value and converts it into PTB format
uint8_t GuitarProImporter::readDuration(Gp::InputStream &stream)
{
    const int8_t gpDuration = stream.read<int8_t>();

    // Durations for Guitar Pro are stored as 0 -> quarter note, -1 -> half
    // note, 1 -> eight note, etc
    // We need to convert to 1 = whole note, 2 = half note, 4 = quarter note,
    // etc
    return pow(2.0, gpDuration + 2);
}

/// Reads the notes for a given position
void GuitarProImporter::readNotes(Gp::InputStream &stream, Position &position)
{
    const Gp::Flags stringsPlayed = stream.read<uint8_t>();

    for (int i = Gp::NumberOfStrings - 1; i >= 0; i--)
    {
        if (stringsPlayed.test(i))
        {
            Note note;

            note.SetString(Gp::NumberOfStrings - i - 1);

            const Gp::Flags flags = stream.read<uint8_t>();

            position.SetMarcato(flags.test(Gp::AccentedNote));

            if (stream.version > Gp::Version4)
            {
                position.SetSforzando(flags.test(Gp::HeavyAccentedNote));
            }

            note.SetGhostNote(flags.test(Gp::GhostNote));
            // ignore dotted note flag - already handled elsewhere for the
            // Position object

            if (flags.test(Gp::NoteType))
            {
                const uint8_t noteType = stream.read<uint8_t>();
                note.SetTied(noteType == Gp::TiedNote);
                note.SetMuted(noteType == Gp::MutedNote);
            }

            if (stream.version <= Gp::Version4 &&
                flags.test(Gp::TimeIndependentDuration))
            {
                // this is a repeat of the Position duration -- ignore
                stream.skip(1);
                stream.skip(1);
            }

            if (flags.test(Gp::Dynamic))
            {
                stream.skip(1); // TODO - convert into a Dynamic object
            }

            if (flags.test(Gp::NoteType)) // if there is a non-empty note, read
                                          // fret number
            {
                // TODO - drum tracks will crash here because of fret numbers
                // larger than 29
                // The temporary workaround is to just set the fret number to 0
                const uint8_t fret = stream.read<uint8_t>();
                if (Note::IsValidFretNumber(fret))
                {
                    note.SetFretNumber(fret);
                }
            }

            if (flags.test(Gp::FingeringType))
            {
                // left and right hand fingerings -- ignore
                stream.skip(1);
                stream.skip(1);
            }

            if (stream.version > Gp::Version4)
            {
                // TODO - figure out what this data is used for in GP5
                if (flags.test(Gp::TimeIndependentDuration))
                {
                    stream.skip(8);
                }
                stream.skip(1);
            }

            if (flags.test(Gp::NoteEffects))
            {
                if (stream.version >= Gp::Version4)
                {
                    readNoteEffects(stream, position, note);
                }
                else if (stream.version == Gp::Version3)
                {
                    readNoteEffectsGp3(stream, position, note);
                }
            }

            position.InsertNote(note.CloneObject());
        }
    }
}

/// Reads the effects for the given note
/// Note: some of the effects apply to the entire Position (not just a single
/// note),
/// due to differences between Guitar Pro and Power Tab notation
void GuitarProImporter::readNoteEffects(Gp::InputStream &stream,
                                        Position &position, Note &note)
{
    const Gp::Flags header1 = stream.read<uint8_t>();
    const Gp::Flags header2 = stream.read<uint8_t>();

    if (header1.test(Gp::HasBend))
    {
        readBend(stream, note);
    }

    if (header1.test(Gp::HasGraceNote))
    {
        // TODO - handle grace notes
        stream.skip(1); // fret number grace note is made from
        stream.skip(1); // grace note dynamic
        stream.skip(1); // transition type
        stream.skip(1); // duration
        if (stream.version > Gp::Version4)
        {
            stream.skip(1); // flags for GP5
        }
    }

    if (header2.test(Gp::HasTremoloPicking))
    {
        // ignore - Power Tab does not allow different values for the tremolo
        // picking duration (e.g. eighth notes)
        stream.skip(1);
        position.SetTremoloPicking(true);
    }

    if (header2.test(Gp::HasSlide))
    {
        readSlide(stream, note);
    }

    if (header2.test(Gp::HasHarmonic))
    {
        readHarmonic(stream, note);
    }

    if (header2.test(Gp::HasTrill))
    {
        const uint8_t fret = stream.read<uint8_t>();
        if (note.IsValidTrill(fret))
        {
            note.SetTrill(fret);
        }

        stream.skip(1); // ignore trill duration (duration is a fixed value in
                        // Power Tab)
    }

    position.SetLetRing(header1.test(Gp::HasLetRing));

    note.SetHammerOn(header1.test(Gp::HasHammerOnOrPullOff));

    position.SetVibrato(header2.test(Gp::HasVibrato));
    position.SetPalmMuting(header2.test(Gp::HasPalmMute));
    position.SetStaccato(header2.test(Gp::HasStaccato));
}

/// Note effects are sufficiently different in GP3 to make a separate function
/// necessary
void GuitarProImporter::readNoteEffectsGp3(Gp::InputStream &stream,
                                           Position &position, Note &note)
{
    const Gp::Flags flags = stream.read<uint8_t>();

    position.SetLetRing(flags.test(Gp::HasLetRing));
    note.SetHammerOn(flags.test(Gp::HasHammerOnOrPullOff));

    if (flags.test(Gp::HasSlideOutVer3))
    {
        note.SetSlideOutOf(Note::slideOutOfDownwards, 12);
    }

    if (flags.test(Gp::HasBend))
    {
        readBend(stream, note);
    }

    if (flags.test(Gp::HasGraceNote))
    {
        stream.read<uint8_t>(); // fret number grace note is made from
        stream.read<uint8_t>(); // grace note dynamic
        stream.read<uint8_t>(); // transition type
        stream.read<uint8_t>(); // duration
        // TODO - will need to add an extra note to be the grace note
    }
}

void GuitarProImporter::readSlide(Gp::InputStream &stream, Note &note)
{
    int8_t slideValue = stream.read<int8_t>();

    if (stream.version <= Gp::Version4)
    {
        /* Slide values are as follows:
            -2 : slide into from above
            -1 : slide into from below
            0  : no slide
            1  : shift slide
            2  : legato slide
            3  : slide out of downwards
            4  : slide out of upwards
        */

        if (slideValue < 0) // slide into
        {
            note.SetSlideInto(std::abs(slideValue));
        }
        else if (slideValue > 0)
        {
            note.SetSlideOutOf(slideValue, 0); // We don't know the number of
                                               // steps for the slide yet
        }
    }
    else
    {
        // slide values are powers of 2
        slideValue = Common::log2(slideValue) + 1;

        if (slideValue > 4)
        {
            note.SetSlideInto(slideValue - 4);
        }
        else
        {
            note.SetSlideOutOf(slideValue, 0);
        }
    }
}

void GuitarProImporter::readHarmonic(Gp::InputStream &stream, Note &note)
{
    const uint8_t harmonic = stream.read<uint8_t>();

    if (harmonic == Gp::NaturalHarmonic)
    {
        note.SetNaturalHarmonic(true);
    }
    else if (harmonic == Gp::TappedHarmonic)
    {
        if (stream.version > Gp::Version4)
        {
            std::cerr << "Tapped Harmonic Data: " << (int)stream.read<uint8_t>()
                      << std::endl;
        }
        note.SetTappedHarmonic(true);
    }
    // TODO - handle artificial harmonics for GP3, GP4, and GP5
    else if (harmonic == Gp::ArtificalHarmonicGp5)
    {
        stream.skip(3);
    }
}

void GuitarProImporter::readBend(Gp::InputStream &stream, Note &)
{
    // TODO - perform conversion for bends

    stream.read<uint8_t>(); // bend type
    stream.read<uint32_t>(); // bend height

    const uint32_t numPoints = stream.read<uint32_t>(); // number of bend points

    for (uint32_t i = 0; i < numPoints; i++)
    {
        stream.skip(4); // time relative to the previous point
        stream.skip(4); // bend position
        stream.skip(1); // bend vibrato
    }
}

void GuitarProImporter::readTremoloBar(Gp::InputStream &stream,
                                       Position &position)
{
    uint8_t eventType;
    if (stream.version == Gp::Version3)
    {
        eventType = Position::dip;
    }
    else
    {
        eventType = convertTremoloEventType(stream.read<uint8_t>());
    }

    const int32_t pitch = convertBendPitch(stream.read<int32_t>());

    position.SetTremoloBar(eventType, 0, pitch);

    if (stream.version >= Gp::Version4)
    {
        const uint32_t numPoints =
            stream.read<uint32_t>(); // number of bend points
        for (uint32_t i = 0; i < numPoints; i++)
        {
            stream.skip(4); // time relative to the previous point
            stream.skip(4); // bend value
            stream.skip(1); // vibrato (used for bend, not for tremolo bar)
        }
    }
}

uint8_t GuitarProImporter::convertTremoloEventType(uint8_t gpEventType)
{
    switch (gpEventType)
    {
        case Gp::Dip:
            return Position::dip;

        case Gp::Dive:
            return Position::diveAndRelease;

        case Gp::ReleaseUp:
        case Gp::ReleaseDown:
            return Position::release;

        case Gp::InvertedDip:
            return Position::invertedDip;

        case Gp::TremoloReturn:
            return Position::returnAndRelease;

        default:
        {
            std::cerr << "Invalid tremolo bar event type: " << (int)gpEventType
                      << std::endl;
            return Position::diveAndRelease;
        }
    }
}

/// Converts bend pitches from GP format (25 per quarter tone) to PTB (1 per
/// quarter tone)
uint8_t GuitarProImporter::convertBendPitch(int32_t gpBendPitch)
{
    return std::abs(gpBendPitch / 25);
}

void GuitarProImporter::readMixTableChangeEvent(Gp::InputStream &stream,
                                                PositionData &position)
{
    // TODO - implement conversions for this

    stream.read<int8_t>(); // instrument

    if (stream.version > Gp::Version4)
    {
        stream.skip(16); // RSE Info???
    }

    int8_t volume = stream.read<int8_t>(); // volume
    int8_t pan = stream.read<uint8_t>(); // pan
    int8_t chorus = stream.read<uint8_t>(); // chorus
    int8_t reverb = stream.read<uint8_t>(); // reverb
    int8_t phaser = stream.read<uint8_t>(); // phaser
    int8_t tremolo = stream.read<uint8_t>(); // tremolo

    if (stream.version > Gp::Version4)
    {
        std::cerr << stream.readString() << std::endl; // tempo name???
    }

    // New tempo.
    int32_t tempo = stream.read<int32_t>();
    if (tempo > 0)
    {
        position.tempoMarker = boost::make_shared<TempoMarker>();
        position.tempoMarker->SetBeatsPerMinute(tempo);
    }

    if (volume >= 0)
    {
        stream.read<uint8_t>(); // volume change duration
    }

    if (pan >= 0)
    {
        stream.read<uint8_t>(); // pan change duration
    }

    if (chorus >= 0)
    {
        stream.read<uint8_t>(); // chorus change duration
    }

    if (reverb >= 0)
    {
        stream.read<uint8_t>(); // reverb change duration
    }

    if (phaser >= 0)
    {
        stream.read<uint8_t>(); // phaser change duration
    }

    if (tremolo >= 0)
    {
        stream.read<uint8_t>(); // tremolo change duration
    }

    if (tempo >= 0)
    {
        stream.skip(1); // tempo change duration

        if (stream.version == Gp::Version5_1)
        {
            stream.skip(1);
        }
    }

    if (stream.version >= Gp::Version4)
    {
        stream.read<uint8_t>(); // details of score-wide or track-specific
                                // changes
    }

    if (stream.version > Gp::Version4)
    {
        stream.skip(1);
        if (stream.version == Gp::Version5_1)
        {
            std::cerr << stream.readString() << std::endl;
            std::cerr << stream.readString() << std::endl;
        }
    }
}

void GuitarProImporter::readPositionEffects(Gp::InputStream &stream,
                                            Position &position)
{
    const Gp::Flags flags1 = stream.read<uint8_t>();

    Gp::Flags flags2; // only read this if we are in GP4 or higher

    // GP3 effect decoding
    if (stream.version == Gp::Version3)
    {
        position.SetVibrato(flags1.test(Gp::VibratoGp3_1) ||
                            flags1.test(Gp::VibratoGp3_2));

        // FIXME - in Power Tab, harmonic correspond to notes, not to positions
        // (beats)
        // However, when the Position effects are being read, the notes haven't
        // been read yet ...
        if (flags1.test(Gp::NaturalHarmonicGp3))
        {
            // TODO - set natural harmonic
        }
        if (flags1.test(Gp::ArtificialHarmonicGp3))
        {
            // TODO - set artificial harmonic
        }

        // ignore fade-in effect
    }
    else
    {
        flags2 = stream.read<uint8_t>();
    }

    if (flags1.test(Gp::HasTapping))
    {
        const uint8_t type = stream.read<uint8_t>();

        // in GP3, a value of 0 indicates a tremolo bar
        if (type == Gp::TremoloBarGp3 && stream.version == Gp::Version3)
        {
            readTremoloBar(stream, position);
        }
        else
        {
            position.SetTap(type == Gp::Tapping);
            // Ignore slapping and popping

            if (stream.version == Gp::Version3)
            {
                stream.read<uint32_t>(); // TODO - decipher this data
            }
        }
    }

    if (stream.version >= Gp::Version4 && flags2.test(Gp::HasTremoloBarEvent))
    {
        readTremoloBar(stream, position);
    }

    if (flags1.test(Gp::HasStrokeEffect))
    {
        // upstroke and downstroke duration values - we will just use these for
        // toggling pickstroke up/down
        if (stream.read<uint8_t>() > 0)
        {
            position.SetPickStrokeDown();
        }
        if (stream.read<uint8_t>() > 0)
        {
            position.SetPickStrokeUp();
        }
    }

    if (stream.version >= Gp::Version4)
    {
        position.SetTremoloPicking(flags2.test(Gp::HasRasguedo));
    }

    if (stream.version >= Gp::Version4 && flags2.test(Gp::Pickstroke))
    {
        const uint8_t pickstrokeType = stream.read<uint8_t>();

        if (pickstrokeType == Gp::PickstrokeUp)
        {
            position.SetArpeggioUp();
        }
        else if (pickstrokeType == Gp::PickstrokeDown)
        {
            position.SetArpeggioDown();
        }
    }
}

/// TODO - test reading of chord diagrams, and implement for GP5
void GuitarProImporter::readChordDiagram(Gp::InputStream &stream,
                                         const Tuning &tuning)
{
    if (stream.version > Gp::Version4)
    {
        throw FileFormatException("Chord Diagrams not supported yet for GP5");
    }

    const Gp::Flags header = stream.read<uint8_t>();

    if (!header.test(Gp::Gp4ChordFormat))
    {
        readOldStyleChord(stream, tuning);
        return;
    }

    if (stream.version == Gp::Version3)
    {
        stream.skip(25);
        stream.readFixedLengthString(34); // chord name
        stream.read<uint32_t>(); // top fret of chord

        // strings that are used
        for (int i = 0; i < Gp::NumberOfStringsGp3; i++)
        {
            stream.read<uint32_t>();
        }
        stream.skip(36);
        return;
    }

    stream.read<bool>(); // sharps/flats

    // blank bytes for backwards compatibility with gp3
    stream.skip(3);

    stream.read<uint8_t>(); // root of chord
    stream.read<uint8_t>(); // chord type
    stream.read<uint8_t>(); // extension (9, 11, 13)
    stream.read<uint32_t>(); // bass note of chord
    stream.read<uint32_t>(); // diminished/augmented
    stream.read<uint8_t>(); // "add" chord

    stream.readFixedLengthString(Gp::ChordDiagramDescriptionLength);

    // more blank bytes for backwards compatibility
    stream.skip(2);

    stream.read<uint8_t>(); // tonality of the 5th
    stream.read<uint8_t>(); // tonality of the 9th
    stream.read<uint8_t>(); // tonality of the 11th

    stream.read<int32_t>(); // base fret of the chord

    // fret numbers for each string
    for (int i = 0; i < Gp::NumberOfStrings; i++)
    {
        stream.read<int32_t>();
    }

    stream.read<uint8_t>(); // number of barres in the chord

    for (int i = 0; i < Gp::NumberOfBarres; i++)
    {
        stream.read<uint8_t>(); // fret of the barre
    }

    for (int i = 0; i < Gp::NumberOfBarres; i++)
    {
        stream.read<uint8_t>(); // barre start
    }

    for (int i = 0; i < Gp::NumberOfBarres; i++)
    {
        stream.read<uint8_t>(); // barre end
    }

    // Omission1, Omission3, Omission5, Omission7, Omission9,
    // Omission11, Omission13, and another blank byte
    stream.skip(8);

    // fingering of chord
    for (int i = 0; i < Gp::NumberOfStrings; i++)
    {
        stream.read<int8_t>();
    }

    stream.read<bool>(); // show fingering
}

/// Reads an old-style "simple" chord
void GuitarProImporter::readOldStyleChord(Gp::InputStream &stream,
                                          const Tuning &tuning)
{
    std::vector<uint8_t> fretNumbers(tuning.GetStringCount(),
                                     ChordDiagram::stringMuted);

    ChordDiagram diagram(0, fretNumbers);

    stream.readString(); // chord diagram name

    const uint32_t baseFret = stream.read<uint32_t>();
    diagram.SetTopFret(baseFret);

    if (baseFret != 0)
    {
        for (int i = 0; i < Gp::NumberOfStringsGp3; i++)
        {
            diagram.SetFretNumber(i, stream.read<uint32_t>());
        }
    }
}
#endif
