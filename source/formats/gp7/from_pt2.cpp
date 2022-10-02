/*
 * Copyright (C) 2020 Cameron White
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

#include "from_pt2.h"

#include "document.h"

#include <score/generalmidi.h>
#include <score/score.h>
#include <score/scorelocation.h>
#include <score/scoreinfo.h>
#include <score/utils.h>
#include <score/voiceutils.h>
#include <util/tostring.h>

#include <set>
#include <unordered_map>
#include <unordered_set>

/// Convert the Guitar Pro file metadata.
static Gp7::ScoreInfo
convertScoreInfo(const Score &score)
{
    const ScoreInfo &info = score.getScoreInfo();
    Gp7::ScoreInfo gp_info;

    if (info.getScoreType() == ScoreInfo::ScoreType::Song)
    {
        const SongData &song_data = info.getSongData();
        gp_info.myTitle = song_data.getTitle();
        gp_info.mySubtitle = song_data.getSubtitle();
        gp_info.myArtist = song_data.getArtist();

        if (song_data.isAudioRelease())
            gp_info.myAlbum = song_data.getAudioReleaseInfo().getTitle();
        else if (song_data.isVideoRelease())
            gp_info.myAlbum = song_data.getVideoReleaseInfo().getTitle();
        else if (song_data.isBootleg())
            gp_info.myAlbum = song_data.getBootlegInfo().getTitle();

        if (!song_data.isTraditionalAuthor())
        {
            gp_info.myMusic = song_data.getAuthorInfo().getComposer();
            gp_info.myWords = song_data.getAuthorInfo().getLyricist();
        }

        gp_info.myCopyright = song_data.getCopyright();
        gp_info.myTabber = song_data.getTranscriber();
        gp_info.myInstructions = song_data.getPerformanceNotes();
    }
    else
    {
        const LessonData &lesson_data = info.getLessonData();
        gp_info.myTitle = lesson_data.getTitle();
        gp_info.mySubtitle = lesson_data.getSubtitle();

        gp_info.myMusic = lesson_data.getAuthor();
        gp_info.myCopyright = lesson_data.getCopyright();
        gp_info.myInstructions = lesson_data.getNotes();
    }

    // Record the number of bars in each system (one less than the number of
    // barlines).
    for (const System &system : score.getSystems())
        gp_info.myScoreSystemsLayout.push_back(system.getBarlines().size() - 1);

    return gp_info;
}

static Gp7::ChordName
convertChordName(const ChordName &name)
{
    Gp7::ChordName gp_name;
    using Accidental = Gp7::ChordName::Note::Accidental;
    using Degree = Gp7::ChordName::Degree;
    using Alteration = Degree::Alteration;

    static const std::string theKeys[] = { "C", "D", "E", "F", "G", "A", "B" };
    gp_name.myKeyNote.myStep = theKeys[name.getTonicKey()];
    gp_name.myKeyNote.myAccidental =
        static_cast<Accidental>(name.getTonicVariation());

    gp_name.myBassNote.myStep = theKeys[name.getBassKey()];
    gp_name.myBassNote.myAccidental =
        static_cast<Accidental>(name.getBassVariation());

    // Fifth
    switch (name.getFormula())
    {
        case ChordName::Augmented:
        case ChordName::Augmented7th:
            gp_name.myFifth = Alteration::Augmented;
            break;
        case ChordName::Diminished:
        case ChordName::Diminished7th:
        case ChordName::Minor7thFlatted5th:
            gp_name.myFifth = Alteration::Diminished;
            break;
        default:
            if (name.hasModification(ChordName::Flatted5th))
                gp_name.myFifth = Alteration::Diminished;
            else if (name.hasModification(ChordName::Raised5th))
                gp_name.myFifth = Alteration::Augmented;
            else
                gp_name.myFifth = Alteration::Perfect;
            break;
    }

    // Third
    switch (name.getFormula())
    {
        case ChordName::Major:
        case ChordName::Augmented:
        case ChordName::Major6th:
        case ChordName::Dominant7th:
        case ChordName::Major7th:
        case ChordName::Augmented7th:
            gp_name.myThird = Alteration::Major;
            break;
        case ChordName::Minor:
        case ChordName::Diminished:
        case ChordName::Minor6th:
        case ChordName::Minor7th:
        case ChordName::Diminished7th:
        case ChordName::MinorMajor7th:
        case ChordName::Minor7thFlatted5th:
            gp_name.myThird = Alteration::Minor;
            break;
        default:
            break;
    }

    // Second
    if (name.hasModification(ChordName::Added2nd) ||
        name.hasModification(ChordName::Suspended2nd))
    {
        gp_name.mySecond = Alteration::Perfect;
    }

    // Fourth
    if (name.hasModification(ChordName::Added4th) ||
        name.hasModification(ChordName::Suspended4th))
    {
        gp_name.myFourth = Alteration::Perfect;
    }

    // Clear out the third for sus chords.
    if (name.hasModification(ChordName::Suspended2nd) ||
        name.hasModification(ChordName::Suspended4th))
    {
        gp_name.myThird.reset();
    }

    // Sixth
    switch (name.getFormula())
    {
        case ChordName::Major6th:
        case ChordName::Minor6th:
            gp_name.mySixth = Alteration::Major;
            break;
        default:
            if (name.hasModification(ChordName::Flatted6th))
                gp_name.mySixth = Alteration::Minor;
            else if (name.hasModification(ChordName::Added6th))
                gp_name.mySixth = Alteration::Major;
            break;
    }

    // Seventh
    switch (name.getFormula())
    {
        case ChordName::Dominant7th:
        case ChordName::Minor7th:
        case ChordName::Augmented7th:
        case ChordName::Minor7thFlatted5th:
            gp_name.mySeventh = Alteration::Minor;
            break;
        case ChordName::Major7th:
        case ChordName::MinorMajor7th:
            gp_name.mySeventh = Alteration::Major;
            break;
        case ChordName::Diminished7th:
            gp_name.mySeventh = Alteration::Diminished;
            break;
        default:
            break;
    }

    // 9th
    if (name.hasModification(ChordName::Extended9th) ||
        name.hasModification(ChordName::Added9th))
    {
        gp_name.myNinth = Alteration::Perfect;
    }
    else if (name.hasModification(ChordName::Flatted9th))
        gp_name.myNinth = Alteration::Diminished;
    else if (name.hasModification(ChordName::Raised9th))
        gp_name.myNinth = Alteration::Augmented;

    // 11th
    if (name.hasModification(ChordName::Extended11th) ||
        name.hasModification(ChordName::Added11th))
    {
        gp_name.myEleventh = Alteration::Perfect;
    }
    else if (name.hasModification(ChordName::Raised11th))
        gp_name.myEleventh = Alteration::Augmented;

    // 13th
    if (name.hasModification(ChordName::Extended13th))
        gp_name.myThirteenth = Alteration::Perfect;
    else if (name.hasModification(ChordName::Flatted13th))
        gp_name.myThirteenth = Alteration::Diminished;

    return gp_name;
}

static Gp7::ChordDiagram
convertChordDiagram(const ChordDiagram &diagram)
{
    Gp7::ChordDiagram gp_diagram;
    gp_diagram.myBaseFret = diagram.getTopFret();

    gp_diagram.myFrets = diagram.getFretNumbers();
    // Strings are in the opposite order, and frets are relative to the top
    // fret.
    std::reverse(gp_diagram.myFrets.begin(), gp_diagram.myFrets.end());
    for (int &fret : gp_diagram.myFrets)
    {
        if (fret > 0)
            fret -= diagram.getTopFret();
    }

    return gp_diagram;
}

using GpChordsMap = std::unordered_map<int, Gp7::Chord>;
using ChordNameIdMap = std::unordered_map<ChordName, int>;

static GpChordsMap
convertChords(const Score &score, ChordNameIdMap &chord_name_ids)
{
    GpChordsMap chords;

    // Add all chord diagrams. Note that their chord names may not be unique
    // (same chord but different fingering).
    for (const ChordDiagram &diagram : score.getChordDiagrams())
    {
        const int chord_id = chords.size();
        chord_name_ids[diagram.getChordName()] = chord_id;

        Gp7::Chord gp_chord;
        gp_chord.myDescription = Util::toString(diagram.getChordName());
        gp_chord.myName = convertChordName(diagram.getChordName());
        gp_chord.myDiagram = convertChordDiagram(diagram);
        chords.emplace(chord_id, gp_chord);
    }

    // Add all unique chord text items.
    for (const System &system : score.getSystems())
    {
        for (const ChordText &text : system.getChords())
        {
            const ChordName &name = text.getChordName();

            if (chord_name_ids.find(name) != chord_name_ids.end())
                continue;

            const int chord_id = chords.size();
            chord_name_ids[name] = chord_id;

            Gp7::Chord gp_chord;
            gp_chord.myDescription = Util::toString(name);
            gp_chord.myName = convertChordName(name);
            chords.emplace(chord_id, gp_chord);
        }
    }

    return chords;
}

using PlayerInstrumentMap = std::unordered_map<const Player *, std::set<int>>;

/// Find all of the instruments assigned to each player in the score.
static PlayerInstrumentMap
findPlayerInstruments(const Score &score)
{
    PlayerInstrumentMap map;

    for (const System &system : score.getSystems())
    {
        for (const PlayerChange &change : system.getPlayerChanges())
        {
            for (size_t staff = 0; staff < system.getStaves().size(); ++staff)
            {
                for (const ActivePlayer &active :
                     change.getActivePlayers(staff))
                {
                    const Player *player =
                        &score.getPlayers()[active.getPlayerNumber()];
                    map[player].insert(active.getInstrumentNumber());
                }
            }
        }
    }

    return map;
}

static std::vector<Gp7::Track>
convertTracks(const Score &score, ChordNameIdMap &chord_name_ids)
{
    std::vector<Gp7::Track> tracks;

    GpChordsMap chords = convertChords(score, chord_name_ids);

    PlayerInstrumentMap player_instruments = findPlayerInstruments(score);
    int player_idx = 0;
    for (const Player &player : score.getPlayers())
    {
        // Skip unused players. GP requires a sound for each track.
        const std::set<int> &instruments = player_instruments[&player];
        if (instruments.empty())
            continue;

        Gp7::Track track;
        track.myName = player.getDescription();

        // Include only instruments that this player uses.
        for (int instrument_idx : instruments)
        {
            const Instrument &inst = score.getInstruments()[instrument_idx];
            Gp7::Sound sound;
            sound.myLabel = inst.getDescription();
            sound.myName = "Instrument_" + std::to_string(instrument_idx);
            sound.myPath = "Midi/" + sound.myName;
            sound.myMidiPreset = inst.getMidiPreset();

            track.mySounds.push_back(sound);
        }

        const Tuning &tuning = player.getTuning();
        const std::vector<uint8_t> &notes = tuning.getNotes();

        Gp7::Staff staff;
        staff.myCapo = tuning.getCapo();
        staff.myTuning.assign(notes.rbegin(), notes.rend());
        track.myStaves.push_back(staff);

        // Chords are the same for all players. In the future we could trim
        // this based on which chord names are used while the player is active,
        // but the chord diagrams apply to all players anyways
        track.myChords = chords;

        // TODO - export player changes as sound automations
        // For now we just assign an initial instrument
        track.mySoundChanges.push_back({ 0, 0, 0 });

        track.myMidiChannel = Midi::getPlayerChannel(player_idx);

        tracks.push_back(std::move(track));
        ++player_idx;
    }

    return tracks;
}

static Gp7::Rhythm
convertRhythm(const Voice &voice, const Position &pos)
{
    Gp7::Rhythm rhythm;

    rhythm.myDuration = static_cast<int>(pos.getDurationType());
    if (pos.hasProperty(Position::Dotted))
        rhythm.myDots = 1;
    else if (pos.hasProperty(Position::DoubleDotted))
        rhythm.myDots = 2;

    // TODO - combine nested tuplets?
    for (const IrregularGrouping *group : VoiceUtils::getIrregularGroupsInRange(
             voice, pos.getPosition(), pos.getPosition()))
    {
        rhythm.myTupletNum = group->getNotesPlayed();
        rhythm.myTupletDenom = group->getNotesPlayedOver();
    }

    return rhythm;
}

static Gp7::Note::Pitch
convertPitch(const Tuning &tuning, const KeySignature &key, const Note &note,
             bool transpose)
{
    Gp7::Note::Pitch pitch;

    const uint8_t midi_note =
        tuning.getNote(note.getString(), transpose) + note.getFretNumber();

    std::string text = Midi::getMidiNoteText(
        midi_note, key.getKeyType() == KeySignature::Minor, key.usesSharps(),
        key.getNumAccidentals(), false);

    pitch.myNote = text[0];
    pitch.myAccidental = text.substr(1);
    pitch.myOctave = Midi::getMidiNoteOctave(midi_note, pitch.myNote) + 1;

    return pitch;
}

/// Convert from PT bend values (quarter steps) to GP bend values
/// (percentage of full steps)
static double
convertQuarterStepToPitch(int steps)
{
    return steps * 25.0;
}

static Gp7::Bend
convertBend(const Bend &bend)
{
    // TODO - handle bends stretching over multiple notes

    const double bent_pitch = convertQuarterStepToPitch(bend.getBentPitch());
    const double release_pitch = convertQuarterStepToPitch(bend.getReleasePitch());

    Gp7::Bend gp_bend;
    gp_bend.myDestOffset = 100;
    double middle_offset = 50;
    double middle_value = 0;
    double start_value = 0;
    double end_value = 0;

    switch (bend.getType())
    {
        case Bend::PreBendAndRelease:
            start_value = bent_pitch;
            middle_value = 0.5 * bent_pitch;
            end_value = 0;
            break;

        case Bend::PreBend:
        case Bend::PreBendAndHold:
            // Same behaviour, the only different for GP is if ties are present
            start_value = middle_value = end_value = bent_pitch;
            break;

        case Bend::NormalBend:
        case Bend::BendAndHold:
            middle_offset = 10; // Short bend
            middle_value = end_value = bent_pitch;
            break;

        case Bend::BendAndRelease:
            middle_value = bent_pitch;
            end_value = release_pitch;
            break;

        // TODO - these two modes probably need to locate the pitch from the
        // previous bend to get the correct starting point.
        case Bend::GradualRelease:
            start_value = bent_pitch;
            middle_value = 0.5 * (bent_pitch + release_pitch);
            end_value = release_pitch;
            break;
        case Bend::ImmediateRelease:
            start_value = middle_value = end_value = bent_pitch;
            break;
    }

    gp_bend.myOriginValue = start_value;
    gp_bend.myMiddleOffset1 = gp_bend.myMiddleOffset2 = middle_offset;
    gp_bend.myMiddleValue = middle_value;
    gp_bend.myDestValue = end_value;

    return gp_bend;
}

static Gp7::Bend
convertTremoloBar(const TremoloBar &trem)
{
    // TODO - handle events stretching over multiple notes

    const double pitch = convertQuarterStepToPitch(trem.getPitch());

    Gp7::Bend gp_bend;
    double middle_offset = 50;
    double middle_value = 0;
    double start_value = 0;
    double end_value = 0;
    double end_offset = 100;

    static constexpr double dip_middle = 15; // Short dip length
    static constexpr double dip_end = 30;

    switch (trem.getType())
    {
        case TremoloBar::Type::Dip:
            middle_offset = dip_middle;
            end_offset = dip_end;
            middle_value = -pitch;
            break;
        case TremoloBar::Type::InvertedDip:
            middle_offset = dip_middle;
            end_offset = dip_end;
            middle_value = pitch;
            break;
        case TremoloBar::Type::DiveAndRelease:
        case TremoloBar::Type::DiveAndHold:
            end_value = -pitch;
            middle_value = 0.5 * end_value;
            break;
        case TremoloBar::Type::Release:
            start_value = middle_value = end_value = pitch;
            break;
        // TODO - these two modes probably need to locate the pitch from the
        // previous bend to get the correct starting point.
        case TremoloBar::Type::ReturnAndRelease:
        case TremoloBar::Type::ReturnAndHold:
            end_value = pitch;
            middle_value = 0.5 * end_value;
            break;
    }

    gp_bend.myOriginValue = start_value;
    gp_bend.myMiddleOffset1 = gp_bend.myMiddleOffset2 = middle_offset;
    gp_bend.myMiddleValue = middle_value;
    gp_bend.myDestValue = end_value;
    gp_bend.myDestOffset = end_offset;

    return gp_bend;
}

static Gp7::Note::FingerType
convertFingering(LeftHandFingering::Finger f)
{
    using Finger = LeftHandFingering::Finger;
    using GpFingerType = Gp7::Note::FingerType;
    switch (f)
    {
        case Finger::None:
            return GpFingerType::Open;
        case Finger::Index:
            return GpFingerType::I;
        case Finger::Middle:
            return GpFingerType::M;
        case Finger::Ring:
            return GpFingerType::A;
        case Finger::Little:
            return GpFingerType::C;
        case Finger::Thumb:
            return GpFingerType::P;
    }

    return GpFingerType::Open;
}

static void
convertBeat(Gp7::Document &doc, Gp7::MasterBar &master_bar, Gp7::Beat &beat,
            const ChordNameIdMap &chord_name_ids, const System &system,
            const Voice &voice, const Voice *next_voice, const Tuning &tuning,
            const KeySignature &key, const boost::rational<int> &bar_time,
            const Position &pos)
{
    beat.myGraceNote = pos.hasProperty(Position::Acciaccatura);
    beat.myTremoloPicking = pos.hasProperty(Position::TremoloPicking);

    beat.myBrushDown = pos.hasProperty(Position::PickStrokeDown);
    beat.myBrushUp = pos.hasProperty(Position::PickStrokeUp);
    beat.myArpeggioUp = pos.hasProperty(Position::ArpeggioUp);
    beat.myArpeggioDown = pos.hasProperty(Position::ArpeggioDown);

    if (pos.hasProperty(Position::Fermata))
        master_bar.myFermatas.insert(bar_time);

    if (pos.hasTremoloBar())
        beat.myWhammy = convertTremoloBar(pos.getTremoloBar());

    if (const TextItem *text = ScoreUtils::findByPosition(system.getTextItems(),
                                                          pos.getPosition()))
    {
        beat.myFreeText = text->getContents();
    }

    if (const ChordText *text =
            ScoreUtils::findByPosition(system.getChords(), pos.getPosition()))
    {
        beat.myChordId = chord_name_ids.at(text->getChordName());
    }

    for (const Note &note: pos.getNotes())
    {
        Gp7::Note gp_note;

        // String numbers are flipped around
        gp_note.myString = tuning.getStringCount() - note.getString() - 1;
        gp_note.myFret = note.getFretNumber();

        // GP needs the actual pitch or else the note is ignored..
        gp_note.myConcertPitch = convertPitch(tuning, key, note, false);
        gp_note.myTransposedPitch = convertPitch(tuning, key, note, true);
        gp_note.myMidiPitch =
            tuning.getNote(note.getString(), false) + note.getFretNumber();

        // Note / position properties
        gp_note.myPalmMuted = pos.hasProperty(Position::PalmMuting);
        gp_note.myTapped = pos.hasProperty(Position::Tap);
        gp_note.myHammerOn = note.hasProperty(Note::HammerOnOrPullOff);
        gp_note.myLeftHandTapped = note.hasProperty(Note::HammerOnFromNowhere);
        gp_note.myMuted = note.hasProperty(Note::Muted);
        gp_note.myGhost = note.hasProperty(Note::GhostNote);
        gp_note.myVibrato = pos.hasProperty(Position::Vibrato);
        gp_note.myWideVibrato = pos.hasProperty(Position::WideVibrato);
        gp_note.myLetRing = pos.hasProperty(Position::LetRing);

        // The GP trill is the midi note value, not the fret number!
        if (note.hasTrill())
        {
            gp_note.myTrillNote =
                tuning.getNote(note.getString(), false) + note.getTrilledFret();
        }

        // Ties
        gp_note.myTieDest = note.hasProperty(Note::Tied);
        if (auto next_note = VoiceUtils::getNextNote(
                voice, pos.getPosition(), note.getString(), next_voice))
        {
            gp_note.myTieOrigin = next_note->hasProperty(Note::Tied);
        }

        // Accents
        using GpAccentType = Gp7::Note::AccentType;
        gp_note.myAccentTypes.set(int(GpAccentType::Staccato),
                                  pos.hasProperty(Position::Staccato));
        gp_note.myAccentTypes.set(int(GpAccentType::HeavyAccent),
                                  pos.hasProperty(Position::Sforzando));
        gp_note.myAccentTypes.set(int(GpAccentType::Accent),
                                  pos.hasProperty(Position::Marcato));

        // Slides
        using GpSlideType = Gp7::Note::SlideType;
        gp_note.mySlideTypes.set(int(GpSlideType::Shift),
                                 note.hasProperty(Note::ShiftSlide));
        gp_note.mySlideTypes.set(int(GpSlideType::Legato),
                                 note.hasProperty(Note::LegatoSlide));
        gp_note.mySlideTypes.set(int(GpSlideType::SlideOutDown),
                                 note.hasProperty(Note::SlideOutOfDownwards));
        gp_note.mySlideTypes.set(int(GpSlideType::SlideOutUp),
                                 note.hasProperty(Note::SlideOutOfUpwards));
        gp_note.mySlideTypes.set(int(GpSlideType::SlideInAbove),
                                 note.hasProperty(Note::SlideIntoFromAbove));
        gp_note.mySlideTypes.set(int(GpSlideType::SlideInBelow),
                                 note.hasProperty(Note::SlideIntoFromBelow));

        // Octaves are stored on the beat in GP
        if (note.hasProperty(Note::Octave8va))
            beat.myOttavia = Gp7::Beat::Ottavia::O8va;
        else if (note.hasProperty(Note::Octave8vb))
            beat.myOttavia = Gp7::Beat::Ottavia::O8vb;
        else if (note.hasProperty(Note::Octave15ma))
            beat.myOttavia = Gp7::Beat::Ottavia::O15ma;
        else if (note.hasProperty(Note::Octave15mb))
            beat.myOttavia = Gp7::Beat::Ottavia::O15mb;

        if (note.hasBend())
            gp_note.myBend = convertBend(note.getBend());

        if (note.hasLeftHandFingering())
        {
            gp_note.myLeftFinger =
                convertFingering(note.getLeftHandFingering().getFinger());
        }

        using HarmonicType = Gp7::Note::HarmonicType;
        if (note.hasProperty(Note::NaturalHarmonic))
        {
            gp_note.myHarmonic = HarmonicType::Natural;
        }
        else if (note.hasTappedHarmonic())
        {
            gp_note.myHarmonic = HarmonicType::Tap;
            gp_note.myHarmonicFret =
                note.getTappedHarmonicFret() - note.getFretNumber();
        }
        else if (note.hasArtificialHarmonic())
        {
            gp_note.myHarmonic = HarmonicType::Artificial;

            const int base_pitch = tuning.getNote(note.getString(), false) +
                                   tuning.getCapo() + note.getFretNumber();
            const int pitch = Harmonics::getArtificialHarmonicPitch(
                base_pitch, note.getArtificialHarmonic());
            const int pitch_offset = pitch - base_pitch;

            gp_note.myHarmonicFret = Harmonics::getFretOffset(pitch_offset);

            // Fix up a couple issues from GP storing more precise fret offsets.
            if (gp_note.myHarmonicFret == 3)
                gp_note.myHarmonicFret = 3.2;
            else if (gp_note.myHarmonicFret == 15)
                gp_note.myHarmonicFret = 14.7;
        }

        const int note_id = doc.myNotes.size();
        doc.myNotes.emplace(note_id, gp_note);
        beat.myNoteIds.push_back(note_id);
    }
}

static void
convertBar(Gp7::Document &doc, const ChordNameIdMap &chord_name_ids,
           std::unordered_map<Gp7::Rhythm, int> &unique_rhythms,
           Gp7::MasterBar &master_bar, Gp7::Bar &bar,
           ConstScoreLocation &location, const Tuning &tuning,
           const KeySignature &key, int start_idx, int end_idx)
{
    const Staff &staff = location.getStaff();

    switch (staff.getClefType())
    {
        case Staff::TrebleClef:
            bar.myClefType = Gp7::Bar::ClefType::G2;
            break;
        case Staff::BassClef:
            bar.myClefType = Gp7::Bar::ClefType::F4;
            break;
    }

    location.setVoiceIndex(0);
    for (const Voice &voice : staff.getVoices())
    {
        const Voice *next_voice = VoiceUtils::getAdjacentVoice(location, 1);

        Gp7::Voice gp_voice;

        boost::rational<int> bar_time;
        for (const Position &pos :
             ScoreUtils::findInRange(voice.getPositions(), start_idx, end_idx))
        {
            Gp7::Beat beat;
            convertBeat(doc, master_bar, beat, chord_name_ids,
                        location.getSystem(), voice, next_voice, tuning, key,
                        bar_time, pos);
            bar_time += VoiceUtils::getDurationTime(voice, pos);

            Gp7::Rhythm rhythm = convertRhythm(voice, pos);
            // Consolidate identical rhythms to reduce file size, which GP also
            // does.
            if (auto it = unique_rhythms.find(rhythm);
                it != unique_rhythms.end())
            {
                beat.myRhythmId = it->second;
            }
            else
            {
                const int rhythm_id = doc.myRhythms.size();
                doc.myRhythms.emplace(rhythm_id, rhythm);
                beat.myRhythmId = rhythm_id;

                unique_rhythms.emplace(rhythm, rhythm_id);
            }

            const int beat_id = doc.myBeats.size();
            doc.myBeats.emplace(beat_id, beat);
            gp_voice.myBeatIds.push_back(beat_id);
        }

        const int voice_id = doc.myVoices.size();
        doc.myVoices.emplace(voice_id, gp_voice);
        bar.myVoiceIds.push_back(voice_id);

        location.setVoiceIndex(location.getVoiceIndex() + 1);
    }
}

static void
convertDirection(const Direction &dir, Gp7::MasterBar &master_bar)
{
    using DirectionTarget = Gp7::MasterBar::DirectionTarget;
    using DirectionJump = Gp7::MasterBar::DirectionJump;
    std::vector<DirectionTarget> &targets = master_bar.myDirectionTargets;
    std::vector<DirectionJump> &jumps = master_bar.myDirectionJumps;

    for (const DirectionSymbol &symbol : dir.getSymbols())
    {
        switch (symbol.getSymbolType())
        {
            case DirectionSymbol::Coda:
                targets.push_back(DirectionTarget::Coda);
                break;
            case DirectionSymbol::DoubleCoda:
                targets.push_back(DirectionTarget::DoubleCoda);
                break;
            case DirectionSymbol::Segno:
                targets.push_back(DirectionTarget::Segno);
                break;
            case DirectionSymbol::SegnoSegno:
                targets.push_back(DirectionTarget::SegnoSegno);
                break;
            case DirectionSymbol::Fine:
                targets.push_back(DirectionTarget::Fine);
                break;
            case DirectionSymbol::DaCapo:
                jumps.push_back(DirectionJump::DaCapo);
                break;
            case DirectionSymbol::DalSegno:
                jumps.push_back(DirectionJump::DaSegno);
                break;
            case DirectionSymbol::DalSegnoSegno:
                jumps.push_back(DirectionJump::DaSegnoSegno);
                break;
            case DirectionSymbol::ToCoda:
                jumps.push_back(DirectionJump::DaCoda);
                break;
            case DirectionSymbol::ToDoubleCoda:
                jumps.push_back(DirectionJump::DaDoubleCoda);
                break;
            case DirectionSymbol::DaCapoAlCoda:
                jumps.push_back(DirectionJump::DaCapoAlCoda);
                break;
            case DirectionSymbol::DaCapoAlDoubleCoda:
                jumps.push_back(DirectionJump::DaCapoAlDoubleCoda);
                break;
            case DirectionSymbol::DalSegnoAlCoda:
                jumps.push_back(DirectionJump::DaSegnoAlCoda);
                break;
            case DirectionSymbol::DalSegnoAlDoubleCoda:
                jumps.push_back(DirectionJump::DaSegnoAlDoubleCoda);
                break;
            case DirectionSymbol::DalSegnoSegnoAlCoda:
                jumps.push_back(DirectionJump::DaSegnoSegnoAlCoda);
                break;
            case DirectionSymbol::DalSegnoSegnoAlDoubleCoda:
                jumps.push_back(DirectionJump::DaSegnoSegnoAlDoubleCoda);
                break;
            case DirectionSymbol::DaCapoAlFine:
                jumps.push_back(DirectionJump::DaCapoAlFine);
                break;
            case DirectionSymbol::DalSegnoAlFine:
                jumps.push_back(DirectionJump::DaSegnoAlFine);
                break;
            case DirectionSymbol::DalSegnoSegnoAlFine:
                jumps.push_back(DirectionJump::DaSegnoSegnoAlFine);
                break;
            case DirectionSymbol::NumSymbolTypes:
                // Do nothing
                break;
        }
    }
}

/// Create a GP master bar, from a pair of barlines in the score.
static Gp7::MasterBar
convertMasterBar(const Gp7::Document &doc, const System &system,
                 const Barline &start_line, const Barline &end_line)
{
    Gp7::MasterBar master_bar;

    if (start_line.hasRehearsalSign())
    {
        const RehearsalSign &sign = start_line.getRehearsalSign();
        master_bar.mySection = { sign.getLetters(), sign.getDescription() };
    }

    if (end_line.getBarType() == Barline::RepeatEnd)
    {
        master_bar.myRepeatEnd = true;
        master_bar.myRepeatCount = end_line.getRepeatCount();
    }

    master_bar.myRepeatStart =
        (start_line.getBarType() == Barline::RepeatStart);
    master_bar.myDoubleBar = (end_line.getBarType() == Barline::DoubleBar);
    master_bar.myFreeTime = (end_line.getBarType() == Barline::FreeTimeBar);

    // Time signature.
    const TimeSignature &time_sig = start_line.getTimeSignature();
    master_bar.myTimeSig.myBeats = time_sig.getNumPulses();
    master_bar.myTimeSig.myBeatValue = time_sig.getBeatValue();

    // Key signature.
    const KeySignature &key_sig = start_line.getKeySignature();
    master_bar.myKeySig.myAccidentalCount = key_sig.getNumAccidentals();
    master_bar.myKeySig.myMinor = key_sig.getKeyType() == KeySignature::Minor;
    master_bar.myKeySig.mySharps = key_sig.usesSharps();

    // Alternate endings.
    for (const AlternateEnding &ending : ScoreUtils::findInRange(
             system.getAlternateEndings(), start_line.getPosition(),
             end_line.getPosition() - 1))
    {
        for (int number : ending.getNumbers())
            master_bar.myAlternateEndings.push_back(number);
    }

    // Directions
    int direction_start =
        (start_line.getPosition() == 0) ? 0 : (start_line.getPosition() + 1);
    int direction_end = end_line.getPosition();
    for (const Direction &direction : ScoreUtils::findInRange(
             system.getDirections(), direction_start, direction_end))
    {
        convertDirection(direction, master_bar);
    }

    // Tempo markers.
    for (const TempoMarker &marker : ScoreUtils::findInRange(
             system.getTempoMarkers(), start_line.getPosition(),
             end_line.getPosition() - 1))
    {
        Gp7::TempoChange change;
        change.myPosition = 0; // TODO - compute location within bar.
        change.myIsVisible = true;
        change.myDescription = marker.getDescription();

        change.myIsLinear =
            (marker.getMarkerType() == TempoMarker::AlterationOfPace);

        if (change.myIsLinear)
        {
            // Copy the previous tempo, since this starts a linear change from
            // the current tempo to whatever the next tempo is.
            for (auto it = doc.myMasterBars.rbegin();
                 it != doc.myMasterBars.rend(); ++it)
            {
                if (!it->myTempoChanges.empty())
                {
                    const Gp7::TempoChange &last = it->myTempoChanges.back();
                    change.myBeatsPerMinute = last.myBeatsPerMinute;
                    change.myBeatType = last.myBeatType;
                    break;
                }
            }
        }
        else
        {
            change.myBeatsPerMinute = marker.getBeatsPerMinute();

            switch (marker.getBeatType())
            {
                using BeatType = Gp7::TempoChange::BeatType;

                case TempoMarker::Eighth:
                    change.myBeatType = BeatType::Eighth;
                    break;
                case TempoMarker::Half:
                    change.myBeatType = BeatType::Half;
                    break;
                case TempoMarker::HalfDotted:
                    change.myBeatType = BeatType::HalfDotted;
                    break;
                case TempoMarker::Quarter:
                    change.myBeatType = BeatType::Quarter;
                    break;
                case TempoMarker::QuarterDotted:
                    change.myBeatType = BeatType::QuarterDotted;
                    break;

                default:
                    // Other types aren't supported in GP
                    break;
            }
        }

        master_bar.myTempoChanges.push_back(change);
    }

    return master_bar;
}

static void
convertScore(const Score &score, const ChordNameIdMap &chord_name_ids,
             Gp7::Document &doc)
{
    std::unordered_map<Gp7::Rhythm, int> unique_rhythms;

    ConstScoreLocation location(score);
    while (location.getSystemIndex() <
           static_cast<int>(score.getSystems().size()))
    {
        const System &system = location.getSystem();

        auto [current_bar, next_bar] = SystemUtils::getSurroundingBarlines(
            system, location.getPositionIndex());

        const PlayerChange *active_players = ScoreUtils::getCurrentPlayers(
            score, location.getSystemIndex(), location.getPositionIndex());

        // TODO - skip "empty" bars due to adjacent repeat end / start
        Gp7::MasterBar master_bar =
            convertMasterBar(doc, system, current_bar, next_bar);
        master_bar.myBarIds.assign(doc.myTracks.size(), -1);

        location.setStaffIndex(0);
        for (const Staff &staff : system.getStaves())
        {
            if (!active_players ||
                active_players->getActivePlayers(location.getStaffIndex()).empty())
            {
                continue;
            }

            int player_idx = active_players->getActivePlayers(location.getStaffIndex())[0].getPlayerNumber();
            const Tuning &tuning = score.getPlayers()[player_idx].getTuning();
            if (tuning.getStringCount() != staff.getStringCount())
                continue; // Something is incorrect in the file...

            Gp7::Bar bar;
            convertBar(doc, chord_name_ids, unique_rhythms, master_bar, bar,
                       location, tuning, current_bar.getKeySignature(),
                       current_bar.getPosition(), next_bar.getPosition());

            const int bar_id = doc.myBars.size();
            doc.myBars.emplace(bar_id, bar);

            // Assign the bar to each player (track) in this staff.
            for (const ActivePlayer &player :
                 active_players->getActivePlayers(location.getStaffIndex()))
            {
                master_bar.myBarIds[player.getPlayerNumber()] = bar_id;
            }

            location.setStaffIndex(location.getStaffIndex() + 1);
        }

        doc.myMasterBars.emplace_back(std::move(master_bar));

        // Move to next bar / system.
        if (&next_bar == &system.getBarlines().back())
        {
            location.setSystemIndex(location.getSystemIndex() + 1);
            location.setPositionIndex(0);
        }
        else
            location.setPositionIndex(next_bar.getPosition());
    }
}

Gp7::Document
Gp7::convert(const Score &score)
{
    Gp7::Document gp_doc;
    gp_doc.myScoreInfo = convertScoreInfo(score);

    ChordNameIdMap chord_name_ids;
    gp_doc.myTracks = convertTracks(score, chord_name_ids);
    convertScore(score, chord_name_ids, gp_doc);

    return gp_doc;
}
