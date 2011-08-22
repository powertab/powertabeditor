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
#include <QVector>
#include <boost/cstdint.hpp>

class StaffData;
class KeySignature;

class KeySignaturePainter : public QObject, public PainterBase
{
    Q_OBJECT
public:
    KeySignaturePainter(const StaffData& staffInformation, const KeySignature& signature,
                        uint32_t position);

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

signals:
    void clicked(int);

private:
    void mousePressEvent(QGraphicsSceneMouseEvent *event);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);

    const StaffData& staffInfo;
    const KeySignature& keySignature;
    const uint32_t position;
    static QFont musicFont;
    void adjustHeightOffset(QVector<double>& lst);
    void drawAccidentals(QVector<double>& positions, QChar accidental, QPainter* painter);
    void initAccidentalPositions();
    void init();

    QVector<double> flatPositions;
    QVector<double> sharpPositions;
};

#endif // KEYSIGNATUREPAINTER_H
