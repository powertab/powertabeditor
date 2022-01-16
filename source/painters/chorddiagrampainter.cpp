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

#include "layoutinfo.h"
#include "simpletextitem.h"

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
            int fret_offset = fret;
            if (myDiagram.getTopFret() > 0)
            {
                // A top fret of 1 displays the same as 0, just without drawing
                // the nut.
                fret_offset -= myDiagram.getTopFret() - 1;
            }

            painter->setBrush(QBrush(myColor));
            painter->drawEllipse(
                QPointF(x, top_fret_y + (fret_offset - 0.5) * fret_spacing),
                dot_radius, dot_radius);
            painter->setBrush(QBrush()); // reset

            // TODO - also draw the top fret number.
        }
    }

    // Draw top fret number.
    if (myDiagram.getTopFret() > 0)
    {
        QFont font(QStringLiteral("Liberation Sans"));
        font.setPixelSize(7);
        painter->setFont(font);

        const QString text = QString::number(myDiagram.getTopFret());
        painter->drawText(DIAGRAM_WIDTH - x_pad + 3, top_fret_y + fret_spacing,
                          text);
    }
}

QGraphicsItem *
ChordDiagramPainter::renderDiagrams(const Score &score, const QColor &color,
                                    double max_width)
{
    auto group = new QGraphicsItemGroup();

    QFont font(QStringLiteral("Liberation Sans"));
    font.setPixelSize(10);
    font.setStyleStrategy(QFont::PreferAntialias);

    // Layout horizontally, shifting to a new line when necessary.
    static constexpr double SPACING = 6;
    std::vector<QGraphicsItem *> current_line_items;
    double x = 0;
    double y = 0;
    for (const ChordDiagram &diagram : score.getChordDiagrams())
    {
        auto item = new ChordDiagramPainter(diagram, color);

        const QSizeF size = item->boundingRect().size();
        if (x > max_width)
        {
            x = 0;
            y += size.height() + SPACING;
            current_line_items.clear();
        }

        item->setPos(x, y);
        group->addToGroup(item);
        current_line_items.push_back(item);

        // Overlay the chord name, using SimpleTextItem to keep the
        // ChordDiagramPainter simpler.
        const std::string text = Util::toString(diagram.getChordName());
        auto text_item = new SimpleTextItem(QString::fromStdString(text), font,
                                            TextAlignment::Top, QPen(color));
        text_item->setPos(
            x + 0.5 * (size.width() - text_item->boundingRect().width()), y);
        group->addToGroup(text_item);
        current_line_items.push_back(text_item);

        x += size.width() + SPACING;
    }

    // Center the final line.
    if (!current_line_items.empty())
    {
        const double total_width = x - SPACING;
        const double offset = LayoutInfo::centerItem(0, max_width, total_width);

        for (QGraphicsItem *item : current_line_items)
            item->setX(item->x() + offset);
    }

    return group;
}
