/*
  * Copyright (C) 2014 Cameron White
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

#include "scoremerger.h"

#include <actions/insertnotes.h>
#include <score/score.h>
#include <score/utils.h>
#include <score/voiceutils.h>

/// Approximate upper limit on the number of positions in a system.
static const int POSITION_LIMIT = 30;

ScoreMerger::ScoreMerger(Score &dest, Score &guitarScore, Score &bassScore)
    : myDestScore(dest),
      myDestCaret(dest),
      myDestLoc(myDestCaret.getLocation()),
      myGuitarScore(guitarScore),
      myBassScore(bassScore),
      myGuitarState(guitarScore, false),
      myBassState(bassScore, true),
      myNumGuitarStaves(0)
{
}

void ScoreMerger::mergePlayers()
{
    for (const Player &player : myGuitarScore.getPlayers())
        myDestScore.insertPlayer(player);
    for (const Player &player : myBassScore.getPlayers())
        myDestScore.insertPlayer(player);

    for (const Instrument &instrument : myGuitarScore.getInstruments())
        myDestScore.insertInstrument(instrument);
    for (const Instrument &instrument : myBassScore.getInstruments())
        myDestScore.insertInstrument(instrument);
}

static int insertWholeRest(ScoreLocation &dest, ScoreLocation &)
{
    Position wholeRest(dest.getPositionIndex(), Position::WholeNote);
    wholeRest.setRest();
    dest.getVoice().insertPosition(wholeRest);

    // A whole rest should probably span at least a few positions.
    return 8;
}

static int insertMultiBarRest(ScoreLocation &dest, ScoreLocation &, int count)
{
    Position rest(dest.getPositionIndex(), Position::WholeNote);
    rest.setRest();
    rest.setMultiBarRest(count);
    dest.getVoice().insertPosition(rest);

    // A multi-bar rest should probably span at least a few positions.
    return 16;
}

/// Copy notes from the source bar to the destination.
static int copyNotes(ScoreLocation &dest, ScoreLocation &src)
{
    const System &srcSystem = src.getSystem();
    const Barline *srcBar = src.getBarline();
    assert(srcBar);
    const Barline *nextSrcBar = srcSystem.getNextBarline(srcBar->getPosition());
    assert(nextSrcBar);

    auto positions = ScoreUtils::findInRange(src.getVoice().getPositions(),
                                             srcBar->getPosition(),
                                             nextSrcBar->getPosition());
    std::vector<IrregularGrouping> groups;
    for (const IrregularGrouping *group : VoiceUtils::getIrregularGroupsInRange(
             src.getVoice(), srcBar->getPosition(), nextSrcBar->getPosition()))
    {
        groups.push_back(*group);
    }

    if (!positions.empty())
    {
        int length = positions.back().getPosition() - srcBar->getPosition();
        if (srcBar->getPosition() == 0)
            ++length;

        InsertNotes action(
            dest, std::vector<Position>(positions.begin(), positions.end()),
            groups);
        action.redo();

        return length;
    }
    else
        return 0;
}

int ScoreMerger::importNotes(
    ScoreLocation &dest, ScoreLocation &srcLoc, bool bass,
    std::function<int(ScoreLocation &, ScoreLocation &)> action)
{
    System &destSystem = dest.getSystem();
    const System &srcSystem = srcLoc.getSystem();

    const int staffOffset = bass ? myNumGuitarStaves : 0;
    int length = 0;

    // Merge the notes for each staff.
    for (int i = 0; i < srcSystem.getStaves().size(); ++i)
    {
        // Ensure that there are enough staves in the destination system.
        if ((!bass && myNumGuitarStaves <= i) ||
            destSystem.getStaves().size() <= i + staffOffset)
        {
            const Staff &srcStaff = srcSystem.getStaves()[i];
            Staff destStaff(srcStaff.getStringCount());
            destStaff.setClefType(srcStaff.getClefType());
            destStaff.setViewType(bass ? Staff::BassView : Staff::GuitarView);
            destSystem.insertStaff(destStaff);

            if (!bass)
                ++myNumGuitarStaves;
        }

        myDestLoc.setStaffIndex(i + staffOffset);
        srcLoc.setStaffIndex(i);

        // Import each voice.
        for (int v = 0; v < Staff::NUM_VOICES; ++v)
        {
            myDestLoc.setVoiceIndex(v);
            srcLoc.setVoiceIndex(v);

            length = std::max(length, action(myDestLoc, srcLoc));
        }
    }

    return length;
}

void ScoreMerger::merge()
{
    myDestScore.insertSystem(System());

    while (true)
    {
        System &destSystem = myDestLoc.getSystem();
        const Barline *destBar = myDestLoc.getBarline();
        assert(destBar);

        // Insert the notes at the first position after the barline.
        if (myDestLoc.getPositionIndex() != 0)
            myDestCaret.moveHorizontal(1);

        // We only need special handling for multi-bar rests if both staves are active.
        if (!myGuitarState.done && !myBassState.done)
        {
            myGuitarState.checkForMultibarRest();
            myBassState.checkForMultibarRest();
        }

        int barLength = 0;

        if (myGuitarState.inMultibarRest && myBassState.inMultibarRest)
        {
            // If both scores are in a multi-bar rest, insert a multi-bar rest
            // for the shorter duration of the two.
            const int count = std::min(myGuitarState.multibarRestCount,
                                       myBassState.multibarRestCount);

            auto action = std::bind(insertMultiBarRest, std::placeholders::_1,
                                    std::placeholders::_2, count);
            barLength = importNotes(myDestLoc, myGuitarState.loc, false, action);
            barLength = std::max(barLength,
                                 importNotes(myDestLoc, myBassState.loc, true, action));

            myGuitarState.multibarRestCount -= count;
            myBassState.multibarRestCount -= count;
        }
        else
        {
            for (State *state : {&myGuitarState, &myBassState})
            {
                if (state->done)
                    continue;

                int length = 0;
                // If one state is a multibar rest, but the other is not, keep
                // inserting whole rests. If we've reached the end of a score,
                // keep inserting whole rests until we move onto the next
                // system in the destination score.
                if (state->inMultibarRest || state->finishing)
                {
                    length = importNotes(myDestLoc, state->loc, state->isBass,
                                         insertWholeRest);

                    if (state->inMultibarRest)
                        --state->multibarRestCount;
                }
                else
                {
                    // TODO - this also should copy time signatures, dynamics, etc.
                    length = importNotes(myDestLoc, state->loc, state->isBass,
                                         copyNotes);
                }

                barLength = std::max(barLength, length);
            }
        }

        myGuitarState.advance();
        myBassState.advance();

        const int nextBarPos = destBar->getPosition() + barLength + 1;

        // If we're about to move to a new system, transition from finishing to
        // done.
        if (nextBarPos > POSITION_LIMIT)
        {
            myGuitarState.finishIfPossible();
            myBassState.finishIfPossible();
        }

        if ((myGuitarState.done || myGuitarState.finishing) &&
            (myBassState.done || myBassState.finishing))
        {
            break;
        }

        // Create the next bar or move to the next system.
        if (nextBarPos > POSITION_LIMIT)
        {
            destSystem.getBarlines().back().setPosition(nextBarPos);
            myDestScore.insertSystem(System());
            myNumGuitarStaves = 0;
            myDestCaret.moveSystem(1);
        }
        else
        {
            // TODO - correctly copy over barline, key signatures, etc.
            Barline barline(nextBarPos, Barline::FreeTimeBar);
            barline.setPosition(nextBarPos);
            destSystem.insertBarline(barline);
            myDestCaret.moveToNextBar();
            destSystem.getBarlines().back().setPosition(nextBarPos + 10);
        }
    }
}

ScoreMerger::State::State(Score &score, bool isBass)
    : caret(score),
      loc(caret.getLocation()),
      isBass(isBass),
      inMultibarRest(false),
      multibarRestCount(0),
      done(false),
      finishing(false)
{
}

void ScoreMerger::State::advance()
{
    if (inMultibarRest && multibarRestCount == 0)
        inMultibarRest = false;

    if (!inMultibarRest && !caret.moveToNextBar())
        finishing = true;
}

void ScoreMerger::State::finishIfPossible()
{
    if (finishing)
    {
        finishing = false;
        done = true;
    }
}

void ScoreMerger::State::checkForMultibarRest()
{
    if (inMultibarRest)
        return;

    const System &system = loc.getSystem();
    const Barline *bar = loc.getBarline();
    const Barline *nextBar = system.getNextBarline(bar->getPosition());

    for (const Staff &staff : system.getStaves())
    {
        for (const Voice &voice : staff.getVoices())
        {
            for (const Position &pos : ScoreUtils::findInRange(
                     voice.getPositions(), bar->getPosition(),
                     nextBar->getPosition()))
            {
                if (pos.hasMultiBarRest())
                {
                    inMultibarRest = true;
                    multibarRestCount = pos.getMultiBarRestCount();
                    return;
                }
            }
        }
    }
}
