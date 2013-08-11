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

#include "stdnotationnote.h"

#include <boost/assign/list_of.hpp>
#include <boost/foreach.hpp>
#include <boost/unordered_map.hpp>
#include <painters/layoutinfo.h>
#include <painters/musicfont.h>
#include <score/generalmidi.h>
#include <score/score.h>
#include <score/tuning.h>

/// Maps notes to their position on the staff (relative to the top line),
/// using units of 0.5 * STD_NOTATION_LINE_SPACING.
/// This is for treble clef, but can easily be adjusted for bass clef.
static const boost::unordered_map<char, int> theNotePositions =
        boost::assign::map_list_of('F', 0) ('E', 1) ('D', 2) ('C', 3) ('B', -3)
                                  ('A', -2) ('G', -1);

StdNotationNote::StdNotationNote(const Position &pos, const Note &note,
                                 double y)
    : myY(y),
      myPosition(pos.getPosition())
{
    // Choose the note head symbol.
    switch (pos.getDurationType())
    {
    case Position::WholeNote:
        myNoteHeadSymbol = MusicFont::WholeNote;
        break;
    case Position::HalfNote:
        myNoteHeadSymbol = MusicFont::HalfNote;
        break;
    default:
        myNoteHeadSymbol = MusicFont::QuarterNoteOrLess;
        break;
    }

    if (note.hasProperty(Note::NaturalHarmonic))
        myNoteHeadSymbol = MusicFont::NaturalHarmonicNoteHead;
    // TODO - handle artificial harmonics.
    else if (note.hasTappedHarmonic())
        myNoteHeadSymbol = MusicFont::ArtificialHarmonicNoteHead;
    else if (note.hasProperty(Note::Muted))
        myNoteHeadSymbol = MusicFont::MutedNoteHead;
}

std::vector<StdNotationNote> StdNotationNote::getNotesInStaff(
        const Score &score, const System &system, int systemIndex,
        const Staff &staff, int staffIndex)
{
    std::vector<StdNotationNote> notes;

    // If there is no active player, use standard 8-string tuning as a default
    // for calculating the music notation.
    Tuning fallbackTuning;
    std::vector<uint8_t> tuningNotes = fallbackTuning.getNotes();
    tuningNotes.push_back(Midi::MIDI_NOTE_B2);
    tuningNotes.push_back(Midi::MIDI_NOTE_E1);
    fallbackTuning.setNotes(tuningNotes);

    for (int voice = 0; voice < Staff::NUM_VOICES; ++voice)
    {
        BOOST_FOREACH(const Barline &bar, system.getBarlines())
        {
            const Barline *nextBar = system.getNextBarline(bar.getPosition());
            if (!nextBar)
                break;

            BOOST_FOREACH(const Position &pos, staff.getPositionsInRange(
                              voice, bar.getPosition(), nextBar->getPosition()))
            {
                if (pos.isRest() || pos.hasMultiBarRest())
                    continue;

                // Find an active player so that we know what tuning to use.
                std::vector<ActivePlayer> activePlayers;
                const PlayerChange *players = ScoreUtils::getCurrentPlayers(
                            score, systemIndex, pos.getPosition());
                if (players)
                    activePlayers = players->getActivePlayers(staffIndex);

                const Player *player = 0;
                if (!activePlayers.empty())
                {
                    player = &score.getPlayers()[
                            activePlayers.front().getPlayerNumber()];
                }

                BOOST_FOREACH(const Note &note, pos.getNotes())
                {
                    const double y = getNoteLocation(
                                staff, note, bar.getKeySignature(),
                                player ? player->getTuning() : fallbackTuning);

                    notes.push_back(StdNotationNote(pos, note, y));
                }
            }
        }
    }

    return notes;
}

double StdNotationNote::getY() const
{
    return myY;
}

double StdNotationNote::getNoteLocation(const Staff &staff, const Note &note,
                                        const KeySignature &key,
                                        const Tuning &tuning)
{
    const int pitch = tuning.getNote(note.getString(), true) + note.getFretNumber();

    const std::string text = Midi::getMidiNoteText(
                pitch, key.getKeyType() == KeySignature::Minor,
                key.usesSharps() || key.getNumAccidentals() == 0,
                key.getNumAccidentals());

    // Find the offset of the note - accidentals don't matter, so C# and C
    // have the same offset, for example.
    int y = theNotePositions.find(text[0])->second;

    // Adjust for bass clef, where A is the note at the top line.
    if (staff.getClefType() == Staff::BassClef)
        y += 2;

    const int topNote = (staff.getClefType() == Staff::TrebleClef) ?
                Midi::MIDI_NOTE_F4 : Midi::MIDI_NOTE_A2;

    // Shift the note by the number of octaves.
    y += 7 * (Midi::getMidiNoteOctave(topNote) -
              Midi::getMidiNoteOctave(pitch, text[0])) +
            7 * getOctaveOffset(note);

    return y * 0.5 * LayoutInfo::STD_NOTATION_LINE_SPACING;
}

int StdNotationNote::getOctaveOffset(const Note &note)
{
    if (note.hasProperty(Note::Octave8va))
        return 1;
    else if (note.hasProperty(Note::Octave15ma))
        return 2;
    else if (note.hasProperty(Note::Octave8vb))
        return -1;
    else if (note.hasProperty(Note::Octave15mb))
        return -2;
    else
        return 0;
}

QChar StdNotationNote::getNoteHeadSymbol() const
{
    return myNoteHeadSymbol;
}

int StdNotationNote::getPosition() const
{
    return myPosition;
}
