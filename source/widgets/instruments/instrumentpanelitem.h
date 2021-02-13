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

#ifndef WIDGETS_INSTRUMENTPANELITEM_H
#define WIDGETS_INSTRUMENTPANELITEM_H

#include <QWidget>

namespace Ui
{
class InstrumentPanelItem;
}

class Instrument;

class InstrumentPanelItem : public QWidget
{
    Q_OBJECT

public:
    explicit InstrumentPanelItem(QWidget *parent, int instrumentIndex,
                                 const Instrument &instrument);
    ~InstrumentPanelItem();

    void update(const Instrument &instrument);

signals:
    void instrumentEdited(const Instrument &instrument);
    void instrumentRemoved();

private:
    void onInstrumentNameEdited();
    void onEdited();

    Ui::InstrumentPanelItem *ui;
    const int myInstrumentIndex;
};

#endif
