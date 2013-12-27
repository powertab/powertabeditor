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

#include "instrumentpanel.h"

#include "instrumentpanelitem.h"
#include <QVBoxLayout>
#include <score/score.h>

InstrumentPanel::InstrumentPanel(QWidget *parent,
                                 const InstrumentPubSub &pubsub)
    : QWidget(parent), myPubSub(pubsub)
{
    myLayout = new QVBoxLayout(this);
    myLayout->setSpacing(0);
    myLayout->setSizeConstraint(QLayout::SetFixedSize);
    setLayout(myLayout);
}

void InstrumentPanel::reset(const Score &score)
{
    clear();

    for (int i = 0; i < score.getInstruments().size(); ++i)
    {
        myLayout->addWidget(new InstrumentPanelItem(
            this, i, score.getInstruments()[i], myPubSub));
    }
}

void InstrumentPanel::update(const Score &score)
{
    Q_ASSERT(score.getInstruments().size() == myLayout->count());

    for (int i = 0; i < score.getInstruments().size(); ++i)
    {
        static_cast<InstrumentPanelItem *>(myLayout->itemAt(i)->widget())
            ->update(score.getInstruments()[i]);
    }
}

void InstrumentPanel::clear()
{
    while (QLayoutItem *item = myLayout->takeAt(0))
    {
        // We might be clearing the instrument panel in response to a signal
        // from one of its widgets, so it's not safe to delete the widget until
        // control returns to the event loop.
        item->widget()->deleteLater();
        delete item;
    }
}
