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

#ifndef SCORE_SCORELOCATION_H
#define SCORE_SCORELOCATION_H

class Score;
class Staff;
class System;

class ScoreLocation
{
public:
    ScoreLocation(const Score &score, int system = 0, int staff = 0,
                  int position = 0, int voice = 0, int string = 0);

    const Score &getScore() const;

    int getSystemIndex() const;
    void setSystemIndex(int system);
    const System &getSystem() const;

    int getStaffIndex() const;
    void setStaffIndex(int staff);
    const Staff &getStaff() const;

    int getPositionIndex() const;
    void setPositionIndex(int position);

    int getSelectionStart() const;
    void setSelectionStart(int position);

    int getVoice() const;
    void setVoice(int voice);

    int getString() const;
    void setString(int string);

private:
    const Score &myScore;
    int mySystemIndex;
    int myStaffIndex;
    int myPositionIndex;
    /// The initial location of the selection. This isn't necessarily less than
    /// the myPositionIndex value.
    int mySelectionStart;
    int myVoice;
    int myString;
};

#endif
