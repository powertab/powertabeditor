/*
  * Copyright (C) 2022 Cameron White
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

#ifndef PAINTERS_CHORDDIAGRAMPAINTER_H
#define PAINTERS_CHORDDIAGRAMPAINTER_H

#include <QColor>
#include <QGraphicsItem>

class ChordDiagram;
class Score;
class ScoreClickEvent;

/// Renders a single chord diagram.
/// Use the static renderDiagrams() method to layout and render all of the
/// diagrams in a score.
class ChordDiagramPainter : public QGraphicsObject
{
    Q_OBJECT

public:
    ChordDiagramPainter(const ChordDiagram &diagram, const QColor &color);

    QRectF boundingRect() const override { return myBounds; }
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *,
               QWidget *) override;

    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;
    void hoverMoveEvent(QGraphicsSceneHoverEvent *event) override;
    void hoverLeaveEvent(QGraphicsSceneHoverEvent *event) override;

    /// Returns a group of all the chord diagrams, constrained to the provided
    /// width.
    static QGraphicsItem *renderDiagrams(const Score &score,
                                         const QColor &color,
                                         const ScoreClickEvent &click_event,
                                         double max_width);
signals:
    void clicked(int string, int fret);

private:
    bool findFretAtPosition(const QPointF &pos, int &string, int &fret) const;

    const QRectF myBounds;
    const QColor myColor;
    const ChordDiagram &myDiagram;
    double myXPad = 0;
};

#endif
