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
  
#ifndef TABNOTEPAINTER_H
#define TABNOTEPAINTER_H

#include <QGraphicsItem>

#include <QFont>
#include <QStaticText>
#include <QFontMetrics>

class Note;

class TabNotePainter : public QGraphicsItem
{
public:
    TabNotePainter(Note* note);

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
    QRectF boundingRect() const;

protected:
    void init();

    QRectF bounds;
    Note* note;
    Qt::GlobalColor textColor;
    QFont tabFont;
    QStaticText displayText;
    QFontMetricsF fontMetrics;
};

#endif // TABNOTEPAINTER_H
