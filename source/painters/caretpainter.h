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

private:
    void onLocationChanged();

    const Caret &myCaret;
    boost::scoped_ptr<LayoutInfo> myLayout;
    std::vector<QRectF> mySystemRects;

    static const double PEN_WIDTH;
    /// Spacing around a highlighted note.
    static const double CARET_NOTE_SPACING;

#if 0
    std::vector<Position*> getSelectedPositions() const;
    std::vector<Note*> getSelectedNotes() const;
    std::vector<boost::shared_ptr<Barline> > getSelectedBarlines() const;
    bool hasSelection() const;

    QString toString() const;

signals:
    void moved();
    void selectionChanged();

public slots:
    void updateSelection(int start, int end);

private:
    void updateStaffInfo();

    bool inPlaybackMode;

    Score* currentScore;
    uint32_t currentSystemIndex;
    uint32_t currentStaffIndex;
    uint32_t currentStringIndex;
    uint32_t currentPositionIndex;

    uint32_t currentVoice;

    uint32_t currentStaffTopEdge;
    int lineSpacing;
    StaffData currentStaffInfo;

    std::pair<int, int> selectionRange;
#endif
};

#endif // CARET_H
