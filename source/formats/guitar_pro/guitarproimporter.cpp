#include "guitarproimporter.h"

#include "gp_channel.h"

#include <fstream>
#include <cmath>
#include <iostream>

#include <formats/guitar_pro/inputstream.h>

#include <powertabdocument/powertabdocument.h>
#include <powertabdocument/powertabfileheader.h>
#include <powertabdocument/barline.h>
#include <powertabdocument/system.h>
#include <powertabdocument/guitar.h>
#include <powertabdocument/score.h>
#include <powertabdocument/tuning.h>
#include <powertabdocument/position.h>
#include <powertabdocument/note.h>
#include <powertabdocument/staff.h>
#include <powertabdocument/tempomarker.h>
#include <powertabdocument/chorddiagram.h>
#include <powertabdocument/alternateending.h>

const std::map<std::string, Gp::Version> GuitarProImporter::versionStrings = {
    {"FICHIER GUITAR PRO v3.00", Gp::Version3},
    {"FICHIER GUITAR PRO v4.00", Gp::Version4},
    {"FICHIER GUITAR PRO v4.06", Gp::Version4},
    {"FICHIER GUITAR PRO L4.06", Gp::Version4}
};

// Use a slightly smaller position spacing than the default in order to reduce the number of systems needed
const uint8_t GuitarProImporter::DEFAULT_POSITION_SPACING = 15;

GuitarProImporter::GuitarProImporter() :
    FileFormatImporter(FileFormat("Guitar Pro 3, 4", "*.gp3 *.gp4"))
{
}

std::shared_ptr<PowerTabDocument> GuitarProImporter::load(const std::string& fileName)
{
    std::ifstream in(fileName.c_str(), std::ios::binary | std::ios::in);
    Gp::InputStream stream(in);

    findFileVersion(stream);

    auto ptbDoc = std::make_shared<PowerTabDocument>();
    Score* score = ptbDoc->GetGuitarScore();

    readHeader(stream, ptbDoc->GetHeader());

    readStartTempo(stream, score);

    stream.skip(4); // initial key -- ignore

    if (stream.version >= Gp::Version4)
    {
        stream.skip(1); // octave 8va -- ignore
    }

    const std::vector<Gp::Channel> channels = readChannels(stream);

    const uint32_t numMeasures = stream.read<uint32_t>();
    const uint32_t numTracks = stream.read<uint32_t>();

    BarlineList barlines;
    AlternateEndingsMap altEndings;
    readBarlines(stream, numMeasures, barlines, altEndings);

    readTracks(stream, score, numTracks, channels);

    readSystems(stream, score, barlines, altEndings);
    fixRepeatEnds(score);

    return ptbDoc;
}

/// Check that the file version string is valid, and set the version flag for the input stream
/// @throw FileFormatException
void GuitarProImporter::findFileVersion(Gp::InputStream& stream)
{
    const std::string versionString = stream.readVersionString();

    auto versionStringIt = versionStrings.find(versionString);

    if (versionStringIt != versionStrings.end())
    {
        stream.version = versionStringIt->second;
    }
    else
    {
        throw FileFormatException("Unsupported file version: " + versionString);
    }
}

/// Read the song information (title, artist, etc)
void GuitarProImporter::readHeader(Gp::InputStream& stream, PowerTabFileHeader& ptbHeader)
{
    ptbHeader.SetSongTitle(stream.readString());
    stream.readString(); // subtitle -- ignore

    ptbHeader.SetSongArtist(stream.readString());
    ptbHeader.SetSongAudioReleaseTitle(stream.readString());
    ptbHeader.SetSongComposer(stream.readString());

    ptbHeader.SetSongCopyright(stream.readString());
    ptbHeader.SetSongGuitarScoreTranscriber(stream.readString());

    const std::string instructions = stream.readString();

    std::string comments;

    if (!instructions.empty())
    {
        comments += instructions + "\n";
    }

    // read several lines of comments
    const uint32_t numComments = stream.read<uint32_t>();
    for (uint32_t i = 0; i < numComments; i++)
    {
        comments += stream.readString();
    }
    ptbHeader.SetSongGuitarScoreNotes(comments);

    stream.skip(1); // triplet feel attribute -- ignore

    if (stream.version >= Gp::Version4)
    {
        // read lyrics
        std::string lyrics;
        stream.skip(4); // track number that the lyrics are associated with -- ignore

        for (int i = 0; i < Gp::NumberOfLinesOfLyrics; i++) // read several lines of lyrics
        {
            stream.skip(4); // measure associated with the line -- ignore
            lyrics += stream.readIntString();
        }

        ptbHeader.SetSongLyrics(lyrics);
    }
}

/// Read the midi channels (i.e. mixer settings)
/// We store them into a temporary structure (Gp::Channel) since they must be
/// referenced later when importing the tracks. Eventually, the data is used
/// by the Guitar class
std::vector<Gp::Channel> GuitarProImporter::readChannels(Gp::InputStream& stream)
{
    std::vector<Gp::Channel> channels;

    for (int i = 0; i < Gp::NumberOfMidiChannels; i++) // 64 MIDI channels
    {
        Gp::Channel channel;

        channel.instrument = stream.read<uint32_t>();
        channel.volume = Gp::Channel::readChannelProperty(stream);
        channel.balance = Gp::Channel::readChannelProperty(stream);
        channel.chorus = Gp::Channel::readChannelProperty(stream);
        channel.reverb = Gp::Channel::readChannelProperty(stream);
        channel.phaser = Gp::Channel::readChannelProperty(stream);
        channel.tremolo = Gp::Channel::readChannelProperty(stream);

        // ignored (compatibility with older versions
        stream.skip(2);

        channels.push_back(channel);
    }

    return channels;
}

/// Reads all of the measures in the score, and any alternate endings that occur
void GuitarProImporter::readBarlines(Gp::InputStream& stream, uint32_t numMeasures,
                                     BarlineList& barlines, AlternateEndingsMap& altEndings)
{
    char nextRehearsalSignletter = 'A';

    for (uint32_t measure = 0; measure < numMeasures; measure++)
    {
        auto barline = std::make_shared<Barline>();

        // clone time signature, key signature from previous barline if possible
        if (measure != 0)
        {
            auto prevBarline = barlines.back();

            barline->SetTimeSignature(prevBarline->GetTimeSignature());
            barline->GetTimeSignature().SetShown(false);

            barline->SetKeySignature(prevBarline->GetKeySignature());
            barline->GetKeySignature().SetShown(false);
        }

        TimeSignature& timeSignature = barline->GetTimeSignature();

        const Gp::Flags flags = stream.read<uint8_t>();

        if (flags.test(Gp::Numerator))
        {
            timeSignature.SetShown(true);
            timeSignature.SetBeatsPerMeasure(stream.read<uint8_t>());
        }
        if (flags.test(Gp::Denominator))
        {
            timeSignature.SetShown(true);
            timeSignature.SetBeatAmount(stream.read<uint8_t>());
        }

        if (flags.test(Gp::RepeatBegin))
        {
            barline->SetType(Barline::repeatStart);
        }

        if (flags.test(Gp::RepeatEnd))
        {
            // we don't set the repeatEnd type here - see the 'fixRepeatEnds' function for details
            barline->SetRepeatCount(stream.read<uint8_t>() + 1);
        }

        if (flags.test(Gp::AlternateEnding))
        {
            auto altEnding = std::make_shared<AlternateEnding>();
            altEnding->SetNumber(stream.read<uint8_t>());

            // associate the alternate ending with the barline index - we will insert the Alternate Ending
            // into the score later, once we know its exact location (i.e. after layout is figured out)
            altEndings[measure] = altEnding;
        }

        if (flags.test(Gp::Marker)) // import rehearsal sign
        {
            RehearsalSign& sign = barline->GetRehearsalSign();

            const std::string description = stream.readString();

            if (!description.empty())
            {
                sign.SetDescription(description);
            }

            // set the rehearsal sign letter to the next available letter
            if (RehearsalSign::IsValidLetter(nextRehearsalSignletter))
            {
                sign.SetLetter(nextRehearsalSignletter);
                nextRehearsalSignletter++;
            }
            else
            {
                std::cerr << "Too many rehearsal signs! (only A-Z allowed)" << std::endl;
            }

            readColor(stream);
        }

        if (flags.test(Gp::KeySignatureChange))
        {
            KeySignature& keySignature = barline->GetKeySignature();
            keySignature.SetShown(true);

            keySignature.SetKeyType(stream.read<uint8_t>()); // tonality type (minor/major)
            keySignature.SetKeyAccidentals(convertKeyAccidentals(stream.read<int8_t>())); // key accidentals
        }

        if (flags.test(Gp::DoubleBar))
        {
            barline->SetType(Barline::doubleBar);
        }

        barlines.push_back(barline);
    }
}

void GuitarProImporter::readColor(Gp::InputStream& stream)
{
    // ignore, since PowerTab doesn't currently have any use for the colors in GP files
    stream.skip(4); // 4 bytes - red, green, blue, white
}

/// Converts key accidentals from the Guitar Pro format to Power Tab
uint8_t GuitarProImporter::convertKeyAccidentals(int8_t gpKey)
{
    // Guitar Pro uses 0 for C, 1 for G, ..., -1 for F, -2 for Bb, ..., whereas
    // Power Tab uses 0 for 1, 1 for G, ..., 8 for F, 9 for Bb, etc
    if (gpKey < 0)
    {
        return KeySignature::sevenSharps + abs(gpKey);
    }
    else
    {
        return gpKey;
    }
}

/// Read and convert all tracks (guitars)
void GuitarProImporter::readTracks(Gp::InputStream& stream, Score* score, uint32_t numTracks,
                                   const std::vector<Gp::Channel>& channels)
{
    for (uint32_t i = 0; i < numTracks; ++i)
    {
        Score::GuitarPtr guitar = std::make_shared<Guitar>();
        guitar->SetNumber(score->GetGuitarCount());

        stream.skip(1); // flags used for indicating drum tracks, banjo tracks, etc -- ignore

        guitar->SetDescription(stream.readFixedLengthString(Gp::TrackDescriptionLength));

        guitar->SetTuning(readTuning(stream));

        stream.skip(4); // MIDI port used -- ignore (Power Tab handles this)

        const uint32_t channelIndex = stream.read<uint32_t>(); // Index of MIDI channel used

        // find the specified channel and copy its information (in Power Tab, the Guitar class
        // stores the MIDI information along with tuning, etc)
        if (channelIndex < channels.size())
        {
            const Gp::Channel& channel = channels[channelIndex];

            guitar->SetPreset(channel.instrument);
            guitar->SetInitialVolume(channel.volume);
            guitar->SetPan(channel.balance);
            guitar->SetChorus(channel.chorus);
            guitar->SetReverb(channel.reverb);
            guitar->SetPhaser(channel.phaser);
            guitar->SetTremolo(channel.tremolo);
        }
        else
        {
            std::cerr << "Invalid channel index: " << channelIndex <<
                         " for track: " << i << std::endl;
        }

        stream.skip(4); // MIDI channel used for effects -- ignore

        stream.skip(4); // number of frets -- ignore

        guitar->SetCapo(stream.read<uint32_t>());

        readColor(stream); // ignore track color

        score->InsertGuitar(guitar);
    }
}

/// Imports a Guitar Pro tuning and converts it into a Power Tab tuning
Tuning GuitarProImporter::readTuning(Gp::InputStream& stream)
{
    Tuning tuning;

    const uint32_t numStrings = stream.read<uint32_t>();

    std::vector<uint8_t> tuningNotes;

    /// Gp::NumberOfStrings integers are in the file, but only the first "numStrings" are actually used
    for (uint32_t i = 0; i < Gp::NumberOfStrings; i++)
    {
        const uint32_t tuningNote = stream.read<uint32_t>();

        if (i < numStrings)
        {
            tuningNotes.push_back(tuningNote);
        }
    }

    tuning.SetTuningNotes(tuningNotes);
    assert(tuning.IsValid());

    return tuning;
}

void GuitarProImporter::readSystems(Gp::InputStream& stream, Score* score,
                                    const BarlineList& barlines, const AlternateEndingsMap& altEndings)
{
    std::vector<uint8_t> staffSizes;
    for (uint32_t guitar = 0; guitar < score->GetGuitarCount(); guitar++)
    {
        staffSizes.push_back(score->GetGuitar(guitar)->GetStringCount());
    }

    Score::SystemPtr currentSystem = std::make_shared<System>();
    currentSystem->Init(staffSizes);
    currentSystem->SetPositionSpacing(DEFAULT_POSITION_SPACING);
    score->InsertSystem(currentSystem, 0);

    auto currentBarline = barlines.begin();
    uint32_t lastBarlinePos = 0;

    for (uint32_t measure = 0; measure < barlines.size(); measure++)
    {
        std::vector<std::vector<Position*> > positionLists(score->GetGuitarCount());
        size_t largestMeasure = 0;

        for (uint32_t track = 0; track < score->GetGuitarCount(); track++)
        {
            std::vector<Position*>& positionList = positionLists.at(track);

            const uint32_t numBeats = stream.read<uint32_t>(); // number of beats in measure

            for (uint32_t k = 0; k < numBeats; k++)
            {
                positionList.push_back(readBeat(stream));
            }

            largestMeasure = std::max(largestMeasure, positionList.size());
        }

        // check if we need to jump to a new system (measure is too large)
        if (!currentSystem->IsValidPosition(lastBarlinePos + largestMeasure + 1))
        {
            currentSystem = std::make_shared<System>();
            currentSystem->Init(staffSizes);
            currentSystem->SetPositionSpacing(DEFAULT_POSITION_SPACING);

            // adjust height to be below the previous system
            const Rect prevRect = score->GetSystem(score->GetSystemCount() - 1)->GetRect();
            Rect currentRect = currentSystem->GetRect();
            currentRect.SetTop(prevRect.GetBottom() + Score::SYSTEM_SPACING);
            currentSystem->SetRect(currentRect);

            score->InsertSystem(currentSystem, score->GetSystemCount());
            lastBarlinePos = 0;

            // ensure that there is enough space in the staff for all notes of the measure
            if (!currentSystem->IsValidPosition(largestMeasure))
            {
                currentSystem->SetPositionSpacing(System::MIN_POSITION_SPACING);
            }
        }

        // insert positions
        for (uint32_t track = 0; track < score->GetGuitarCount(); track++)
        {
            const std::vector<Position*>& positionList = positionLists.at(track);
            System::StaffPtr currentStaff = currentSystem->GetStaff(track);

            for (uint32_t posIndex = 0; posIndex < positionList.size(); posIndex++)
            {
                Position* currentPos = positionList.at(posIndex);
                currentPos->SetPosition(lastBarlinePos + posIndex);
                currentStaff->InsertPosition(0, currentPos);
            }

            currentStaff->CalculateClef(score->GetGuitar(track)->GetTuning());
        }

        // insert barline
        if (currentBarline != barlines.end())
        {
            if (lastBarlinePos == 0)
            {
                currentSystem->SetStartBar(*currentBarline);
                (*currentBarline)->SetPosition(0);
            }
            else
            {
                (*currentBarline)->SetPosition(lastBarlinePos - 1);
                currentSystem->InsertBarline(*currentBarline);
            }

            {
                // insert alternate ending (associated with the bar) if neccessary
                const uint32_t position = (*currentBarline)->GetPosition();
                auto altEndingIt = altEndings.find(position);
                if (altEndingIt != altEndings.end())
                {
                    Score::AlternateEndingPtr altEnding = altEndingIt->second;
                    altEnding->SetSystem(score->GetSystemCount() - 1);
                    altEnding->SetPosition(position);
                    score->InsertAlternateEnding(altEnding);
                }
            }

            lastBarlinePos += largestMeasure + 1;
            ++currentBarline;
        }
    }

    // calculate the beaming for all notes, and calculate the layout of the systems
    for (size_t i = 0; i < score->GetSystemCount(); i++)
    {
        Score::SystemPtr system = score->GetSystem(i);
        system->CalculateBeamingForStaves();
        score->UpdateSystemHeight(system);
    }
}

/// Reads a beat (Guitar Pro equivalent of a Position in Power Tab)
Position* GuitarProImporter::readBeat(Gp::InputStream& stream)
{
    const Gp::Flags flags = stream.read<uint8_t>();

    Position pos;

    pos.SetDotted(flags.test(Gp::Dotted));

    if (flags.test(Gp::BeatStatus))
    {
        const uint8_t status = stream.read<uint8_t>();

        if (status == Gp::BeatEmpty)
        {
            // TODO - handle empty position?
        }
        else if (status == Gp::BeatRest)
        {
            pos.SetRest(true);
        }
    }

    pos.SetDurationType(readDuration(stream));

    if (flags.test(Gp::IrregularGrouping))
    {
        const uint32_t notesPlayed = stream.read<uint32_t>(); // notes played in the irregular grouping

        // the "denominator" of the irregular grouping is the nearest power of 2 (from below)
        const uint32_t notesOver = pow(2, floor(log2(notesPlayed)));

        pos.SetIrregularGroupingTiming(notesPlayed, notesOver);
        pos.SetIrregularGroupingStart(true); // TODO - need to group with other notes properly
    }

    if (flags.test(Gp::ChordDiagram))
    {
        readChordDiagram(stream);
    }

    if (flags.test(Gp::Text))
    {
        stream.readString(); // TODO - not sure what to do with this text (floating text???)
    }

    if (flags.test(Gp::NoteEffects))
    {
        readPositionEffects(stream, pos);
    }

    if (flags.test(Gp::MixTableChangeEvent))
    {
        readMixTableChangeEvent(stream);
    }

    readNotes(stream, pos);

    return pos.CloneObject();
}

/// Reads a duration value and converts it into PTB format
uint8_t GuitarProImporter::readDuration(Gp::InputStream& stream)
{
    const int8_t gpDuration = stream.read<int8_t>();

    // Durations for Guitar Pro are stored as 0 -> quarter note, -1 -> half note, 1 -> eight note, etc
    // We need to convert to 1 = whole note, 2 = half note, 4 = quarter note, etc
    return pow(2, gpDuration + 2);
}

/// Reads the notes for a given position
void GuitarProImporter::readNotes(Gp::InputStream& stream, Position& position)
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

            note.SetGhostNote(flags.test(Gp::GhostNote));
            // ignore dotted note flag - already handled elsewhere for the Position object

            if (flags.test(Gp::NoteType))
            {
                const uint8_t noteType = stream.read<uint8_t>();
                note.SetTied(noteType == Gp::TiedNote);
                note.SetMuted(noteType == Gp::MutedNote);
            }

            if (flags.test(Gp::TimeIndependentDuration))
            {
                // this is a repeat of the Position duration -- ignore
                stream.skip(1);
                stream.skip(1);
            }

            if (flags.test(Gp::Dynamic))
            {
                stream.skip(1); // TODO - convert into a Dynamic object
            }

            if (flags.test(Gp::NoteType)) // if there is a non-empty note, read fret number
            {
                // TODO - drum tracks will crash here because of fret numbers larger than 29
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
/// Note: some of the effects apply to the entire Position (not just a single note),
/// due to differences between Guitar Pro and Power Tab notation
void GuitarProImporter::readNoteEffects(Gp::InputStream& stream,
                                        Position& position, Note& note)
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
    }

    if (header2.test(Gp::HasTremoloPicking))
    {
        // ignore - Power Tab does not allow different values for the tremolo picking duration (e.g. eighth notes)
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

        stream.skip(1); // ignore trill duration (duration is a fixed value in Power Tab)
    }

    position.SetLetRing(header1.test(Gp::HasLetRing));

    note.SetHammerOn(header1.test(Gp::HasHammerOnOrPullOff)); // TODO - check whether a pulloff needs to be set instead

    position.SetVibrato(header2.test(Gp::HasVibrato));
    position.SetPalmMuting(header2.test(Gp::HasPalmMute));
    position.SetStaccato(header2.test(Gp::HasStaccato));
}

/// Note effects are sufficiently different in GP3 to make a separate function necessary
void GuitarProImporter::readNoteEffectsGp3(Gp::InputStream& stream,
                                           Position& position, Note& note)
{
    const Gp::Flags flags = stream.read<uint8_t>();

    position.SetLetRing(flags.test(Gp::HasLetRing));
    note.SetHammerOn(flags.test(Gp::HasHammerOnOrPullOff)); // TODO - check whether a pulloff needs to be set instead

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

void GuitarProImporter::readSlide(Gp::InputStream& stream, Note& note)
{
    const int8_t slideValue = stream.read<uint8_t>();

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
        note.SetSlideInto(abs(slideValue));
    }
    else if (slideValue > 0)
    {
        note.SetSlideOutOf(slideValue, 0); // We don't know the number of steps for the slide yet
    }
}

void GuitarProImporter::readHarmonic(Gp::InputStream& stream, Note& note)
{
    const uint8_t harmonic = stream.read<uint8_t>();

    if (harmonic == Gp::NaturalHarmonic)
    {
        note.SetNaturalHarmonic(true);
    }
    if (harmonic == Gp::TappedHarmonic)
    {
        note.SetTappedHarmonic(true);
    }

    // TODO - handle artificial harmonics
}

void GuitarProImporter::readBend(Gp::InputStream& stream, Note&)
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

void GuitarProImporter::readTremoloBar(Gp::InputStream& stream, Position& position)
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

    const uint32_t pitch = convertBendPitch(stream.read<uint32_t>());

    position.SetTremoloBar(eventType, 0, pitch);

    if (stream.version >= Gp::Version4)
    {
        const uint32_t numPoints = stream.read<uint32_t>(); // number of bend points
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
    switch(gpEventType)
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
        std::cerr << "Invalid tremolo bar event type: " << (int)gpEventType << std::endl;
        return -1;
    }
    }
}

/// Converts bend pitches from GP format (25 per quarter tone) to PTB (1 per quarter tone)
uint8_t GuitarProImporter::convertBendPitch(uint32_t gpBendPitch)
{
    return gpBendPitch / 25;
}

void GuitarProImporter::readMixTableChangeEvent(Gp::InputStream& stream)
{
    // TODO - implement conversions for this

    stream.read<int8_t>(); // instrument
    int8_t volume = stream.read<int8_t>(); // volume
    int8_t pan = stream.read<uint8_t>(); // pan
    int8_t chorus = stream.read<uint8_t>(); // chorus
    int8_t reverb = stream.read<uint8_t>(); // reverb
    int8_t phaser = stream.read<uint8_t>(); // phaser
    int8_t tremolo = stream.read<uint8_t>(); // tremolo

    int32_t tempo = stream.read<int32_t>(); // new tempo

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
        stream.read<uint8_t>(); // tempo change duration
    }

    if (stream.version >= Gp::Version4)
    {
        stream.read<uint8_t>(); // details of score-wide or track-specific changes
    }
}

void GuitarProImporter::readPositionEffects(Gp::InputStream& stream, Position& position)
{
    const Gp::Flags flags1 = stream.read<uint8_t>();

    Gp::Flags flags2; // only read this if we are in GP4 or higher

    // GP3 effect decoding
    if (stream.version == Gp::Version3)
    {
        position.SetVibrato(flags1.test(Gp::VibratoGp3_1) || flags1.test(Gp::VibratoGp3_2));

        // FIXME - in Power Tab, harmonic correspond to notes, not to positions (beats)
        // However, when the Position effects are being read, the notes haven't been read yet ...
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

        position.SetTap(type == Gp::Tapping);
        // Ignore slapping and popping
    }

    if (stream.version >= Gp::Version4 && flags2.test(Gp::HasTremoloBarEvent))
    {
        readTremoloBar(stream, position);
    }

    if (flags1.test(Gp::HasStrokeEffect))
    {
        // upstroke and downstroke duration values - we will just use these for toggling arpeggio up/down
        if (stream.read<uint8_t>() > 0)
        {
            position.SetArpeggioUp();
        }
        if (stream.read<uint8_t>() > 0)
        {
            position.SetArpeggioDown();
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
            position.SetPickStrokeUp(true);
        }
        else if (pickstrokeType == Gp::PickstrokeDown)
        {
            position.SetPickStrokeDown(true);
        }
    }
}

/// TODO - implement reading of chord diagrams
void GuitarProImporter::readChordDiagram(Gp::InputStream& stream)
{
    const Gp::Flags header = stream.read<uint8_t>();

    if (!header.test(Gp::Gp4ChordFormat))
    {
        readOldStyleChord(stream);
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

    stream.read<uint8_t>(); // base fret of the chord

    // fret numbers for each string
    for (int i = 0; i < Gp::NumberOfStrings; i++)
    {
        stream.read<int8_t>();
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
void GuitarProImporter::readOldStyleChord(Gp::InputStream& stream)
{
    ChordDiagram diagram;

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

/// Read the initial tempo in the score
void GuitarProImporter::readStartTempo(Gp::InputStream& stream, Score* score)
{
    Score::TempoMarkerPtr tempo(new TempoMarker(0, 0, false));
    tempo->SetBeatType(TempoMarker::quarter);
    tempo->SetBeatsPerMinute(stream.read<uint32_t>());

    score->InsertTempoMarker(tempo);
}

/// In Guitar Pro, repeat ends are stored with the current barline, not with the next barline
/// This method adjusts that by shifting repeat ends forward
/// TODO - need to deal with consecutive pairs of repeat start/end bars
void GuitarProImporter::fixRepeatEnds(Score* score)
{
    for (size_t i = 0; i < score->GetSystemCount(); i++)
    {
        Score::SystemPtr currentSystem = score->GetSystem(i);
        std::vector<System::BarlinePtr> barlines;
        currentSystem->GetBarlines(barlines);

        for (size_t j = 0; j < barlines.size() - 1; j++)
        {
            System::BarlinePtr currentBarline = barlines[j];
            System::BarlinePtr nextBarline = barlines[j+1];

            if (currentBarline->GetRepeatCount() >= Barline::MIN_REPEAT_COUNT &&
                    !currentBarline->IsRepeatEnd())
            {
                nextBarline->SetType(Barline::repeatEnd);
                nextBarline->SetRepeatCount(currentBarline->GetRepeatCount());
            }
        }
    }
}
