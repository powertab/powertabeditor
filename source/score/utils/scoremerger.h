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

#ifndef SCORE_UTILS_SCOREMERGER_H
#define SCORE_UTILS_SCOREMERGER_H

#include <app/caret.h>

class PlayerChange;

class ScoreMerger
{
    struct State;

public:
    ScoreMerger(Score &dest, Score &guitarScore, Score &bassScore);

    void merge();

private:
    /// Merge players and instruments.
    void mergePlayers();

    int importNotes(
        ScoreLocation &dest, State &srcState,
        std::function<int(ScoreLocation &, ScoreLocation &)> action);

    /// Fetch the current pair of barlines from one of the source scores.
    void copyBarsFromSource(Barline &destBar, Barline &nextDestBar);

    /// Combine player changes from the two scores.
    void mergePlayerChanges();

    /// Merge in tempo markers, etc. from the scores.
    void mergeSystemSymbols();

    /// Check for a player change in the current bar.
    const PlayerChange *findPlayerChange(const State &state);

    struct State
    {
        State(Score &score, bool isBass);

        Caret caret;
        ScoreLocation &loc;
        bool isBass;

        bool inMultibarRest;
        bool expandingMultibarRest;
        int multibarRestCount;

        bool done;
        bool finishing;

        bool outOfNotes() const;
        void advance();
        void finishIfPossible();
        void checkForMultibarRest();
    };

    Score &myDestScore;
    Caret myDestCaret;
    ScoreLocation &myDestLoc;

    Score &myGuitarScore;
    Score &myBassScore;
    State myGuitarState;
    State myBassState;

    int myNumGuitarStaves;
    int myPrevNumGuitarStaves;
};

#endif
