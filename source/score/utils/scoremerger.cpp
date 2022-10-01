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
#include <unordered_set>

#include <app/caret.h>
#include <app/viewoptions.h>
#include <score/score.h>
#include <score/systemlocation.h>
#include <score/utils.h>
#include <score/utils/repeatindexer.h>
#include <score/voiceutils.h>

static const int thePositionLimit = 30;
static const ViewOptions theDefaultViewOptions;

class ExpandedBar
{
public:
    ExpandedBar(const SystemLocation &location, bool is_expanded,
                int rest_count, const Barline &start_bar, int remaining_repeats,
                bool is_repeat_end, bool is_alt_ending)
        : myLocation(location),
          myMultiBarRestCount(rest_count),
          myIsExpanded(is_expanded),
          myStartBar(start_bar),
          myRemainingRepeats(remaining_repeats),
          myRepeatEnd(is_repeat_end),
          myAlternateEnding(is_alt_ending)
    {
    }

    const SystemLocation &getLocation() const { return myLocation; }

    int getMultiBarRestCount() const { return myMultiBarRestCount; }
    void setMultiBarRestCount(int count)
    {
        myMultiBarRestCount = count;
    }

    bool isExpanded() const { return myIsExpanded; }

    const Barline &getStartBar() const { return myStartBar; }
    int getRemainingRepeats() const { return myRemainingRepeats; }
    bool isRepeatEnd() const { return myRepeatEnd; }
    bool isAlternateEnding() const { return myAlternateEnding; }

    void clearRepeat()
    {
        if (myStartBar.getBarType() == Barline::RepeatStart)
            myStartBar.setBarType(Barline::SingleBar);
        myRemainingRepeats = 0;
        myRepeatEnd = false;
        myAlternateEnding = false;
    }

private:
    SystemLocation myLocation;

    /// Multi-bar rest count from the source bar.
    int myMultiBarRestCount;

    /// Whether this bar was expanded from e.g. a multi-bar rest or a repeated
    /// section.
    bool myIsExpanded;

    /// Original barline (including key signature, etc).
    Barline myStartBar;

    /// For an expanded repeated section, the current number of remaining
    /// repeats.
    int myRemainingRepeats;

    /// Whether the bar is the end of a repeated section.
    bool myRepeatEnd;

    /// Whether the bar is part of an alternate ending.
    bool myAlternateEnding;
};

typedef std::list<ExpandedBar> ExpandedBarList;

static void expandScore(Score &score, ExpandedBarList &expanded_bars)
{
    Caret caret(score, theDefaultViewOptions);
    RepeatIndexer repeat_index(score);
    int remaining_repeats = 0;
    bool alternate_ending = false;

    while (true)
    {
        const ScoreLocation &score_loc = caret.getLocation();
        const System &system = score_loc.getSystem();
        const Barline *prev_bar =
            system.getPreviousBarline(score_loc.getPositionIndex() + 1);
        const Barline *next_bar =
            system.getNextBarline(score_loc.getPositionIndex());
        if (!next_bar)
        {
            // We might have ended up at the end bar, so move to next system.
            if (!caret.moveToNextBar())
                break;

            continue;
        }

        // Ensure that the caret is actually at the start of the bar.
        caret.moveToPosition(prev_bar->getPosition());

        const SystemLocation location(score_loc.getSystemIndex(),
                                      score_loc.getPositionIndex());
        const SystemLocation next_bar_loc(location.getSystem(),
                                          next_bar->getPosition());

        RepeatedSection *active_repeat = repeat_index.findRepeat(next_bar_loc);
        if (active_repeat)
        {
            if (!remaining_repeats)
                remaining_repeats = active_repeat->getTotalRepeatCount();
        }
        else
        {
            remaining_repeats = 0;
            alternate_ending = false;
        }

        if (!ScoreUtils::findInRange(system.getAlternateEndings(),
                                     prev_bar->getPosition(),
                                     next_bar->getPosition() - 1).empty())
        {
            alternate_ending = true;
        }

        const Position *multibar_rest = score_loc.findMultiBarRest();
        if (multibar_rest)
        {
            for (int i = multibar_rest->getMultiBarRestCount(); i > 0; --i)
            {
                expanded_bars.emplace_back(
                    location, i != multibar_rest->getMultiBarRestCount(), i,
                    *prev_bar, remaining_repeats,
                    next_bar->getBarType() == Barline::RepeatEnd,
                    alternate_ending);
            }
        }
        else if (!score_loc.isEmptyBar())
        {
            expanded_bars.emplace_back(
                location, remaining_repeats > 0, 0, *prev_bar,
                remaining_repeats, next_bar->getBarType() == Barline::RepeatEnd,
                alternate_ending);
        }

        // TODO - handle directions.

        // Follow repeats.
        if (active_repeat)
        {
            SystemLocation new_loc = active_repeat->performRepeat(next_bar_loc);
            if (new_loc != next_bar_loc)
            {
                caret.moveToSystem(new_loc.getSystem(), false);
                caret.moveToPosition(new_loc.getPosition());

                if (next_bar->getBarType() == Barline::RepeatEnd)
                {
                    --remaining_repeats;
                    alternate_ending = false;
                }

                continue;
            }
            else if (remaining_repeats == 1 &&
                     next_bar->getBarType() == Barline::RepeatEnd)
            {
                remaining_repeats = 0;
            }
        }

        // Otherwise, advance to the next bar.
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

static void
mergeChordDiagrams(Score &dest_score, const Score &guitar_score,
                   const Score &bass_score)
{
    for (const ChordDiagram &diagram : guitar_score.getChordDiagrams())
        dest_score.insertChordDiagram(diagram);

    for (const ChordDiagram &diagram : bass_score.getChordDiagrams())
        dest_score.insertChordDiagram(diagram);
}

/// Returns the position range (inclusive) and offset to copy objects into the
/// destination location.
static void
getPositionRange(const ScoreLocation &dest, const ScoreLocation &src,
                 bool copy_positions, int &offset, int &left, int &right)
{
    const System &src_system = src.getSystem();
    const Barline *src_bar = src.getBarline();
    assert(src_bar);
    const Barline *next_src_bar =
        src_system.getNextBarline(src_bar->getPosition());
    assert(next_src_bar);

    // Note this is expected to be the first position after the dest barline.
    int dest_pos = dest.getPositionIndex();

    if (copy_positions)
    {
        // Positions start after the barline unless at position 0.
        left = (src_bar->getPosition() == 0) ? 0 : (src_bar->getPosition() + 1);
    }
    else
    {
        // Copy other symbols from the src barline to the dest barline.
        left = src_bar->getPosition();
        if (dest_pos > 0)
            --dest_pos;
    }

    offset = dest_pos - left;
    right = next_src_bar->getPosition() - 1;
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
    getPositionRange(dest, src, /* copy_positions */ true, offset, left, right);

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

        int length = right + 1 - left;
        if (left == 0)
            ++length;

        return length;
    }
    else
        return 0;
}

static int importNotes(
    ScoreLocation &dest_loc, ScoreLocation &src_loc, bool is_bass,
    bool is_expanded_bar, int &num_guitar_staves,
    std::function<int(ScoreLocation &, ScoreLocation &)> action)
{
    System &dest_system = dest_loc.getSystem();
    const System &src_system = src_loc.getSystem();

    int offset, left, right;
    getPositionRange(dest_loc, src_loc, false, offset, left, right);

    const int staff_offset = is_bass ? num_guitar_staves : 0;
    int length = 0;

    // Merge the notes for each staff.
    for (int i = 0; i < static_cast<int>(src_system.getStaves().size()); ++i)
    {
        // Ensure that there are enough staves in the destination system.
        if ((!is_bass && num_guitar_staves <= i) ||
            static_cast<int>(dest_system.getStaves().size()) <=
                i + staff_offset)
        {
            const Staff &src_staff = src_system.getStaves()[i];
            Staff dest_staff(src_staff.getStringCount());
            dest_staff.setClefType(src_staff.getClefType());
            dest_system.insertStaff(dest_staff, i + staff_offset);

            if (!is_bass)
                ++num_guitar_staves;
        }

        dest_loc.setStaffIndex(i + staff_offset);
        src_loc.setStaffIndex(i);

        assert(src_loc.getStaff().getStringCount() ==
               dest_loc.getStaff().getStringCount());

        // Import dynamics, but don't repeatedly do so when e.g. a multi-bar
        // rest was expanded.
        if (!is_expanded_bar)
        {
            for (const Dynamic &dynamic : ScoreUtils::findInRange(
                     src_loc.getStaff().getDynamics(), left, right))
            {
                Dynamic new_dynamic(dynamic);
                new_dynamic.setPosition(dynamic.getPosition() + offset);
                dest_loc.getStaff().insertDynamic(new_dynamic);
            }
        }

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

template <typename Symbol>
static void copySymbols(
    const boost::iterator_range<typename std::vector<Symbol>::const_iterator> &
        src_symbols,
    System &dest_system,
    const boost::iterator_range<typename std::vector<Symbol>::const_iterator> &
        dest_symbols,
    void (System::*add_symbol)(const Symbol &), int offset, int left, int right)
{
    std::unordered_set<int> filled_positions;
    for (const Symbol &dest_symbol : dest_symbols)
        filled_positions.insert(dest_symbol.getPosition());

    for (const Symbol &src_symbol :
         ScoreUtils::findInRange(src_symbols, left, right))
    {
        Symbol symbol(src_symbol);
        symbol.setPosition(src_symbol.getPosition() + offset);

        // We might get duplicate symbols from the guitar and bass scores.
        if (filled_positions.find(symbol.getPosition()) !=
            filled_positions.end())
        {
            continue;
        }

        (dest_system.*add_symbol)(symbol);
    }
}

static void mergeSystemSymbols(ScoreLocation &dest_loc,
                               const ScoreLocation &src_loc,
                               const ExpandedBar &src_bar)
{
    int offset, left, right;
    getPositionRange(dest_loc, src_loc, false, offset, left, right);

    System &dest_system = dest_loc.getSystem();
    const System &src_system = src_loc.getSystem();

    if (!src_bar.isExpanded())
    {
        copySymbols(src_system.getTempoMarkers(), dest_system,
                    dest_system.getTempoMarkers(), &System::insertTempoMarker,
                    offset, left, right);

        copySymbols(src_system.getTextItems(), dest_system,
                    dest_system.getTextItems(), &System::insertTextItem, offset,
                    left, right);
    }

    copySymbols(src_system.getChords(), dest_system, dest_system.getChords(),
                &System::insertChord, offset, left, right);

    if (src_bar.isAlternateEnding())
    {
        copySymbols(src_system.getAlternateEndings(), dest_system,
                    dest_system.getAlternateEndings(),
                    &System::insertAlternateEnding, offset, left, right);
    }
}

static int copyContent(ScoreLocation &dest_loc, int &num_guitar_staves,
                       Caret &src_caret, const ExpandedBar &src_bar,
                       bool is_bass)
{
    src_caret.moveToSystem(src_bar.getLocation().getSystem(), false);
    src_caret.moveToPosition(src_bar.getLocation().getPosition());
    ScoreLocation &src_loc = src_caret.getLocation();

    mergeSystemSymbols(dest_loc, src_loc, src_bar);

    if (src_bar.getMultiBarRestCount() > 0)
    {
        auto insert_rest =
            std::bind(insertMultiBarRest, std::placeholders::_1,
                      std::placeholders::_2, src_bar.getMultiBarRestCount());
        return importNotes(dest_loc, src_loc, is_bass, src_bar.isExpanded(),
                           num_guitar_staves, insert_rest);
    }
    else
    {
        return importNotes(dest_loc, src_loc, is_bass, src_bar.isExpanded(),
                           num_guitar_staves, copyNotes);
    }
}

static const PlayerChange *findPlayerChange(
    const ScoreLocation &dest_loc, const ScoreLocation &src_loc,
    ExpandedBarList::const_iterator src_bar,
    ExpandedBarList::const_iterator end_src_bar)
{
    if (src_bar == end_src_bar || src_bar->isExpanded())
        return nullptr;

    int offset, left, right;
    getPositionRange(dest_loc, src_loc, false, offset, left, right);

    auto changes = ScoreUtils::findInRange(
        src_loc.getSystem().getPlayerChanges(), left, right);

    return changes.empty() ? nullptr : &changes.front();
}

static void mergePlayerChanges(ScoreLocation &dest_loc,
                               const ScoreLocation &guitar_loc,
                               const ScoreLocation &bass_loc,
                               ExpandedBarList::const_iterator guitar_bar,
                               ExpandedBarList::const_iterator end_guitar_bar,
                               ExpandedBarList::const_iterator bass_bar,
                               ExpandedBarList::const_iterator end_bass_bar,
                               int num_guitar_staves,
                               int prev_num_guitar_staves)
{
    System &dest_system = dest_loc.getSystem();
    const PlayerChange *guitar_change =
        findPlayerChange(dest_loc, guitar_loc, guitar_bar, end_guitar_bar);
    const PlayerChange *bass_change =
        findPlayerChange(dest_loc, bass_loc, bass_bar, end_bass_bar);

    // If either the guitar or bass score has a player change, or we're in a
    // system that has a different number of guitar staves, insert a player
    // change to ensure that player are assigned to the correct staves.
    if (guitar_change || bass_change ||
        (num_guitar_staves != prev_num_guitar_staves &&
         !ScoreUtils::findByPosition(dest_system.getPlayerChanges(), 0)))
    {
        PlayerChange change;

        if (!guitar_change && guitar_bar != end_guitar_bar)
        {
            // If there is only a player change in the bass score, carry over
            // the current active players from the guitar score.
            guitar_change = ScoreUtils::getCurrentPlayers(
                guitar_loc.getScore(), guitar_loc.getSystemIndex(),
                guitar_loc.getPositionIndex());
        }

        if (!bass_change && bass_bar != end_bass_bar)
        {
            // If there is only a player change in the guitar score, carry over
            // the current active players from the bass score.
            bass_change = ScoreUtils::getCurrentPlayers(
                bass_loc.getScore(), bass_loc.getSystemIndex(),
                bass_loc.getPositionIndex());
        }

        // Merge in data from only the active staves.
        if (guitar_change)
        {
            for (int i = 0; i < num_guitar_staves; ++i)
            {
                for (const ActivePlayer &player :
                     guitar_change->getActivePlayers(i))
                {
                    change.insertActivePlayer(i, player);
                }
            }
        }

        // Merge in the bass score's player change and adjust
        // staff/player/instrument numbers.
        if (bass_change)
        {
            for (unsigned int i = 0; i < bass_loc.getSystem().getStaves().size(); ++i)
            {
                for (const ActivePlayer &player :
                     bass_change->getActivePlayers(i))
                {
                    change.insertActivePlayer(
                        num_guitar_staves + i,
                        ActivePlayer(
                            static_cast<int>(
                                guitar_loc.getScore().getPlayers().size()) +
                                player.getPlayerNumber(),
                            static_cast<int>(
                                guitar_loc.getScore().getInstruments().size()) +
                                player.getInstrumentNumber()));
                }
            }
        }

        // If the number of staves changed, insert the player change at the
        // start of the system.
        if (!guitar_change && !bass_change)
            change.setPosition(0);
        else
            change.setPosition(dest_loc.getPositionIndex());

        dest_system.insertPlayerChange(change);
    }
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

/// After moving to the next bar, inspects the source system to determine if it
/// is significantly different from the current system in the destination score
/// (e.g. some of the staves have different numbers of strings and/or are
/// reordered). In such cases it is preferable to just move to a new system in
/// the destination score.
static bool areStavesIncompatible(const ScoreLocation &dest_loc,
                                  Caret &src_caret,
                                  ExpandedBarList::const_iterator src_bar,
                                  ExpandedBarList::const_iterator end_src_bar,
                                  int staff_begin, int staff_end)
{
    if (src_bar == end_src_bar)
        return false;

    src_caret.moveToSystem(src_bar->getLocation().getSystem(), false);
    src_caret.moveToPosition(src_bar->getLocation().getPosition());
    const ScoreLocation &src_loc = src_caret.getLocation();

    const System &dest_system = dest_loc.getSystem();
    const System &src_system = src_loc.getSystem();

    for (int i = staff_begin; i < staff_end; ++i)
    {
        if ((i - staff_begin) <
                static_cast<int>(src_system.getStaves().size()) &&
            dest_system.getStaves()[i].getStringCount() !=
                src_system.getStaves()[i - staff_begin].getStringCount())
        {
            return true;
        }
    }

    return false;
}

static bool areStavesIncompatible(
    const ScoreLocation &dest_loc, Caret &guitar_caret, Caret &bass_caret,
    ExpandedBarList::const_iterator guitar_bar,
    ExpandedBarList::const_iterator end_guitar_bar,
    ExpandedBarList::const_iterator bass_bar,
    ExpandedBarList::const_iterator end_bass_bar, int num_guitar_staves)
{
    return areStavesIncompatible(dest_loc, guitar_caret, guitar_bar,
                                 end_guitar_bar, 0, num_guitar_staves) ||
           areStavesIncompatible(
               dest_loc, bass_caret, bass_bar, end_bass_bar, num_guitar_staves,
               static_cast<int>(dest_loc.getSystem().getStaves().size()));
}

static void insertNewSystem(Score &score)
{
    // Initially move the end bar very far away - it will be set to the correct
    // position after the system is filled in.
    System system;
    system.getBarlines().back().setPosition(std::numeric_limits<int>::max());

    score.insertSystem(system);
}

static void combineScores(Score &dest_score, Score &guitar_score,
                          const ExpandedBarList &guitar_bars, Score &bass_score,
                          const ExpandedBarList &bass_bars)
{
    mergePlayers(dest_score, guitar_score, bass_score);
    mergeChordDiagrams(dest_score, guitar_score, bass_score);

    int num_guitar_staves = 0;
    int prev_num_guitar_staves = 0;

    insertNewSystem(dest_score);
    Caret dest_caret(dest_score, theDefaultViewOptions);
    ScoreLocation &dest_loc = dest_caret.getLocation();

    Caret guitar_caret(guitar_score, theDefaultViewOptions);
    const ScoreLocation &guitar_loc = guitar_caret.getLocation();
    Caret bass_caret(bass_score, theDefaultViewOptions);
    const ScoreLocation &bass_loc = bass_caret.getLocation();

    auto guitar_bar = guitar_bars.begin();
    const auto end_guitar_bar = guitar_bars.end();
    auto bass_bar = bass_bars.begin();
    const auto end_bass_bar = bass_bars.end();

    while (guitar_bar != end_guitar_bar || bass_bar != end_bass_bar)
    {
        System &dest_system = dest_loc.getSystem();

        const ExpandedBarList::const_iterator current_bar =
            (guitar_bar != end_guitar_bar) ? guitar_bar : bass_bar;

        const ExpandedBar *prev_bar = (guitar_bar != guitar_bars.begin())
                                          ? &*std::prev(current_bar)
                                          : nullptr;

        // Add a barline if necessary.
        if (dest_loc.getPositionIndex() > 0)
        {
            // If a repeated section starts immediately after another, we need
            // an extra barline.
            if (prev_bar && prev_bar->isRepeatEnd() &&
                current_bar->getStartBar().getBarType() == Barline::RepeatStart)
            {
                dest_system.insertBarline(
                    Barline(dest_loc.getPositionIndex(), Barline::RepeatEnd,
                            prev_bar->getRemainingRepeats()));
                dest_caret.moveHorizontal(1);
            }

            dest_system.insertBarline(
                Barline(dest_loc.getPositionIndex(), Barline::SingleBar));
        }

        // Set the barline's properties, key signature, etc.
        Barline *barline = ScoreUtils::findByPosition(
            dest_system.getBarlines(), dest_loc.getPositionIndex());
        *barline = current_bar->getStartBar();
        barline->setPosition(dest_loc.getPositionIndex());
        if (current_bar->isExpanded())
            hideSignaturesAndRehearsalSign(*barline);

        if (dest_loc.getPositionIndex() > 0)
        {
            // Insert notes at the first position after the barline, except when
            // we're at the start of the system.
            dest_caret.moveHorizontal(1);
        }
        else if (barline->getBarType() == Barline::RepeatEnd)
        {
            // If we just entered a new system, there is already a repeat end
            // bar at the end of the previous system.
            barline->setBarType(Barline::SingleBar);
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

        mergePlayerChanges(dest_loc, guitar_loc, bass_loc, guitar_bar,
                           end_guitar_bar, bass_bar, end_bass_bar,
                           num_guitar_staves, prev_num_guitar_staves);

        // Advance to the next bar in the source scores.
        if (guitar_bar != end_guitar_bar)
            ++guitar_bar;
        if (bass_bar != end_bass_bar)
            ++bass_bar;

        const int next_bar_pos =
            dest_caret.getLocation().getPositionIndex() + bar_length;

        bool need_new_system = next_bar_pos > thePositionLimit;
        need_new_system |= areStavesIncompatible(
            dest_loc, guitar_caret, bass_caret, guitar_bar, end_guitar_bar,
            bass_bar, end_bass_bar, num_guitar_staves);

        const bool finishing =
            (guitar_bar == end_guitar_bar && bass_bar == end_bass_bar);

        if (finishing || need_new_system)
        {
            Barline &end_bar = dest_system.getBarlines().back();
            end_bar.setPosition(next_bar_pos);

            if (current_bar->isRepeatEnd())
            {
                end_bar.setBarType(Barline::RepeatEnd);
                end_bar.setRepeatCount(current_bar->getRemainingRepeats());
            }
            else if (finishing)
                end_bar.setBarType(Barline::DoubleBarFine);

            if (!finishing)
            {
                insertNewSystem(dest_score);
                dest_caret.moveSystem(1);
                prev_num_guitar_staves = num_guitar_staves;
                num_guitar_staves = 0;
            }
        }
        else
            dest_caret.moveToPosition(next_bar_pos);
    }
}

/// Merge the expanded bars from a multi-bar rest.
static ExpandedBarList::iterator mergeMultiBarRest(
    ExpandedBarList &bars, ExpandedBarList::iterator bar, int count)
{
    bar->setMultiBarRestCount(count);

    // Remove the following bars that were expanded.
    auto range_begin = bar;
    auto range_end = bar;
    std::advance(range_begin, 1);
    std::advance(range_end, count);
    return bars.erase(range_begin, range_end);
}

static void mergeMultiBarRests(ExpandedBarList &guitar_bars,
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

static ExpandedBarList::iterator clearRepeatedSection(
    ExpandedBarList::iterator bar, ExpandedBarList::iterator end_bar)
{
    bool repeat_end = false;

    do
    {
        repeat_end = bar->isRepeatEnd();
        bar->clearRepeat();
        ++bar;
    } while (!repeat_end && bar != end_bar);

    return bar;
}

static ExpandedBarList::iterator collapseRepeatedSection(
    ExpandedBarList &bars, ExpandedBarList::iterator bar, int num_repeats)
{
    // Collapse any non-degenerate repeated sections.
    std::unordered_set<SystemLocation> known_bars;

    // Skip the first repeated section.
    int first_repeat = bar->getRemainingRepeats();
    while (bar != bars.end() && bar->getRemainingRepeats() == first_repeat)
    {
        known_bars.insert(bar->getLocation());
        ++bar;
    }

    // Remove the following expanded repeats. However, we need to keep any bars
    // that are part of an alternate ending.
    for (int i = first_repeat - 1; i > first_repeat - num_repeats; --i)
    {
        while (bar != bars.end() && bar->getRemainingRepeats() == i)
        {
            if (bar->isAlternateEnding() &&
                known_bars.find(bar->getLocation()) == known_bars.end())
            {
                known_bars.insert(bar->getLocation());
                ++bar;
            }
            else
                bar = bars.erase(bar);
        }
    }

    return bar;
}

// If one score is longer than another, we can trivially collapse any repeated
// sections in the longer score.
static void trivialMergeRepeats(ExpandedBarList &bars,
                                ExpandedBarList::iterator bar)
{
    while (bar != bars.end())
    {
        int remaining_repeats = bar->getRemainingRepeats();
        if (remaining_repeats == 1)
        {
            // Clear degenerate repeated sections.
            bar = clearRepeatedSection(bar, bars.end());
        }
        else if (remaining_repeats > 0)
        {
            // Collapse any non-degenerate repeated sections.
            bar = collapseRepeatedSection(bars, bar, remaining_repeats);
        }
        else
            ++bar;
    }
}

static void mergeRepeats(ExpandedBarList &guitar_bars,
                         ExpandedBarList &bass_bars)
{
    auto guitar_bar = guitar_bars.begin();
    auto guitar_end_bar = guitar_bars.end();
    auto bass_bar = bass_bars.begin();
    auto bass_end_bar = bass_bars.end();

    while (guitar_bar != guitar_end_bar && bass_bar != bass_end_bar)
    {
        if (guitar_bar->getStartBar().getBarType() == Barline::RepeatStart &&
            bass_bar->getStartBar().getBarType() == Barline::RepeatStart)
        {
            auto guitar_section_start = guitar_bar;
            auto bass_section_start = bass_bar;
            const int num_repeats = std::min(guitar_bar->getRemainingRepeats(),
                                             bass_bar->getRemainingRepeats());

            while (guitar_bar != guitar_end_bar && bass_bar != bass_end_bar &&
                   !guitar_bar->isRepeatEnd() && !bass_bar->isRepeatEnd())
            {
                ++guitar_bar;
                ++bass_bar;
            }

            // The repeated sections are identical in length, and so can be
            // collapsed.
            // TODO - check that alternate endings match.
            if (guitar_bar != guitar_end_bar && bass_bar != bass_end_bar &&
                guitar_bar->isRepeatEnd() && bass_bar->isRepeatEnd())
            {
                guitar_bar = collapseRepeatedSection(
                    guitar_bars, guitar_section_start, num_repeats);
                bass_bar = collapseRepeatedSection(
                    bass_bars, bass_section_start, num_repeats);
            }
            else
            {
                // Otherwise, clear the repeat bars from the first expanded
                // repeat.
                clearRepeatedSection(guitar_section_start, guitar_end_bar);
                clearRepeatedSection(bass_section_start, bass_end_bar);
            }

            guitar_bar = guitar_section_start;
            bass_bar = bass_section_start;
        }
        else if (guitar_bar->getStartBar().getBarType() == Barline::RepeatStart)
            clearRepeatedSection(guitar_bar, guitar_end_bar);
        else if (bass_bar->getStartBar().getBarType() == Barline::RepeatStart)
            clearRepeatedSection(bass_bar, bass_end_bar);

        ++guitar_bar;
        ++bass_bar;
    }

    // If the scores have different lengths, deal with the remaining repeated
    // sections.
    if (guitar_bar != guitar_end_bar)
        trivialMergeRepeats(guitar_bars, guitar_bar);
    else if (bass_bar != bass_end_bar)
        trivialMergeRepeats(bass_bars, bass_bar);
}

void ScoreMerger::merge(Score &dest_score, Score &guitar_score,
                        Score &bass_score)
{
    ExpandedBarList guitar_bars;
    ExpandedBarList bass_bars;
    expandScore(guitar_score, guitar_bars);
    expandScore(bass_score, bass_bars);

    mergeMultiBarRests(guitar_bars, bass_bars);
    mergeRepeats(guitar_bars, bass_bars);

    combineScores(dest_score, guitar_score, guitar_bars, bass_score, bass_bars);
}
