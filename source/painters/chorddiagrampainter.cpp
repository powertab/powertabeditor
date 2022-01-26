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
#include <QPainter>
#include <score/chorddiagram.h>
#include <score/score.h>
#include <util/tostring.h>

static constexpr double DIAGRAM_WIDTH = 46;
static constexpr double DIAGRAM_HEIGHT = 55;

ChordDiagramPainter::ChordDiagramPainter(const ChordDiagram &diagram,
                                         const QColor &color)
    : myBounds(0, 0, DIAGRAM_WIDTH, DIAGRAM_HEIGHT),
      myColor(color),
      myDiagram(diagram)
{
}

void
ChordDiagramPainter::paint(QPainter *painter, const QStyleOptionGraphicsItem *,
                           QWidget *)
{
    const QPen default_pen(myColor, 1.0);
    const QPen thick_pen(myColor, 2.0);

    painter->setRenderHint(QPainter::Antialiasing);
    painter->setPen(default_pen);

    static constexpr double top_fret_y = 22;
    static constexpr double fret_spacing = 6;
    static constexpr double string_spacing = fret_spacing;

    static constexpr int num_frets = 6;
    const int num_strings = myDiagram.getStringCount();
    const double x_pad =
        (DIAGRAM_WIDTH - fret_spacing * (num_strings - 1)) * 0.5;

    // Draw horizontal lines for each fret.
    for (int i = 0; i < num_frets; ++i)
    {
        const double y = top_fret_y + i * fret_spacing;

        // Draw the nut a bit thicker.
        if (i == 0 && myDiagram.getTopFret() == 0)
            painter->setPen(thick_pen);
        else
            painter->setPen(default_pen);

        painter->drawLine(
            QLineF(x_pad, y, x_pad + (num_strings - 1) * string_spacing, y));
    }

    // Draw a line for each string, and marker dots for the chord's frets.
    for (int i = 0; i < num_strings; ++i)
    {
        const double x = x_pad + i * string_spacing;
        painter->drawLine(QLineF(x, top_fret_y, x,
                                 top_fret_y + (num_frets - 1) * fret_spacing));

        // Size of the fret marker dots.
        static constexpr double dot_radius = 1.5;
        // Y position for drawing open / muted strings above the nut.
        const double y_above = top_fret_y - 0.6 * fret_spacing;

        const int fret = myDiagram.getFretNumbers()[num_strings - i - 1];
        if (fret < 0) // Muted string.
        {
            painter->drawLine(QLineF(x - dot_radius, y_above - dot_radius,
                                     x + dot_radius, y_above + dot_radius));
            painter->drawLine(QLineF(x + dot_radius, y_above - dot_radius,
                                     x - dot_radius, y_above + dot_radius));
        }
        else if (fret == 0) // Open string.
        {
            painter->drawEllipse(QPointF(x, y_above), dot_radius, dot_radius);
        }
        else
        {
            const int fret_offset = fret - myDiagram.getTopFret();

            painter->setBrush(QBrush(myColor));
            painter->drawEllipse(
                QPointF(x, top_fret_y + (fret_offset - 0.5) * fret_spacing),
                dot_radius, dot_radius);
            painter->setBrush(QBrush()); // reset
        }
    }

    // Draw top fret number.
    if (myDiagram.getTopFret() > 0)
    {
        QFont font(QStringLiteral("Liberation Sans"));
        font.setPixelSize(7);
        painter->setFont(font);

        // Adding 1 since the next fret is where a finger can be placed.
        const QString text = QString::number(myDiagram.getTopFret() + 1);
        painter->drawText(DIAGRAM_WIDTH - x_pad + 3, top_fret_y + fret_spacing,
                          text);
    }
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
        const std::string text = Util::toString(diagram.getChordName());
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
