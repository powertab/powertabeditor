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

#include <formats/guitar_pro/document.h>
#include <formats/guitar_pro/inputstream.h>
#include <score/score.h>

static const int POSITIONS_PER_SYSTEM = 35;

GuitarProImporter::GuitarProImporter()
    : FileFormatImporter(
          FileFormat("Guitar Pro 3, 4, 5", { "gp3", "gp4", "gp5" }))
{
}

void GuitarProImporter::load(const std::string &filename, Score &score)
{
    std::ifstream in(filename, std::ios::binary | std::ios::in);
    Gp::InputStream stream(in);

#if 1
    Gp::Document document;
    document.load(stream);

    ScoreInfo info;
    convertHeader(document.myHeader, info);
    score.setScoreInfo(info);

    convertPlayers(document, score);
    convertScore(document, score);

    // Automatically set the rehearsal sign letters to "A", "B", etc.
    ScoreUtils::adjustRehearsalSigns(score);
#else
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
    readSystems(stream, score, bars);

    ScoreUtils::adjustRehearsalSigns(score);
#endif
}

void GuitarProImporter::convertHeader(Gp::Header &header, ScoreInfo &info)
{
    SongData song;

    song.setTitle(header.myTitle);
    song.setArtist(header.myArtist);
    song.setAudioReleaseInfo(SongData::AudioReleaseInfo(
        SongData::AudioReleaseInfo::ReleaseType::Single, header.myAlbum,
        boost::gregorian::day_clock::local_day().year(), false));
    song.setAuthorInfo(
        SongData::AuthorInfo(header.myComposer, header.myLyricist));
    song.setCopyright(header.myCopyright);
    song.setTranscriber(header.myTranscriber);

    // Merge the instructions and comments into the performance notes.
    std::string comments;
    if (!header.myInstructions.empty())
        comments += header.myInstructions + "\n";
    for (const std::string &comment : header.myNotices)
        comments += comment;
    song.setPerformanceNotes(comments);

    // Merge lyrics together.
    std::string lyrics;
    for (const Gp::Header::LyricLine &line : header.myLyrics)
        lyrics += line.myContents;
    song.setLyrics(lyrics);

    info.setSongData(song);
}

void GuitarProImporter::convertPlayers(Gp::Document &doc, Score &score)
{
    for (const Gp::Track &track : doc.myTracks)
    {
        Player player;
        Instrument instrument;
        Tuning tuning;

        player.setDescription(track.myName);
        instrument.setDescription(track.myName);

        tuning.setNotes(track.myTuning);
        tuning.setCapo(track.myCapo);

        const Gp::Channel &channel = doc.myChannels[track.myChannelIndex];
        instrument.setMidiPreset(channel.myInstrument);
        player.setMaxVolume(channel.myVolume);
        player.setPan(channel.myBalance);

        player.setTuning(tuning);
        score.insertPlayer(player);
        score.insertInstrument(instrument);
    }
}

int GuitarProImporter::convertBarline(const Gp::Measure &measure,
                                      const Gp::Measure *prevMeasure,
                                      System &system, int start, int end)
{
    Barline bar;
    bar.setPosition(start);

    if (prevMeasure && prevMeasure->myIsDoubleBar)
        bar.setBarType(Barline::DoubleBar);
    else if (measure.myIsRepeatBegin)
        bar.setBarType(Barline::RepeatStart);

    if (measure.myMarker)
        bar.setRehearsalSign(RehearsalSign("", *measure.myMarker));

    // Insert at the correct location.
    if (start == 0)
        system.getBarlines().front() = bar;
    else
        system.insertBarline(bar);

    if (measure.myRepeatEnd)
    {
        Barline bar;
        bar.setPosition(end);

        if (measure.myRepeatEnd)
        {
            bar.setBarType(Barline::RepeatEnd);
            bar.setRepeatCount(measure.myRepeatEnd.get());
        }

        // Insert at the correct location.
        if (end > POSITIONS_PER_SYSTEM)
            system.getBarlines().back() = bar;
        else
            system.insertBarline(bar);

        ++end;
    }

    return end;
}

void GuitarProImporter::convertScore(Gp::Document &doc, Score &score)
{
    System system;

    // Add a staff for each player.
    for (const Player &player : score.getPlayers())
        system.insertStaff(Staff(player.getTuning().getStringCount()));

    // Add initial tempo marker.
    {
        TempoMarker marker;
        marker.setPosition(0);
        marker.setBeatsPerMinute(doc.myStartTempo);
        system.insertTempoMarker(marker);
    }

    // Add an initial player change.
    {
        // Set up an initial player change.
        PlayerChange change;
        for (int i = 0; i < score.getPlayers().size(); ++i)
            change.insertActivePlayer(i, ActivePlayer(i, i));
        system.insertPlayerChange(change);
    }

    int startPos = 0;
    for (int m = 0; m < doc.myMeasures.size(); ++m)
    {
        const Gp::Measure &measure = doc.myMeasures[m];

        // Try to create a new system every so often.
        if (startPos > POSITIONS_PER_SYSTEM)
        {
            system.getBarlines().back().setPosition(startPos + 1);
            score.insertSystem(system);
            system = System();

            // Add a staff for each player.
            for (const Player &player : score.getPlayers())
                system.insertStaff(Staff(player.getTuning().getStringCount()));

            startPos = 0;
        }

        // For each player, import the notes from the current measure.
        int nextPos = startPos;
        for (int i = 0; i < score.getPlayers().size(); ++i)
        {
            const Tuning &tuning = score.getPlayers()[i].getTuning();
            Staff &staff = system.getStaves()[i];
            const Gp::Staff &gp_staff = measure.myStaves[i];

            // Start inserting notes after the barline.
            int currentPos = (startPos != 0) ? startPos + 1 : 0;

            for (int v = 0; v < gp_staff.myVoices.size(); ++v)
            {
                Voice &voice = staff.getVoices()[v];

                for (const Gp::Beat &beat : gp_staff.myVoices[v])
                {
                    Position pos;
                    pos.setPosition(currentPos);
                    pos.insertNote(Note(0, 0));

                    voice.insertPosition(pos);
                    ++currentPos;
                }
            }

            nextPos = std::max(nextPos, currentPos);
        }

        // Import the barline, key signature, etc.
        const Gp::Measure *prevMeasure =
            (m > 0) ? &doc.myMeasures[m - 1] : nullptr;
        nextPos = convertBarline(measure, prevMeasure, system, startPos,
                                 nextPos);

        startPos = nextPos;
    }

    // Insert the final system.
    system.getBarlines().back().setPosition(startPos + 1);
    score.insertSystem(system);
}

#if 0

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
#else
            stream.read<uint8_t>();
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

int GuitarProImporter::convertKeyAccidentals(int8_t gpKey)
{
    // Guitar Pro uses 0 for C, 1 for G, ..., -1 for F, -2 for Bb, ..., whereas
    // Power Tab uses 0 for 1, 1 for G, 1 for F, etc.
    return std::abs(gpKey);
}

void GuitarProImporter::readSystems(Gp::InputStream &stream, Score &score,
                                    std::vector<Gp::Bar> bars)
{
    System *system = &score.getSystems().front();
    int startPos = 0;

    for (auto &player : score.getPlayers())
        system->insertStaff(Staff(player.getTuning().getStringCount()));

    // Set up an initial player change.
    PlayerChange change;
    for (int i = 0; i < score.getPlayers().size(); ++i)
        change.insertActivePlayer(i, ActivePlayer(i, i));
    system->insertPlayerChange(change);

    for (Gp::Bar &bar : bars)
    {
        // Try to create a new system every so often.
        if (startPos > POSITIONS_PER_SYSTEM)
        {
            system->getBarlines().back().setPosition(startPos + 1);
            score.insertSystem(System());
            system = &score.getSystems().back();

            for (auto &player : score.getPlayers())
                system->insertStaff(Staff(player.getTuning().getStringCount()));

            startPos = 0;
        }

        int nextPos = startPos;

        for (int i = 0; i < score.getPlayers().size(); ++i)
        {
            const Tuning &tuning = score.getPlayers()[i].getTuning();
            Staff &staff = system->getStaves()[i];

            int currentPos = (startPos != 0) ? startPos + 1 : 0;

            const uint32_t numBeats = stream.read<uint32_t>();

            for (uint32_t j = 0; j < numBeats; ++j)
            {
                Position pos;
                boost::optional<TempoMarker> tempoMarker;
                readBeat(stream, tuning, pos, tempoMarker);

                // Tempo markers are stored at the beat level, not at the system
                // level, so we need to ensure that we don't insert duplicate
                // tempo markers.
                if (tempoMarker && !ScoreUtils::findByPosition(
                                        system->getTempoMarkers(), currentPos))
                {
                    tempoMarker->setPosition(currentPos);
                    system->insertTempoMarker(*tempoMarker);
                }

                pos.setPosition(currentPos);
                staff.getVoices()[0].insertPosition(pos);
                currentPos++;
            }

            // TODO - support second voice for GP5.
            if (stream.version > Gp::Version4)
            {
                const uint32_t beats = stream.read<uint32_t>();
                Position pos;
                boost::optional<TempoMarker> tempoMarker;

                for (uint32_t j = 0; j < beats; ++j)
                    readBeat(stream, tuning, pos, tempoMarker);
            }

            // Not sure what this is used for ...
            if (stream.version > Gp::Version4)
                stream.skip(1);

            nextPos = std::max(nextPos, currentPos);
        }

        bar.myBarline.setPosition(startPos);
        if (startPos == 0)
            system->getBarlines().front() = bar.myBarline;
        else
            system->insertBarline(bar.myBarline);

        startPos = nextPos;
    }

    system->getBarlines().back().setPosition(startPos + 1);
}

void GuitarProImporter::readBeat(Gp::InputStream &stream, const Tuning &tuning,
                                 Position &pos,
                                 boost::optional<TempoMarker> &tempoMarker)
{
    const Gp::Flags flags = stream.read<uint8_t>();
    bool wholeRest = false;

    pos.setProperty(Position::Dotted, flags.test(Gp::Dotted));

    if (flags.test(Gp::BeatStatus))
    {
        const uint8_t status = stream.read<uint8_t>();

        if (status == Gp::BeatEmpty)
        {
            // Whole rest.
            pos.setRest();
            pos.setDurationType(Position::WholeNote);
            wholeRest = true;
        }
        else if (status == Gp::BeatRest)
            pos.setRest();
    }

    Position::DurationType duration = readDuration(stream);
    if (!wholeRest)
        pos.setDurationType(duration);

    if (flags.test(Gp::IrregularGrouping))
    {
// TODO - we don't yet support irregular groups in the new file format.
#if 0
        // Notes played in the irregular grouping.
        const uint32_t notesPlayed = stream.read<uint32_t>();

        // The "denominator" of the irregular grouping is the nearest power of 2
        // (from below)
        const uint32_t notesOver = std::pow(2, std::floor(std::log(notesPlayed) / std::log(2.0)));

        pos->SetIrregularGroupingTiming(notesPlayed, notesOver);
        pos->SetIrregularGroupingStart(
            true); // TODO - need to group with other notes properly
#else
        stream.read<uint32_t>();
#endif
    }

    if (flags.test(Gp::ChordDiagram))
        readChordDiagram(stream, tuning);

    // Floating text - not handled yet.
    if (flags.test(Gp::Text))
        stream.readString();

    if (flags.test(Gp::NoteEffects))
        readPositionEffects(stream, pos);

    if (flags.test(Gp::MixTableChangeEvent))
        readMixTableChangeEvent(stream, tempoMarker);

    readNotes(stream, pos);

    // Unknown GP5 data ...
    if (stream.version > Gp::Version4)
    {
        stream.skip(1);
        const int x = stream.read<uint8_t>();
        if ((x & 0x08) != 0)
            stream.skip(1);
    }
}

Position::DurationType GuitarProImporter::readDuration(Gp::InputStream &stream)
{
    const int8_t gpDuration = stream.read<int8_t>();

    // Durations for Guitar Pro are stored as 0 -> quarter note, -1 -> half
    // note, 1 -> eight note, etc
    // We need to convert to 1 = whole note, 2 = half note, 4 = quarter note,
    // etc
    return static_cast<Position::DurationType>(
        static_cast<int>(std::pow(2.0, gpDuration + 2)));
}

void GuitarProImporter::readChordDiagram(Gp::InputStream &stream,
                                         const Tuning &tuning)
{
    if (stream.version > Gp::Version4)
        throw FileFormatException("Chord Diagrams not supported yet for GP5");

    const Gp::Flags header = stream.read<uint8_t>();

    if (!header.test(Gp::Gp4ChordFormat))
    {
        readOldStyleChord(stream, tuning);
        return;
    }

    if (stream.version == Gp::Version3)
    {
        stream.skip(25);
        stream.readFixedLengthString(34); // Chord name.
        stream.read<uint32_t>(); // Top fret of chord.

        // Strings that are used.
        for (int i = 0; i < Gp::NumberOfStringsGp3; i++)
            stream.read<uint32_t>();
        stream.skip(36);
        return;
    }

    stream.read<bool>(); // Sharps/flats.

    // Blank bytes for backwards compatibility with gp3.
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
        stream.read<int32_t>();

    stream.read<uint8_t>(); // number of barres in the chord

    for (int i = 0; i < Gp::NumberOfBarres; i++)
        stream.read<uint8_t>(); // fret of the barre

    for (int i = 0; i < Gp::NumberOfBarres; i++)
        stream.read<uint8_t>(); // barre start

    for (int i = 0; i < Gp::NumberOfBarres; i++)
        stream.read<uint8_t>(); // barre end

    // Omission1, Omission3, Omission5, Omission7, Omission9,
    // Omission11, Omission13, and another blank byte
    stream.skip(8);

    // fingering of chord
    for (int i = 0; i < Gp::NumberOfStrings; i++)
        stream.read<int8_t>();

    stream.read<bool>(); // show fingering
}

void GuitarProImporter::readOldStyleChord(Gp::InputStream &stream,
                                          const Tuning & /*tuning*/)
{
// TODO - chord diagrams are not yet supported for the new file format.
#if 0
    std::vector<uint8_t> fretNumbers(tuning.getStringCount(),
                                     ChordDiagram::stringMuted);

    ChordDiagram diagram(0, fretNumbers);
#endif
    stream.readString(); // chord diagram name

    const uint32_t baseFret = stream.read<uint32_t>();
#if 0
    diagram.SetTopFret(baseFret);
#endif

    if (baseFret != 0)
    {
        for (int i = 0; i < Gp::NumberOfStringsGp3; ++i)
        {
#if 0
            diagram.SetFretNumber(i, stream.read<uint32_t>());
#else
            stream.read<uint32_t>();
#endif
        }
    }
}

void GuitarProImporter::readPositionEffects(Gp::InputStream &stream,
                                            Position &position)
{
    const Gp::Flags flags1 = stream.read<uint8_t>();

    Gp::Flags flags2; // Only read this if we are in GP4 or higher.

    // GP3 effect decoding
    if (stream.version == Gp::Version3)
    {
        position.setProperty(
            Position::Vibrato,
            flags1.test(Gp::VibratoGp3_1) || flags1.test(Gp::VibratoGp3_2));

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

        // In GP3, a value of 0 indicates a tremolo bar.
        if (type == Gp::TremoloBarGp3 && stream.version == Gp::Version3)
            readTremoloBar(stream, position);
        else
        {
            // Ignore slapping and popping.
            position.setProperty(Position::Tap, type == Gp::Tapping);

            if (stream.version == Gp::Version3)
                stream.read<uint32_t>(); // TODO - decipher this data.
        }
    }

    if (stream.version >= Gp::Version4 && flags2.test(Gp::HasTremoloBarEvent))
        readTremoloBar(stream, position);

    if (flags1.test(Gp::HasStrokeEffect))
    {
        // Upstroke and downstroke duration values - we will just use these for
        // toggling pickstroke up/down.
        if (stream.read<uint8_t>() > 0)
            position.setProperty(Position::PickStrokeDown);
        if (stream.read<uint8_t>() > 0)
            position.setProperty(Position::PickStrokeUp);
    }

    if (stream.version >= Gp::Version4)
    {
        position.setProperty(Position::TremoloPicking,
                             flags2.test(Gp::HasRasguedo));
    }

    if (stream.version >= Gp::Version4 && flags2.test(Gp::Pickstroke))
    {
        const uint8_t pickstrokeType = stream.read<uint8_t>();

        if (pickstrokeType == Gp::PickstrokeUp)
            position.setProperty(Position::ArpeggioUp);
        else if (pickstrokeType == Gp::PickstrokeDown)
            position.setProperty(Position::ArpeggioDown);
    }
}

void GuitarProImporter::readTremoloBar(Gp::InputStream &stream,
                                       Position & /*position*/)
{
// TODO - implement tremolo bar support.
#if 1
    if (stream.version != Gp::Version3)
        stream.read<uint8_t>();

    stream.read<int32_t>();

    if (stream.version >= Gp::Version4)
    {
        const uint32_t numPoints = stream.read<uint32_t>();
        for (uint32_t i = 0; i < numPoints; i++)
        {
            stream.skip(4); // time relative to the previous point
            stream.skip(4); // bend value
            stream.skip(1); // vibrato (used for bend, not for tremolo bar)
        }
    }
#else
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
#endif
}

void GuitarProImporter::readMixTableChangeEvent(
    Gp::InputStream &stream, boost::optional<TempoMarker> &tempoMarker)
{
    // TODO - implement conversions for this.

    stream.read<int8_t>(); // instrument

    if (stream.version > Gp::Version4)
        stream.skip(16); // RSE Info???

    int8_t volume = stream.read<int8_t>(); // volume
    int8_t pan = stream.read<uint8_t>(); // pan
    int8_t chorus = stream.read<uint8_t>(); // chorus
    int8_t reverb = stream.read<uint8_t>(); // reverb
    int8_t phaser = stream.read<uint8_t>(); // phaser
    int8_t tremolo = stream.read<uint8_t>(); // tremolo

    if (stream.version > Gp::Version4)
        std::cerr << stream.readString() << std::endl; // tempo name???

    // New tempo.
    int32_t tempo = stream.read<int32_t>();
    if (tempo > 0)
    {
        tempoMarker = TempoMarker();
        tempoMarker->setBeatsPerMinute(tempo);
    }

    if (volume >= 0)
        stream.read<uint8_t>(); // volume change duration

    if (pan >= 0)
        stream.read<uint8_t>(); // pan change duration

    if (chorus >= 0)
        stream.read<uint8_t>(); // chorus change duration

    if (reverb >= 0)
        stream.read<uint8_t>(); // reverb change duration

    if (phaser >= 0)
        stream.read<uint8_t>(); // phaser change duration

    if (tremolo >= 0)
        stream.read<uint8_t>(); // tremolo change duration

    if (tempo >= 0)
    {
        stream.skip(1); // tempo change duration

        if (stream.version == Gp::Version5_1)
            stream.skip(1);
    }

    if (stream.version >= Gp::Version4)
    {
        // Details of score-wide or track-specific changes.
        stream.read<uint8_t>();
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

// TODO - implement tremolo bar support.
#if 0
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
#endif

void GuitarProImporter::readNotes(Gp::InputStream &stream, Position &position)
{
    const Gp::Flags stringsPlayed = stream.read<uint8_t>();

    for (int i = Gp::NumberOfStrings - 1; i >= 0; i--)
    {
        if (stringsPlayed.test(i))
        {
            Note note;

            note.setString(Gp::NumberOfStrings - i - 1);

            const Gp::Flags flags = stream.read<uint8_t>();

            position.setProperty(Position::Marcato,
                                 flags.test(Gp::AccentedNote));

            if (stream.version > Gp::Version4)
            {
                position.setProperty(Position::Sforzando,
                                     flags.test(Gp::HeavyAccentedNote));
            }

            note.setProperty(Note::GhostNote, flags.test(Gp::GhostNote));

            // Ignore dotted note flag - already handled elsewhere for the
            // Position object.

            if (flags.test(Gp::NoteType))
            {
                const uint8_t noteType = stream.read<uint8_t>();
                note.setProperty(Note::Tied, noteType == Gp::TiedNote);
                note.setProperty(Note::Muted, noteType == Gp::MutedNote);
            }

            if (stream.version <= Gp::Version4 &&
                flags.test(Gp::TimeIndependentDuration))
            {
                // This is a repeat of the Position duration -- ignore.
                stream.skip(1);
                stream.skip(1);
            }

            if (flags.test(Gp::Dynamic))
            {
                stream.skip(1); // TODO - record the dynamic.
            }

            // If there is a non-empty note, read the fret number.
            if (flags.test(Gp::NoteType))
            {
                const uint8_t fret = stream.read<uint8_t>();
                note.setFretNumber(fret);
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
                    readNoteEffects(stream, position, note);
                else if (stream.version == Gp::Version3)
                    readNoteEffectsGp3(stream, position, note);
            }

            position.insertNote(note);
        }
    }
}

void GuitarProImporter::readNoteEffects(Gp::InputStream &stream,
                                        Position &position, Note &note)
{
    const Gp::Flags header1 = stream.read<uint8_t>();
    const Gp::Flags header2 = stream.read<uint8_t>();

    if (header1.test(Gp::HasBend))
        readBend(stream, note);

    if (header1.test(Gp::HasGraceNote))
    {
        // TODO - handle grace notes.
        stream.skip(1); // fret number grace note is made from
        stream.skip(1); // grace note dynamic
        stream.skip(1); // transition type
        stream.skip(1); // duration
        if (stream.version > Gp::Version4)
            stream.skip(1); // flags for GP5
    }

    if (header2.test(Gp::HasTremoloPicking))
    {
        // Ignore - Power Tab does not allow different values for the tremolo
        // picking duration (e.g. eighth notes).
        stream.skip(1);
        position.setProperty(Position::TremoloPicking);
    }

    if (header2.test(Gp::HasSlide))
        readSlide(stream, note);

    if (header2.test(Gp::HasHarmonic))
        readHarmonic(stream, note);

    if (header2.test(Gp::HasTrill))
    {
        const uint8_t fret = stream.read<uint8_t>();
        note.setTrilledFret(fret);

        // Trill duration - not supported in Power Tab.
        stream.skip(1);
    }

    position.setProperty(Position::LetRing, header1.test(Gp::HasLetRing));

    note.setProperty(Note::HammerOnOrPullOff,
                     header1.test(Gp::HasHammerOnOrPullOff));

    position.setProperty(Position::Vibrato, header2.test(Gp::HasVibrato));
    position.setProperty(Position::PalmMuting, header2.test(Gp::HasPalmMute));
    position.setProperty(Position::Staccato, header2.test(Gp::HasStaccato));
}

void GuitarProImporter::readNoteEffectsGp3(Gp::InputStream &stream,
                                           Position &position, Note &note)
{
    const Gp::Flags flags = stream.read<uint8_t>();

    position.setProperty(Position::LetRing, flags.test(Gp::HasLetRing));
    note.setProperty(Note::HammerOnOrPullOff,
                     flags.test(Gp::HasHammerOnOrPullOff));

    if (flags.test(Gp::HasSlideOutVer3))
        note.setProperty(Note::SlideOutOfDownwards);

    if (flags.test(Gp::HasBend))
        readBend(stream, note);

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
        switch (slideValue)
        {
            case -2:
                note.setProperty(Note::SlideIntoFromAbove);
                break;
            case -1:
                note.setProperty(Note::SlideIntoFromBelow);
                break;
            case 1:
                note.setProperty(Note::ShiftSlide);
                break;
            case 2:
                note.setProperty(Note::LegatoSlide);
                break;
            case 3:
                note.setProperty(Note::SlideOutOfDownwards);
                break;
            case 4:
                note.setProperty(Note::SlideOutOfUpwards);
                break;
        }
    }
    else
    {
        switch (slideValue)
        {
            case 1:
                note.setProperty(Note::ShiftSlide);
                break;
            case 2:
                note.setProperty(Note::LegatoSlide);
                break;
            case 4:
                note.setProperty(Note::SlideOutOfDownwards);
                break;
            case 8:
                note.setProperty(Note::SlideOutOfUpwards);
                break;
            case 16:
                note.setProperty(Note::SlideIntoFromBelow);
                break;
            case 32:
                note.setProperty(Note::SlideIntoFromAbove);
                break;
        }
    }
}

void GuitarProImporter::readHarmonic(Gp::InputStream &stream, Note &note)
{
    const uint8_t harmonic = stream.read<uint8_t>();

    if (harmonic == Gp::NaturalHarmonic)
    {
        note.setProperty(Note::NaturalHarmonic);
    }
    else if (harmonic == Gp::TappedHarmonic)
    {
        if (stream.version > Gp::Version4)
        {
            std::cerr << "Tapped Harmonic Data: " << (int)stream.read<uint8_t>()
                      << std::endl;
        }

        // TODO - fix this.
        note.setTappedHarmonicFret(note.getFretNumber());
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

#if 0
/// Converts bend pitches from GP format (25 per quarter tone) to PTB (1 per
/// quarter tone)
uint8_t GuitarProImporter::convertBendPitch(int32_t gpBendPitch)
{
    return std::abs(gpBendPitch / 25);
}
#endif
#endif
