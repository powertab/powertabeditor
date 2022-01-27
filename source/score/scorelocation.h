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

#include <iosfwd>
#include <vector>

class Barline;
class IrregularGrouping;
class Note;
class Position;
class Score;
class Staff;
class System;
class Voice;

/// Location within a const Score.
class ConstScoreLocation
{
public:
    explicit ConstScoreLocation(const Score &score, int system = 0,
                                int staff = 0, int position = 0, int voice = 0,
                                int string = 0);

    const Score &getScore() const;

    int getSystemIndex() const;
    void setSystemIndex(int system);

    const System &getSystem() const;

    const Barline *getBarline() const;

    int getStaffIndex() const;
    void setStaffIndex(int staff);

    const Staff &getStaff() const;

    int getPositionIndex() const;
    void setPositionIndex(int position);

    const Position *getPosition() const;
    const Position *findMultiBarRest() const;
    bool isEmptyBar() const;

    int getSelectionStart() const;
    void setSelectionStart(int position);
    bool hasSelection() const;
    std::vector<const Position *> getSelectedPositions() const;

    const Voice &getVoice() const;
    int getVoiceIndex() const;
    void setVoiceIndex(int voice);

    /// Returns any irregular groupings that are *entirely* contained within
    /// the selection.
    std::vector<const IrregularGrouping *> getSelectedIrregularGroupings() const;

    int getString() const;
    void setString(int string);

    const Note *getNote() const;

    /// @{
    /// Chord diagrams are global, so they can have their own selection index.
    /// This is mainly used for uniform handling of mouse events in the score
    /// view.
    int getChordDiagramIndex() const { return myChordDiagramIndex; }
    void setChordDiagramIndex(int idx) { myChordDiagramIndex = idx; }
    /// @}

protected:
    const Score &myScore;

    int mySystemIndex;
    int myStaffIndex;
    int myPositionIndex;
    /// The initial location of the selection. This isn't necessarily less than
    /// the myPositionIndex value.
    int mySelectionStart;
    int myVoiceIndex;
    int myString;
    int myChordDiagramIndex = -1;
};

/// Location within a non-const Score.
class ScoreLocation : public ConstScoreLocation
{
public:
    explicit ScoreLocation(Score &score, int system = 0, int staff = 0,
                           int position = 0, int voice = 0, int string = 0);
    /// Convert to a writeable score location.
    ScoreLocation(Score &score, const ConstScoreLocation &src_location);

    using ConstScoreLocation::getScore;
    Score &getScore();

    using ConstScoreLocation::getSystem;
    System &getSystem();

    using ConstScoreLocation::getBarline;
    Barline *getBarline();
    std::vector<Barline *> getSelectedBarlines();

    using ConstScoreLocation::getStaff;
    Staff &getStaff();

    using ConstScoreLocation::getPosition;
    Position *getPosition();
    using ConstScoreLocation::getSelectedPositions;
    std::vector<Position *> getSelectedPositions();

    using ConstScoreLocation::getVoice;
    Voice &getVoice();

    using ConstScoreLocation::getNote;
    Note *getNote();
    std::vector<Note *> getSelectedNotes();

private:
    Score &myWriteableScore;
};

std::ostream &operator<<(std::ostream &os, const ScoreLocation &location);

#endif
