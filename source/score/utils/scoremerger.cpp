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

#include <list>

#include <app/caret.h>
#include <app/viewoptions.h>
#include <score/score.h>
#include <score/systemlocation.h>
#include <score/utils.h>
#include <score/voiceutils.h>

static const int thePositionLimit = 30;
static const ViewOptions theDefaultViewOptions;

class ExpandedBar
{
public:
    ExpandedBar(const SystemLocation &location, int rest_count)
        : myLocation(location), myMultiBarRestCount(rest_count)
    {
    }

    const SystemLocation &getLocation() const { return myLocation; }

    int getMultiBarRestCount() const { return myMultiBarRestCount; }
    void setMultiBarRestCount(int count)
    {
        myMultiBarRestCount = count;
    }

private:
    SystemLocation myLocation;

    /// Multi-bar rest count from the source bar.
    int myMultiBarRestCount;
};

typedef std::list<ExpandedBar> ExpandedBarList;

static void expandScore(Score &score, ExpandedBarList &expanded_bars)
{
    Caret caret(score, theDefaultViewOptions);

    while (true)
    {
        const SystemLocation location(caret.getLocation().getSystemIndex(),
                                      caret.getLocation().getPositionIndex());

        const Position *multibar_rest = caret.getLocation().findMultiBarRest();
        if (multibar_rest)
        {
            for (int i = multibar_rest->getMultiBarRestCount(); i > 0; --i)
                expanded_bars.emplace_back(location, i);
        }
        else
        {
            expanded_bars.emplace_back(location, 0);
        }

        // TODO - handle repeats.
        // TODO - handle directions.
        // TODO - skip empty bars?

        if (!caret.moveToNextBar())
            break;
    }
}

static void mergePlayers(Score &dest_score, const Score &guitar_score,
                         const Score &bass_score)
{
    for (const Player &player : guitar_score.getPlayers())
        dest_score.insertPlayer(player);
    for (const Player &player : bass_score.getPlayers())
        dest_score.insertPlayer(player);

    for (const Instrument &instrument : guitar_score.getInstruments())
        dest_score.insertInstrument(instrument);
    for (const Instrument &instrument : bass_score.getInstruments())
        dest_score.insertInstrument(instrument);
}

static void getPositionRange(const ScoreLocation &dest,
                             const ScoreLocation &src, int &offset, int &left,
                             int &right)
{
    const System &src_system = src.getSystem();
    const Barline *src_bar = src.getBarline();
    assert(src_bar);
    const Barline *next_src_bar =
        src_system.getNextBarline(src_bar->getPosition());
    assert(next_src_bar);

    offset = dest.getPositionIndex() - src_bar->getPosition();
    if (src_bar->getPosition() != 0)
        --offset;

    left = src_bar->getPosition();
    right = next_src_bar->getPosition();
}

static int insertMultiBarRest(ScoreLocation &dest, ScoreLocation &, int count)
{
    const bool is_multibar = count >= 2;
    Position rest(dest.getPositionIndex(), Position::WholeNote);
    rest.setRest();
    if (is_multibar)
        rest.setMultiBarRest(count);
    dest.getVoice().insertPosition(rest);

    // A multi-bar rest should probably span at least a few positions. A whole
    // rest spans a somewhat smaller range.
    return is_multibar ? 16 : 8;
}

/// Copy notes from the source bar to the destination.
static int copyNotes(ScoreLocation &dest, const ScoreLocation &src)
{
    int offset, left, right;
    getPositionRange(dest, src, offset, left, right);

    auto positions =
        ScoreUtils::findInRange(src.getVoice().getPositions(), left, right);

    if (!positions.empty())
    {
        for (const Position &pos : positions)
        {
            Position new_pos(pos);
            new_pos.setPosition(new_pos.getPosition() + offset);
            dest.getVoice().insertPosition(new_pos);
        }

        for (const IrregularGrouping *group :
             VoiceUtils::getIrregularGroupsInRange(src.getVoice(), left, right))
        {
            IrregularGrouping new_group(*group);
            new_group.setPosition(new_group.getPosition() + offset);
            dest.getVoice().insertIrregularGrouping(new_group);
        }

        int length = right - left;
        if (left == 0)
            ++length;

        return length;
    }
    else
        return 0;
}

static int importNotes(
    ScoreLocation &dest_loc, ScoreLocation &src_loc, bool is_bass,
    int &num_guitar_staves,
    std::function<int(ScoreLocation &, ScoreLocation &)> action)
{
    System &dest_system = dest_loc.getSystem();
    const System &src_system = src_loc.getSystem();

    int offset, left, right;
    getPositionRange(dest_loc, src_loc, offset, left, right);

    const int staff_offset = is_bass ? num_guitar_staves : 0;
    int length = 0;

    // Merge the notes for each staff.
    for (int i = 0; i < src_system.getStaves().size(); ++i)
    {
        // Ensure that there are enough staves in the destination system.
        if ((!is_bass && num_guitar_staves <= i) ||
            dest_system.getStaves().size() <= i + staff_offset)
        {
            const Staff &srcStaff = src_system.getStaves()[i];
            Staff destStaff(srcStaff.getStringCount());
            destStaff.setClefType(srcStaff.getClefType());
            dest_system.insertStaff(destStaff);

            if (!is_bass)
                ++num_guitar_staves;
        }

        dest_loc.setStaffIndex(i + staff_offset);
        src_loc.setStaffIndex(i);

// Import dynamics, but don't repeatedly do so when expanding a
// multi-bar rest.
        // TODO - implement this once multi-bar rests are supported.
#if 0
        if (!srcState.expandingMultibarRest)
        {
            for (const Dynamic &dynamic : ScoreUtils::findInRange(
                     src_loc.getStaff().getDynamics(), left, right - 1))
            {
                Dynamic newDynamic(dynamic);
                newDynamic.setPosition(newDynamic.getPosition() + offset);
                dest_loc.getStaff().insertDynamic(newDynamic);
            }
        }
#endif

        // Import each voice.
        for (int v = 0; v < Staff::NUM_VOICES; ++v)
        {
            dest_loc.setVoiceIndex(v);
            src_loc.setVoiceIndex(v);

            length = std::max(length, action(dest_loc, src_loc));
        }
    }

    return length;
}

static int copyContent(ScoreLocation &dest_loc, int &num_guitar_staves,
                       Caret &src_caret, const ExpandedBar &src_bar,
                       bool is_bass)
{
    src_caret.moveToSystem(src_bar.getLocation().getSystem(), false);
    src_caret.moveToPosition(src_bar.getLocation().getPosition());
    ScoreLocation &src_loc = src_caret.getLocation();

    if (src_bar.getMultiBarRestCount() > 0)
    {
        auto insert_rest =
            std::bind(insertMultiBarRest, std::placeholders::_1,
                      std::placeholders::_2, src_bar.getMultiBarRestCount());
        return importNotes(dest_loc, src_loc, is_bass, num_guitar_staves,
                           insert_rest);
    }
    else
    {
        return importNotes(dest_loc, src_loc, is_bass, num_guitar_staves,
                           copyNotes);
    }
}

static void combineScores(Score &dest_score, Score &guitar_score,
                          const ExpandedBarList &guitar_bars, Score &bass_score,
                          const ExpandedBarList &bass_bars)
{
    mergePlayers(dest_score, guitar_score, bass_score);

    int num_guitar_staves = 0;

    dest_score.insertSystem(System());
    Caret dest_caret(dest_score, theDefaultViewOptions);
    ScoreLocation &dest_loc = dest_caret.getLocation();

    Caret guitar_caret(guitar_score, theDefaultViewOptions);
    ScoreLocation &guitar_loc = guitar_caret.getLocation();
    Caret bass_caret(bass_score, theDefaultViewOptions);
    ScoreLocation &bass_loc = bass_caret.getLocation();

    auto guitar_bar = guitar_bars.begin();
    const auto end_guitar_bar = guitar_bars.end();
    auto bass_bar = bass_bars.begin();
    const auto end_bass_bar = bass_bars.end();

    while (guitar_bar != end_guitar_bar || bass_bar != end_bass_bar)
    {
        System &dest_system = dest_loc.getSystem();

        // Insert notes at the first position after the barline, except when
        // we're at the start of the system.
        if (dest_loc.getPositionIndex() > 0)
        {
            Barline barline(dest_loc.getPositionIndex(), Barline::SingleBar);
            dest_system.insertBarline(barline);
            dest_caret.moveHorizontal(1);
        }

        int bar_length = 0;
        if (guitar_bar != end_guitar_bar)
        {
            bar_length = std::max(
                bar_length, copyContent(dest_loc, num_guitar_staves,
                                        guitar_caret, *guitar_bar, false));
        }
        if (bass_bar != end_bass_bar)
        {
            bar_length =
                std::max(bar_length, copyContent(dest_loc, num_guitar_staves,
                                                 bass_caret, *bass_bar, true));
        }

        // TODO - merge player changes and system symbols.

        // Advance to the next bar in the source scores.
        if (guitar_bar != end_guitar_bar)
            ++guitar_bar;
        if (bass_bar != end_bass_bar)
            ++bass_bar;

        const int next_bar_pos =
            dest_caret.getLocation().getPositionIndex() + bar_length;

        const bool finishing =
            (guitar_bar == end_guitar_bar && bass_bar == end_bass_bar);

        if (finishing || next_bar_pos > thePositionLimit)
        {
            Barline &end_bar = dest_system.getBarlines().back();
            end_bar.setPosition(next_bar_pos);

            if (finishing)
                end_bar.setBarType(Barline::DoubleBarFine);
            else
            {
                dest_score.insertSystem(System());
                dest_caret.moveSystem(1);
                num_guitar_staves = 0;
            }
        }
        else
            dest_caret.moveToPosition(next_bar_pos);
    }
}

/// Merge the expanded bars from a multi-bar rest.
ExpandedBarList::iterator mergeMultiBarRest(ExpandedBarList &bars,
                                            ExpandedBarList::iterator bar,
                                            int count)
{
    bar->setMultiBarRestCount(count);

    // Remove the following bars that were expanded.
    auto range_begin = bar;
    auto range_end = bar;
    std::advance(range_begin, 1);
    std::advance(range_end, count);
    return bars.erase(range_begin, range_end);
}

void mergeMultiBarRests(ExpandedBarList &guitar_bars,
                        ExpandedBarList &bass_bars)
{
    auto guitar_bar = guitar_bars.begin();
    auto guitar_end_bar = guitar_bars.end();
    auto bass_bar = bass_bars.begin();
    auto bass_end_bar = bass_bars.end();

    while (guitar_bar != guitar_end_bar && bass_bar != bass_end_bar)
    {
        if (guitar_bar->getMultiBarRestCount() > 0 &&
            bass_bar->getMultiBarRestCount() > 0)
        {
            const int count = std::min(guitar_bar->getMultiBarRestCount(),
                                       bass_bar->getMultiBarRestCount());
            guitar_bar = mergeMultiBarRest(guitar_bars, guitar_bar, count);
            bass_bar = mergeMultiBarRest(bass_bars, bass_bar, count);
            continue;
        }

        // Otherwise, keep the expanded bars and convert them to a whole rest.
        if (guitar_bar->getMultiBarRestCount() > 0)
            guitar_bar->setMultiBarRestCount(1);
        else if (bass_bar->getMultiBarRestCount() > 0)
            bass_bar->setMultiBarRestCount(1);

        ++guitar_bar;
        ++bass_bar;
    }

    while (guitar_bar != guitar_end_bar)
    {
        if (guitar_bar->getMultiBarRestCount() > 0)
        {
            guitar_bar = mergeMultiBarRest(guitar_bars, guitar_bar,
                                           guitar_bar->getMultiBarRestCount());
        }
        else
            ++guitar_bar;
    }
    while (bass_bar != bass_end_bar)
    {
        if (bass_bar->getMultiBarRestCount() > 0)
        {
            bass_bar = mergeMultiBarRest(bass_bars, bass_bar,
                                         bass_bar->getMultiBarRestCount());
        }
        else
            ++bass_bar;
    }
}

void ScoreMerger::merge(Score &dest_score, Score &guitar_score,
                        Score &bass_score)
{
    ExpandedBarList guitar_bars;
    ExpandedBarList bass_bars;
    expandScore(guitar_score, guitar_bars);
    expandScore(bass_score, bass_bars);

    mergeMultiBarRests(guitar_bars, bass_bars);

    // TODO - merge repeats.

    combineScores(dest_score, guitar_score, guitar_bars, bass_score, bass_bars);
}
