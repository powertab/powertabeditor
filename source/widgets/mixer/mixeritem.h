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

#ifndef WIDGETS_MIXERITEM_H
#define WIDGETS_MIXERITEM_H

#include <QWidget>
#include <score/tuning.h>

namespace Ui
{
class MixerItem;
}

class Player;
class TuningDictionary;

class MixerItem : public QWidget
{
    Q_OBJECT

public:
    explicit MixerItem(QWidget *parent, int playerIndex, const Player &player,
                       const TuningDictionary &dictionary);
    ~MixerItem();

signals:
    void playerEdited(const Player &player, bool undoable);
    void playerRemoved();

private:
    void onPlayerNameEdited();
    void editTuning();
    void onEdited(bool undoable);

    Ui::MixerItem *ui;
    const TuningDictionary &myDictionary;
    Tuning myTuning;
};

#endif
