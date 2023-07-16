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

#include "chorddiagrampainter.h"

#include "clickableitem.h"
#include "layoutinfo.h"
#include "simpletextitem.h"

#include <QCoreApplication>
#include <QCursor>
#include <QGraphicsSceneMouseEvent>
#include <QPainter>
#include <score/chorddiagram.h>
#include <score/score.h>

static constexpr double DIAGRAM_WIDTH = 48;
static constexpr double DIAGRAM_HEIGHT = 55;
static constexpr double LEFT_PAD = 5;
static constexpr double TOP_FRET_PAD = 3;
static constexpr double TOP_FRET_Y = 22;

/// Size of the fret marker dots.
static constexpr double DOT_RADIUS = 1.5;
/// Hit radius for clicking on fret markers.
static constexpr double HIT_RADIUS = 2.5;

/// Height of the diagram, in frets.
static constexpr int NUM_FRETS = 6;
/// Spacing between frets.
static constexpr double FRET_SPACING = 6.0;
/// Spacing between strings.
static constexpr double STRING_SPACING = FRET_SPACING;

/// Y position for drawing open / muted strings above the nut.
const double Y_ABOVE = TOP_FRET_Y - 0.6 * FRET_SPACING;

ChordDiagramPainter::ChordDiagramPainter(const ChordDiagram &diagram,
                                         const QColor &color)
    : myBounds(0, 0, DIAGRAM_WIDTH, DIAGRAM_HEIGHT),
      myColor(color),
      myDiagram(diagram)
{
    setAcceptHoverEvents(true);
}

void
ChordDiagramPainter::paint(QPainter *painter, const QStyleOptionGraphicsItem *,
                           QWidget *)
{
    const QPen default_pen(myColor, 1.0);
    const QPen thick_pen(myColor, 2.0);

    painter->setRenderHint(QPainter::Antialiasing);
    painter->setPen(default_pen);

    const int num_strings = myDiagram.getStringCount();

    // Draw horizontal lines for each fret.
    for (int i = 0; i < NUM_FRETS; ++i)
    {
        const double y = TOP_FRET_Y + i * FRET_SPACING;

        // Draw the nut a bit thicker.
        if (i == 0 && myDiagram.getTopFret() == 0)
            painter->setPen(thick_pen);
        else
            painter->setPen(default_pen);

        painter->drawLine(QLineF(
            LEFT_PAD, y, LEFT_PAD + (num_strings - 1) * STRING_SPACING, y));
    }

    // Draw a line for each string, and marker dots for the chord's frets.
    for (int i = 0; i < num_strings; ++i)
    {
        const double x = LEFT_PAD + i * STRING_SPACING;
        painter->drawLine(QLineF(x, TOP_FRET_Y, x,
                                 TOP_FRET_Y + (NUM_FRETS - 1) * FRET_SPACING));

        const int fret = myDiagram.getFretNumbers()[num_strings - i - 1];
        if (fret < 0) // Muted string.
        {
            painter->drawLine(QLineF(x - DOT_RADIUS, Y_ABOVE - DOT_RADIUS,
                                     x + DOT_RADIUS, Y_ABOVE + DOT_RADIUS));
            painter->drawLine(QLineF(x + DOT_RADIUS, Y_ABOVE - DOT_RADIUS,
                                     x - DOT_RADIUS, Y_ABOVE + DOT_RADIUS));
        }
        else if (fret == 0) // Open string.
        {
            painter->drawEllipse(QPointF(x, Y_ABOVE), DOT_RADIUS, DOT_RADIUS);
        }
        else
        {
            const int fret_offset = fret - myDiagram.getTopFret();

            painter->setBrush(QBrush(myColor));
            painter->drawEllipse(
                QPointF(x, TOP_FRET_Y + (fret_offset - 0.5) * FRET_SPACING),
                DOT_RADIUS, DOT_RADIUS);
            painter->setBrush(QBrush()); // reset
        }
    }

    // Draw top fret number.
    if (myDiagram.getTopFret() > 0)
    {
        QFont font(QStringLiteral("Liberation Sans"));
        font.setPixelSize(7);
        painter->setFont(font);

        const QString text = QString::number(myDiagram.getTopFret());
        painter->drawText(
            LEFT_PAD + (myDiagram.getStringCount() - 1) * STRING_SPACING +
                TOP_FRET_PAD,
            TOP_FRET_Y + 0.5 * FRET_SPACING, text);
    }
}

bool
ChordDiagramPainter::findFretAtPosition(const QPointF &pos, int &string,
                                        int &fret) const
{
    const int num_strings = myDiagram.getStringCount();
    for (int i = 0; i < NUM_FRETS; ++i)
    {
        const double y =
            (i == 0) ? Y_ABOVE : (TOP_FRET_Y + (i - 0.5) * FRET_SPACING);

        for (int j = 0; j < num_strings; ++j)
        {
            const double x = LEFT_PAD + j * STRING_SPACING;

            if ((pos - QPointF(x, y)).manhattanLength() < HIT_RADIUS)
            {
                fret = i;
                // Strings are right to left in a chord diagram.
                string = num_strings - j - 1;
                return true;
            }
        }
    }

    return false;
}

void
ChordDiagramPainter::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    QGraphicsItem::mouseReleaseEvent(event);
    // Do nothing: this is just needed to get mouseReleaseEvent() to be fired.
}

void
ChordDiagramPainter::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    QGraphicsItem::mouseReleaseEvent(event);

    int string = -1;
    int fret = -1;
    if (findFretAtPosition(event->pos(), string, fret))
        emit clicked(string, fret);
}

void
ChordDiagramPainter::hoverMoveEvent(QGraphicsSceneHoverEvent *event)
{
    int string = -1;
    int fret = -1;
    if (findFretAtPosition(event->pos(), string, fret))
        setCursor(Qt::PointingHandCursor);
    else
        unsetCursor();

    QGraphicsItem::hoverMoveEvent(event);
}

void
ChordDiagramPainter::hoverLeaveEvent(QGraphicsSceneHoverEvent *event)
{
    unsetCursor();
    QGraphicsItem::hoverLeaveEvent(event);
}

/// Empty graphics item to be the parent for the chord diagrams, since they
/// aren't invidually selectable if they're part of a QGraphicsItemGroup.
class EmptyGraphicsItem : public QGraphicsItem
{
public:
    void paint(QPainter *, const QStyleOptionGraphicsItem *, QWidget *) override
    {
        // Do nothing
    }

    QRectF boundingRect() const override
    {
        return myBounds;
    }

    QRectF myBounds = QRectF(0, 0, 0, 0);
};

QGraphicsItem *
ChordDiagramPainter::renderDiagrams(const Score &score, const QColor &color,
                                    const ScoreClickEvent &click_event,
                                    double max_width)
{
    auto diagram_list = new EmptyGraphicsItem();

    QFont font(QStringLiteral("Liberation Sans"));
    font.setPixelSize(10);
    font.setStyleStrategy(QFont::PreferAntialias);

    // Layout horizontally, shifting to a new line when necessary.
    static constexpr double SPACING = 6;
    std::vector<QGraphicsItem *> current_line_items;
    double x = 0;
    double y = 0;

    int chord_idx = 0;
    for (const ChordDiagram &diagram : score.getChordDiagrams())
    {
        // TODO - identify the diagram index.
        ConstScoreLocation location(score);
        location.setChordDiagramIndex(chord_idx);

        auto group = new ClickableGroup(
            QCoreApplication::translate("ScoreArea",
                                        "Double-click to edit chord diagram."),
            click_event, location, ScoreItem::ChordDiagram);

        auto diagram_item = new ChordDiagramPainter(diagram, color);
        const QSizeF size = diagram_item->boundingRect().size();
        group->addToGroup(diagram_item);

        // Overlay the chord name, using SimpleTextItem to keep the
        // ChordDiagramPainter simpler.
        const std::string text = diagram.getChordName().getDescription();
        auto text_item = new SimpleTextItem(QString::fromStdString(text), font,
                                            TextAlignment::Top, QPen(color));
        text_item->setPos(
            0.5 * (size.width() - text_item->boundingRect().width()), 0);
        group->addToGroup(text_item);

        // Layout the diagrams.
        if (x + size.width() > max_width)
        {
            x = 0;
            y += size.height() + SPACING;
            current_line_items.clear();
        }

        group->setPos(x, y);
        group->setParentItem(diagram_list);
        current_line_items.push_back(group);

        x += size.width() + SPACING;
        ++chord_idx;
    }

    // Center the final line.
    if (!current_line_items.empty())
    {
        const double total_width = x - SPACING;
        const double offset = LayoutInfo::centerItem(0, max_width, total_width);

        for (QGraphicsItem *item : current_line_items)
            item->setX(item->x() + offset);
    }

    diagram_list->myBounds.setCoords(0, 0, max_width, y + DIAGRAM_HEIGHT);
    return diagram_list;
}
