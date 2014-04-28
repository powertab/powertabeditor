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
      myNumGuitarStaves(0),
      myPrevNumGuitarStaves(0)
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

static void hideSignaturesAndRehearsalSign(Barline &bar)
{
    bar.clearRehearsalSign();

    KeySignature key = bar.getKeySignature();
    key.setVisible(false);
    bar.setKeySignature(key);

    TimeSignature time = bar.getTimeSignature();
    time.setVisible(false);
    bar.setTimeSignature(time);
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

static void getPositionRange(const ScoreLocation &dest, const ScoreLocation &src,
        int &offset, int &left, int &right)
{
    const System &srcSystem = src.getSystem();
    const Barline *srcBar = src.getBarline();
    assert(srcBar);
    const Barline *nextSrcBar = srcSystem.getNextBarline(srcBar->getPosition());
    assert(nextSrcBar);

    offset = dest.getPositionIndex() - srcBar->getPosition();
    if (srcBar->getPosition() != 0)
        --offset;

    left = srcBar->getPosition();
    right = nextSrcBar->getPosition();
}

/// Copy notes from the source bar to the destination.
static int copyNotes(ScoreLocation &dest, ScoreLocation &src)
{
    int offset, left, right;
    getPositionRange(dest, src, offset, left, right);

    auto positions = ScoreUtils::findInRange(src.getVoice().getPositions(),
                                             left, right);

    if (!positions.empty())
    {
        for (const Position &pos : positions)
        {
            Position newPos(pos);
            newPos.setPosition(newPos.getPosition() + offset);
            dest.getVoice().insertPosition(newPos);
        }

        for (const IrregularGrouping *group :
             VoiceUtils::getIrregularGroupsInRange(src.getVoice(), left, right))
        {
            IrregularGrouping newGroup(*group);
            newGroup.setPosition(newGroup.getPosition() + offset);
            dest.getVoice().insertIrregularGrouping(newGroup);
        }

        int length = right - left;
        if (left == 0)
            ++length;

        return length;
    }
    else
        return 0;
}

int ScoreMerger::importNotes(
    ScoreLocation &dest, State &srcState,
    std::function<int(ScoreLocation &, ScoreLocation &)> action)
{
    ScoreLocation &srcLoc = srcState.loc;

    System &destSystem = dest.getSystem();
    const System &srcSystem = srcLoc.getSystem();

    int offset, left, right;
    getPositionRange(dest, srcLoc, offset, left, right);

    const int staffOffset = srcState.isBass ? myNumGuitarStaves : 0;
    int length = 0;

    // Merge the notes for each staff.
    for (int i = 0; i < srcSystem.getStaves().size(); ++i)
    {
        // Ensure that there are enough staves in the destination system.
        if ((!srcState.isBass && myNumGuitarStaves <= i) ||
            destSystem.getStaves().size() <= i + staffOffset)
        {
            const Staff &srcStaff = srcSystem.getStaves()[i];
            Staff destStaff(srcStaff.getStringCount());
            destStaff.setClefType(srcStaff.getClefType());
            destStaff.setViewType(srcState.isBass ? Staff::BassView : Staff::GuitarView);
            destSystem.insertStaff(destStaff);

            if (!srcState.isBass)
                ++myNumGuitarStaves;
        }

        myDestLoc.setStaffIndex(i + staffOffset);
        srcLoc.setStaffIndex(i);

        // Import dynamics, but don't repeatedly do so when expanding a
        // multi-bar rest.
        if (!srcState.expandingMultibarRest)
        {
            for (const Dynamic &dynamic : ScoreUtils::findInRange(
                        srcLoc.getStaff().getDynamics(), left, right - 1))
            {
                Dynamic newDynamic(dynamic);
                newDynamic.setPosition(newDynamic.getPosition() + offset);
                dest.getStaff().insertDynamic(newDynamic);
            }
        }

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

/// When expanding a repeated section, we need to replace start/end bars with
/// regular barlines.
static void removeRepeatsWhenExpanding(bool isExpanding, Barline &bar)
{
    if (isExpanding && ((bar.getBarType() == Barline::RepeatStart) ||
                        (bar.getBarType() == Barline::RepeatEnd)))
    {
        bar.setBarType(Barline::SingleBar);
    }
}

void ScoreMerger::copyBarsFromSource(Barline &destBar, Barline &nextDestBar)
{
    const bool isExpanding =
        (myGuitarState.repeatState == State::EXPANDING_REPEAT) ||
        (myBassState.repeatState == State::EXPANDING_REPEAT);

    const State *state;
    if (!myGuitarState.outOfNotes())
        state = &myGuitarState;
    else
        state = &myBassState;

    const Barline *srcBar = state->loc.getBarline();
    const System *srcSystem = &state->loc.getSystem();
    const bool isCopied = state->expandingMultibarRest;

    assert(srcBar);
    const Barline *nextSrcBar = srcSystem->getNextBarline(srcBar->getPosition());
    assert(nextSrcBar);

    // Only set the left bar's properties when we're at the start of the system,
    // or after we've detected that bars are being copied (i.e. we want to clear
    // duplicate repeat ends if a multi-bar rest appears directly before a
    // repeat end bar).
    int destPosition = destBar.getPosition();
    if (destPosition == 0 || isCopied)
    {
        destBar = *srcBar;
        removeRepeatsWhenExpanding(isExpanding, destBar);
        // The first bar cannot be the end of a repeat.
        if (destPosition == 0 && destBar.getBarType() == Barline::RepeatEnd)
            destBar.setBarType(Barline::SingleBar);
        destBar.setPosition(destPosition);
    }

    if (isCopied)
        hideSignaturesAndRehearsalSign(destBar);

    // Set the right bar's properties.
    destPosition = nextDestBar.getPosition();
    nextDestBar = *nextSrcBar;
    nextDestBar.setPosition(destPosition);
    removeRepeatsWhenExpanding(isExpanding, nextDestBar);

    if (nextDestBar.getBarType() == Barline::RepeatEnd &&
        state->repeatState == State::MERGING_REPEAT)
    {
        nextDestBar.setRepeatCount(state->numMergedRepeats + 1);
    }
}

const PlayerChange *ScoreMerger::findPlayerChange(const State &state)
{
    if (state.outOfNotes() || state.expandingMultibarRest)
        return nullptr;

    int offset, left, right;
    getPositionRange(myDestLoc, state.loc, offset, left, right);

    auto changes = ScoreUtils::findInRange(
        state.loc.getSystem().getPlayerChanges(), left, right - 1);

    return changes.empty() ? nullptr : &changes.front();
}

void ScoreMerger::mergePlayerChanges()
{
    const PlayerChange *guitarChange = findPlayerChange(myGuitarState);
    const PlayerChange *bassChange = findPlayerChange(myBassState);

    // If either the guitar or bass score has a player change, or we're at the
    // start of a new system that has a different number of guitar staves,
    // insert a player change to ensure that player are assigned to the correct
    // staves.
    if (guitarChange || bassChange ||
        (myNumGuitarStaves != myPrevNumGuitarStaves &&
         myDestLoc.getPositionIndex() == 0))
    {
        PlayerChange change;

        if (!guitarChange && !myGuitarState.done)
        {
            // If there is only a player change in the bass score, carry over
            // the current active players from the guitar score.
            guitarChange = ScoreUtils::getCurrentPlayers(
                myGuitarScore, myGuitarState.loc.getSystemIndex(),
                myGuitarState.loc.getPositionIndex());
        }

        if (!bassChange && !myBassState.done)
        {
            // If there is only a player change in the guitar score, carry over
            // the current active players from the bass score.
            bassChange = ScoreUtils::getCurrentPlayers(
                myBassScore, myBassState.loc.getSystemIndex(),
                myBassState.loc.getPositionIndex());
        }

        // Merge in data from only the active staves.
        if (guitarChange)
        {
            for (int i = 0; i < myNumGuitarStaves; ++i)
            {
                for (const ActivePlayer &player :
                     guitarChange->getActivePlayers(i))
                {
                    change.insertActivePlayer(i, player);
                }
            }
        }

        // Merge in the bass score's player change and adjust
        // staff/player/instrument numbers.
        if (bassChange)
        {
            for (int i = 0; i < myBassState.loc.getSystem().getStaves().size();
                 ++i)
            {
                for (const ActivePlayer &player :
                     bassChange->getActivePlayers(i))
                {
                    change.insertActivePlayer(
                        myNumGuitarStaves + i,
                        ActivePlayer(myGuitarScore.getPlayers().size() +
                                         player.getPlayerNumber(),
                                     myGuitarScore.getInstruments().size() +
                                         player.getInstrumentNumber()));
                }
            }
        }

        change.setPosition(myDestLoc.getPositionIndex());
        myDestLoc.getSystem().insertPlayerChange(change);
    }
}

void ScoreMerger::mergeSystemSymbols()
{
    for (State *state : {&myGuitarState, &myBassState})
    {
        if (state->outOfNotes() || state->expandingMultibarRest)
            continue;

        int offset, left, right;
        getPositionRange(myDestLoc, state->loc, offset, left, right);

        const System &srcSystem = state->loc.getSystem();
        System &destSystem = myDestLoc.getSystem();

        for (const TempoMarker &srcMarker : ScoreUtils::findInRange(
                 srcSystem.getTempoMarkers(), left, right - 1))
        {
            TempoMarker marker(srcMarker);
            marker.setPosition(srcMarker.getPosition() + offset);

            // We might get duplicate tempo markers from the two scores.
            if (ScoreUtils::findByPosition(destSystem.getTempoMarkers(),
                                           marker.getPosition()))
                continue;

            destSystem.insertTempoMarker(marker);
        }

        for (const ChordText &srcChord :
             ScoreUtils::findInRange(srcSystem.getChords(), left, right - 1))
        {
            ChordText chord(srcChord);
            chord.setPosition(srcChord.getPosition() + offset);

            // We might get duplicate items from the two scores.
            if (ScoreUtils::findByPosition(destSystem.getChords(),
                                           chord.getPosition()))
                continue;

            destSystem.insertChord(chord);
        }
    }
}

void ScoreMerger::merge()
{
    mergePlayers();
    myDestScore.insertSystem(System());

    while (true)
    {
        System &destSystem = myDestLoc.getSystem();
        Barline *destBar = myDestLoc.getBarline();
        assert(destBar);
        Barline nextDestBar;

        // We only need special handling for multi-bar rests and repeated
        // sections if both staves are active.
        if (!myGuitarState.done && !myBassState.done)
        {
            myGuitarState.checkForRepeatedSection();
            myGuitarState.checkForMultibarRest();
            myBassState.checkForMultibarRest();
            myBassState.checkForRepeatedSection();
        }

        // Decide whether to merge or expand repeated sections from either
        // score.
        myGuitarState.compareRepeatedSection(myBassState);

        // Copy a bar from one of the scores into the destination bar.
        copyBarsFromSource(*destBar, nextDestBar);

        // We will insert the notes at the first position after the barline.
        if (myDestLoc.getPositionIndex() != 0)
            myDestCaret.moveHorizontal(1);

        // TODO - this also needs to handle mismatched repeats, alternate
        // endings, and directions.

        // The minimum bar length is 1, so that the barlines for an empty bar
        // (e.g. a repeat end that is immediately followed by a repeat start)
        // are not on top of each other.
        int barLength = 1;

        if (myGuitarState.inMultibarRest && myBassState.inMultibarRest)
        {
            // If both scores are in a multi-bar rest, insert a multi-bar rest
            // for the shorter duration of the two.
            const int count = std::min(myGuitarState.multibarRestCount,
                                       myBassState.multibarRestCount);

            auto action = std::bind(insertMultiBarRest, std::placeholders::_1,
                                    std::placeholders::_2, count);
            barLength = importNotes(myDestLoc, myGuitarState, action);
            barLength = std::max(
                barLength, importNotes(myDestLoc, myBassState, action));

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
                    length = importNotes(myDestLoc, *state, insertWholeRest);

                    if (state->inMultibarRest)
                        --state->multibarRestCount;
                }
                else
                    length = importNotes(myDestLoc, *state, copyNotes);

                barLength = std::max(barLength, length);
            }
        }

        // Merge any player changes from the scores.
        mergePlayerChanges();

        // Merge any tempo markers or chord symbols from the scores.
        mergeSystemSymbols();

        myGuitarState.advance();
        myBassState.advance();

        const int nextBarPos = destBar->getPosition() + barLength;

        // If we're about to move to a new system, transition from finishing to
        // done.
        if (nextBarPos > POSITION_LIMIT)
        {
            myGuitarState.finishIfPossible();
            myBassState.finishIfPossible();
        }

        const bool exiting =
            myGuitarState.outOfNotes() && myBassState.outOfNotes();

        // Create the next bar or move to the next system.
        if (exiting || nextBarPos > POSITION_LIMIT)
        {
            Barline &endBar = destSystem.getBarlines().back();

            // Copy over some of the next bar's properties to the end bar.
            if (nextDestBar.getBarType() != Barline::RepeatStart)
                endBar.setBarType(nextDestBar.getBarType());
            endBar.setRepeatCount(nextDestBar.getRepeatCount());
            endBar.setPosition(nextBarPos);
            hideSignaturesAndRehearsalSign(endBar);

            if (exiting)
            {
                // Ensure that we have a double bar or a repeat at the end.
                if (endBar.getBarType() != Barline::RepeatEnd)
                    endBar.setBarType(Barline::DoubleBarFine);
                break;
            }
            else
            {
                myDestScore.insertSystem(System());
                myPrevNumGuitarStaves = myNumGuitarStaves;
                myNumGuitarStaves = 0;
                myDestCaret.moveSystem(1);
            }
        }
        else
        {
            nextDestBar.setPosition(nextBarPos);
            destSystem.insertBarline(nextDestBar);

            myDestCaret.moveToNextBar();
            destSystem.getBarlines().back().setPosition(nextBarPos + 10);
        }
    }
}

ScoreMerger::State::State(Score &score, bool isBass)
    : caret(score),
      repeatIndex(score),
      loc(caret.getLocation()),
      isBass(isBass),
      inMultibarRest(false),
      expandingMultibarRest(false),
      multibarRestCount(0),
      repeatState(NO_REPEAT),
      remainingRepeats(0),
      numMergedRepeats(0),
      done(false),
      finishing(false)
{
    bool empty = true;
    for (const Staff &staff : loc.getSystem().getStaves())
    {
        for (const Voice &voice : staff.getVoices())
            empty &= voice.getPositions().empty();
    }

    // If it looks like the score is unused, don't do anything.
    if (empty)
        done = true;
}

bool ScoreMerger::State::outOfNotes() const
{
    return done || finishing;
}

void ScoreMerger::State::advance()
{
    if (inMultibarRest && multibarRestCount == 0)
    {
        inMultibarRest = false;
        expandingMultibarRest = false;
    }
    else if (inMultibarRest)
        expandingMultibarRest = true;

    // If we're expanding a repeated section, jump back to the start bar when we
    // reach the end bar of the source. If the last bar is a multi-bar rest, we
    // need to first finish expanding that.
    if (!inMultibarRest && repeatState != NO_REPEAT && remainingRepeats)
    {
        const Barline *nextBar = caret.getLocation().getSystem().getNextBarline(
            caret.getLocation().getPositionIndex());
        SystemLocation endBarLoc(caret.getLocation().getSystemIndex(),
                                 nextBar->getPosition());

        if (repeatedSection->getRepeatEndBars().find(endBarLoc) !=
            repeatedSection->getRepeatEndBars().end())
        {
            caret.moveToSystem(
                repeatedSection->getStartBarLocation().getSystem(), true);
            caret.moveToPosition(
                repeatedSection->getStartBarLocation().getPosition());
            --remainingRepeats;
            return;
        }
    }

    // Otherwise, just move on to the next bar.
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

    const Position *rest = loc.findMultiBarRest();
    if (rest)
    {
        inMultibarRest = true;
        multibarRestCount = rest->getMultiBarRestCount();
    }
}

void ScoreMerger::State::checkForRepeatedSection()
{
    // The +1 offset is important - the bar after a repeat end barline shouldn't
    // be considered part of that repeat section, so we need to move off of that
    // barline.
    repeatedSection = repeatIndex.findRepeat(
        SystemLocation(caret.getLocation().getSystemIndex(),
                       caret.getLocation().getPositionIndex() + 1));
    if (repeatedSection)
    {
        if (repeatState == NO_REPEAT)
        {
            remainingRepeats = repeatedSection->getTotalRepeatCount() - 1;
            numMergedRepeats = 0;
            // Initially, try to merge the repeat with another repeat.
            repeatState = MERGING_REPEAT;
        }
    }
    else
        repeatState = NO_REPEAT;
}

void ScoreMerger::State::compareRepeatedSection(State &other)
{
    // Check if we can actually merge these sections.
    if (repeatState == MERGING_REPEAT && other.repeatState == MERGING_REPEAT)
    {
        // Don't redo this on subsequent bars.
        if (numMergedRepeats)
            return;

        // If the two sections have the same number of bars, etc., then we don't
        // need to expand the repeats.
        if (repeatedSection->hasSameStructure(*other.repeatedSection))
        {
            // The repeated sections might have a different number of repeats,
            // so we need to compute how many repetitions we can merge them for.
            numMergedRepeats = std::min(remainingRepeats, other.remainingRepeats);
            remainingRepeats -= numMergedRepeats;
            other.remainingRepeats -= numMergedRepeats;
            other.numMergedRepeats = numMergedRepeats;
            return;
        }
    }

    // Otherwise, expand the repeats.
    if (repeatState == MERGING_REPEAT)
        repeatState = EXPANDING_REPEAT;
    if (other.repeatState == MERGING_REPEAT)
        other.repeatState = EXPANDING_REPEAT;
}
