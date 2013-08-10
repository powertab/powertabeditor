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
  
#ifndef PAINTERS_CARETPAINTER_H
#define PAINTERS_CARETPAINTER_H

#include <boost/scoped_ptr.hpp>
#include <QGraphicsItem>
#include <boost/signals2/signal.hpp>

class Caret;
struct LayoutInfo;

class CaretPainter : public QGraphicsItem
{
public:
    CaretPainter(const Caret &caret);

    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *,
                       QWidget *);
    virtual QRectF boundingRect() const;

    void addSystemRect(const QRectF &rect);
    void setSystemRect(int index, const QRectF &rect);

    typedef boost::signals2::signal<void ()> LocationChangedSlot;
    /// Register a listener for when the caret painter is updated.
    boost::signals2::connection subscribeToMovement(
            const LocationChangedSlot::slot_type &subscriber);

private:
    /// Redraw the caret painter whenever the caret moves.
    void onLocationChanged();

    const Caret &myCaret;
    boost::scoped_ptr<LayoutInfo> myLayout;
    std::vector<QRectF> mySystemRects;
    boost::signals2::scoped_connection myCaretConnection;
    LocationChangedSlot onMyLocationChanged;

    static const double PEN_WIDTH;
    /// Spacing around a highlighted note.
    static const double CARET_NOTE_SPACING;
};

#endif // CARET_H
