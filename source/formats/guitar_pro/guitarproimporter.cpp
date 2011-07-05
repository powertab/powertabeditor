#include "guitarproimporter.h"

#include "gp_channel.h"

#include <fstream>
#include <cmath>
#include <iostream>

#include <formats/guitar_pro/inputstream.h>
#include <formats/guitar_pro/fileformat.h>

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

const std::vector<std::string> GuitarProImporter::gp4Versions = {"FICHIER GUITAR PRO v4.00",
                                                                "FICHIER GUITAR PRO v4.06",
                                                                "FICHIER GUITAR PRO L4.06"};

GuitarProImporter::GuitarProImporter() :
    FileFormatImporter(FileFormat("Guitar Pro 4", "*.gp4"))
{
}

std::shared_ptr<PowerTabDocument> GuitarProImporter::load(const std::string& fileName)
{
    std::ifstream in(fileName.c_str(), std::ios::binary | std::ios::in);
    Gp::InputStream stream(in);

    findFileVersion(stream);

    auto ptbDoc = std::make_shared<PowerTabDocument>();

    readHeader(stream, ptbDoc->GetHeader());

    Score* score = ptbDoc->GetGuitarScore();

    readStartTempo(stream, score);

    /*int8_t initialKey = */stream.read<int8_t>();
    stream.read<uint32_t>(); // octave

    const std::vector<Gp::Channel> channels = readChannels(stream);

    const uint32_t numMeasures = stream.read<uint32_t>();
    const uint32_t numTracks = stream.read<uint32_t>();

    std::vector<System::BarlinePtr> barlines = readBarlines(stream, numMeasures);
    readTracks(stream, score, numTracks, channels);

    readSystems(stream, score, barlines);
    fixRepeatEnds(score);

    return ptbDoc;
}

/// Check that the file version is supported
/// @throw FileFormatException
void GuitarProImporter::findFileVersion(Gp::InputStream& stream)
{
    std::string version = stream.readVersionString();

    if (std::find(gp4Versions.begin(), gp4Versions.end(), version) == gp4Versions.end())
    {
        throw FileFormatException("Unsupported file version: " + version);
    }
}

/// Read the song information (title, artist, etc)
void GuitarProImporter::readHeader(Gp::InputStream& stream, PowerTabFileHeader& ptbHeader)
{
    ptbHeader.SetSongTitle(stream.readString());
    stream.readString(); // ignore subtitle (no .ptb equivalent)

    ptbHeader.SetSongArtist(stream.readString());
    ptbHeader.SetSongAudioReleaseTitle(stream.readString());
    ptbHeader.SetSongComposer(stream.readString());

    ptbHeader.SetSongCopyright(stream.readString());
    ptbHeader.SetSongGuitarScoreTranscriber(stream.readString());

    stream.readString(); // TODO - not sure what the "instructional" parameter is used for ...

    std::string comments;
    const uint32_t numComments = stream.read<uint32_t>();
    for (uint32_t i = 0; i < numComments; i++)
    {
        comments += stream.readString();
    }
    ptbHeader.SetSongGuitarScoreNotes(comments);

    stream.read<uint8_t>(); // TripletFeel attribute

    // read lyrics
    std::string lyrics;
    stream.read<uint32_t>(); // track number that the lyrics are associated with

    for (int i = 0; i < 5; i++) // 5 lines of lyrics
    {
        stream.read<uint32_t>(); // measure associated with the line
        lyrics += stream.readIntString();
    }

    ptbHeader.SetSongLyrics(lyrics);
}

/// Read the midi channels (i.e. mixer settings)
std::vector<Gp::Channel> GuitarProImporter::readChannels(Gp::InputStream& stream)
{
    std::vector<Gp::Channel> channels;

    // Ignore for now
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

        // unused (gp3 compatibility??)
        stream.read<uint8_t>();
        stream.read<uint8_t>();

        channels.push_back(channel);
    }

    return channels;
}

std::vector<System::BarlinePtr> GuitarProImporter::readBarlines(Gp::InputStream& stream,
                                                                uint32_t numMeasures)
{
    std::vector<System::BarlinePtr> barlines;
    char nextRehearsalSignletter = 'A';

    for (uint32_t i = 0; i < numMeasures; i++)
    {
        auto barline = std::make_shared<Barline>();

        if (i != 0)
        {
            auto prevBarline = barlines.back();

            // clone time signature, key signature from previous barline
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
            stream.read<uint8_t>(); // TODO - properly import alternate endings
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

    return barlines;
}

void GuitarProImporter::readColor(Gp::InputStream& stream)
{
    // ignore, since PowerTab doesn't currently have any use for the colors in GP files
    stream.read<uint32_t>(); // 4 bytes - red, green, blue, white
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

        // flags used for indicating drum tracks, banjo tracks, etc
        stream.read<uint8_t>();

        guitar->SetDescription(stream.readFixedLengthString(Gp::TrackDescriptionLength));

        guitar->SetTuning(readTuning(stream));

        stream.read<uint32_t>(); // MIDI port used - ignore (Power Tab handles this)

        const uint32_t channelIndex = stream.read<uint32_t>(); // MIDI channel used

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

        stream.read<uint32_t>(); // MIDI channel used for effects - ignore

        stream.read<uint32_t>(); // number of frets

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
        uint32_t tuningNote = stream.read<uint32_t>();

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
                                    const std::vector<System::BarlinePtr>& barlines)
{
    std::vector<uint8_t> staffSizes;
    for (uint32_t guitar = 0; guitar < score->GetGuitarCount(); guitar++)
    {
        staffSizes.push_back(score->GetGuitar(guitar)->GetStringCount());
    }

    Score::SystemPtr currentSystem = std::make_shared<System>();
    currentSystem->Init(staffSizes);
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
        if (!currentSystem->IsValidPosition(lastBarlinePos + largestMeasure))
        {
            currentSystem = std::make_shared<System>();
            currentSystem->Init(staffSizes);

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

            lastBarlinePos += largestMeasure + 1;
            ++currentBarline;
        }
    }
}

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
            // TODO - handle empty position
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
        stream.readIntString(); // TODO - not sure what to do with this text (floating text???)
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
                // ignore - this is a repeat of the Position duration
                stream.read<uint8_t>();
                stream.read<uint8_t>();
            }

            if (flags.test(Gp::Dynamic))
            {
                stream.read<uint8_t>(); // TODO - convert into a Dynamic object
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
                stream.read<int8_t>();
                stream.read<int8_t>();
            }

            if (flags.test(Gp::NoteEffects))
            {
                readNoteEffects(stream, position, note);
            }

            position.InsertNote(note.CloneObject());
        }
    }
}

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
        stream.read<uint8_t>(); // fret number grace note is made from
        stream.read<uint8_t>(); // grace note dynamic
        stream.read<uint8_t>(); // transition type
        stream.read<uint8_t>(); // duration

        // TODO - will need to add an extra note to be the grace note
    }

    if (header2.test(Gp::HasTremoloPicking))
    {
        // ignore - Power Tab does not allow different values for the tremolo picking duration (e.g. eighth notes)
        stream.read<uint8_t>();
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

        stream.read<uint8_t>(); // ignore trill duration (duration is a fixed value in Power Tab)
    }

    position.SetLetRing(header1.test(Gp::HasLetRing));

    note.SetHammerOn(header1.test(Gp::HasHammerOnOrPullOff)); // TODO - check whether a pulloff needs to be set instead

    position.SetVibrato(header2.test(Gp::HasVibrato));
    position.SetPalmMuting(header2.test(Gp::HasPalmMute));
    position.SetStaccato(header2.test(Gp::HasStaccato));
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
        stream.read<uint32_t>(); // time relative to the previous point
        stream.read<uint32_t>(); // bend position
        stream.read<uint8_t>(); // bend vibrato
    }
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

    stream.read<uint8_t>(); // details of score-wide or track-specific changes
}

void GuitarProImporter::readPositionEffects(Gp::InputStream& stream, Position& position)
{
    const Gp::Flags flags1 = stream.read<uint8_t>();
    const Gp::Flags flags2 = stream.read<uint8_t>();

    if (flags1.test(Gp::HasTapping))
    {
        const uint8_t type = stream.read<uint8_t>();

        position.SetTap(type == Gp::Tapping);
        // Ignore slapping and popping
    }

    if (flags2.test(Gp::HasTremoloBarEvent))
    {
        // TODO - replace with method that actually reads the tremolo bar data
        Note note;
        readBend(stream, note);
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

    position.SetTremoloPicking(flags2.test(Gp::HasRasguedo));

    if (flags2.test(Gp::Pickstroke))
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

void GuitarProImporter::readChordDiagram(Gp::InputStream& stream)
{
    const Gp::Flags header = stream.read<uint8_t>();

    if (!header.test(Gp::Gp4ChordFormat))
    {
        throw FileFormatException("Unsupported Chord Diagram format");
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
