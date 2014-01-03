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

#ifndef WIDGETS_INSTRUMENTPANEL_H
#define WIDGETS_INSTRUMENTPANEL_H

#include <QWidget>

class InstrumentPubSub;
class QVBoxLayout;
class Score;

class InstrumentPanel : public QWidget
{
public:
    InstrumentPanel(QWidget *parent, const InstrumentPubSub &pubsub);

    /// Clear and then populate the instrument panel.
    void reset(const Score &score);

    /// Removes all items from the panel.
    void clear();

private:
    QVBoxLayout *myLayout;
    const InstrumentPubSub &myPubSub;
};

#endif
