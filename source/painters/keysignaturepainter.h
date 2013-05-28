/*
  * Copyright (C) 2011 Cameron White
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
  
#ifndef KEYSIGNATUREPAINTER_H
#define KEYSIGNATUREPAINTER_H

#include "painterbase.h"
#include <QFont>
#include <QVector>
#include <boost/cstdint.hpp>
#include <boost/shared_ptr.hpp>
#include <powertabdocument/systemlocation.h>

class StaffData;
class SystemLocationPubSub;
class KeySignature;

class KeySignaturePainter : public PainterBase
{
public:
    KeySignaturePainter(const StaffData& staffInformation, const KeySignature& signature,
                        const SystemLocation& location,
                        boost::shared_ptr<SystemLocationPubSub> pubsub);

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

private:
    void mousePressEvent(QGraphicsSceneMouseEvent *event);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);

    const StaffData& staffInfo;
    const KeySignature& keySignature;
    const SystemLocation location;
    boost::shared_ptr<SystemLocationPubSub> pubsub;

    QFont musicFont;

    QVector<double> flatPositions;
    QVector<double> sharpPositions;

    void adjustHeightOffset(QVector<double>& lst);
    void drawAccidentals(QVector<double>& positions, QChar accidental, QPainter* painter);
    void initAccidentalPositions();
    void init();
};

#endif // KEYSIGNATUREPAINTER_H
