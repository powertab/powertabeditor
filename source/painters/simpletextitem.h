/*
  * Copyright (C) 2014 Cameron White
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
  
#ifndef PAINTERS_SIMPLETEXTITEM_H
#define PAINTERS_SIMPLETEXTITEM_H

#include <QBrush>
#include <QFont>
#include <QGraphicsItem>
#include <QPen>

/// Specifies the text alignment for SimpleTextItem.
enum class TextAlignment
{
    /// Matches the way that QSimpleTextItem aligns text. The ascent is at y=0
    /// in local space.
    Top,
    /// The font's baseline is at y=0 in local space. This is particularly
    /// useful for the Emmentaler font, where the baseline usually lines up
    /// with where the symbol should be placed on e.g. a staff line.
    Baseline
};

/// Replacement for QGraphicsSimpleTextItem, which is significantly faster but
/// doesn't handle things like multi-line text.
class SimpleTextItem : public QGraphicsItem
{
public:
    SimpleTextItem(const QString &text, const QFont &font,
                   TextAlignment alignment, const QPen &pen = QPen(),
                   const QBrush &background = QBrush(QColor(0, 0, 0, 0)));

    virtual QRectF boundingRect() const override { return myBoundingRect; }

    virtual void paint(QPainter *painter,
                       const QStyleOptionGraphicsItem *option,
                       QWidget *widget) override;

private:
    const QString myText;
    const QFont myFont;
    const QPen myPen;
    const QBrush myBackground;
    const TextAlignment myAlignment;
    QRectF myBoundingRect;
    double myAscent;
};

#endif
