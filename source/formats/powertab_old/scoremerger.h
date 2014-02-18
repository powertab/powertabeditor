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

#ifndef FORMATS_SCOREMERGER_H
#define FORMATS_SCOREMERGER_H

#include <app/caret.h>

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
        ScoreLocation &dest, ScoreLocation &srcLoc, bool bass,
        std::function<int(ScoreLocation &, ScoreLocation &)> action);

    struct State
    {
        State(Score &score, bool isBass);

        Caret caret;
        ScoreLocation &loc;
        bool isBass;
        bool inMultibarRest;
        int multibarRestCount;
        bool done;

        void advance();
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
};

#endif
