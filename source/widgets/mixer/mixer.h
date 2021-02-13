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

#ifndef WIDGETS_MIXER_H
#define WIDGETS_MIXER_H

#include <QWidget>

class Player;
class QVBoxLayout;
class Score;
class TuningDictionary;

class Mixer : public QWidget
{
    Q_OBJECT

public:
    Mixer(QWidget *parent, const TuningDictionary &dictionary);

    /// Clear and then populate the mixer.
    void reset(const Score &score);

    /// Removes all items from the mixer.
    void clear();

signals:
    void playerEdited(int index, const Player &player, bool undoable);
    void playerRemoved(int index);

private:
    QVBoxLayout *myLayout;
    const TuningDictionary &myDictionary;
};

#endif
