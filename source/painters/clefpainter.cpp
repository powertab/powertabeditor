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
  
#include "clefpainter.h"

#include "staffdata.h"
#include <powertabdocument/staff.h>
#include <painters/musicfont.h>

#include <QPainter>
#include <QFontMetricsF>

QFont ClefPainter::font = MusicFont().getFont();

ClefPainter::ClefPainter(const StaffData& staffData, boost::shared_ptr<const Staff> staff) :
        staffInfo(staffData)
{
    this->staff = staff;
    init();
}

void ClefPainter::init()
{
    // draw the correct staff type
    if (staff->GetClef() == Staff::TREBLE_CLEF)
    {
        displayText.setText(MusicFont::getSymbol(MusicFont::TrebleClef));
    }
    // Draw a bass clef otherwise
    else
    {
        displayText.setText(MusicFont::getSymbol(MusicFont::BassClef));
    }

    displayText.prepare(QTransform(), font);

    QFontMetricsF fm(font);
    bounds = QRectF(0, -5, fm.width(MusicFont::TrebleClef), fm.height());
}

void ClefPainter::mousePressEvent(QGraphicsSceneMouseEvent *)
{
}

void ClefPainter::mouseReleaseEvent(QGraphicsSceneMouseEvent *)
{
    // on mouse click, switch clef type from treble to bass, or vice versa

    // TODO - replace with proper QUndoCommand

    /*if (staff->GetClef() == Staff::TREBLE_CLEF)
    {
        staff->SetClef(Staff::BASS_CLEF);
    }
    else
    {
        staff->SetClef(Staff::TREBLE_CLEF);
    }*/

    // redraw clef
    init();
    update(boundingRect());
}

void ClefPainter::mouseMoveEvent(QGraphicsSceneMouseEvent *)
{
}

void ClefPainter::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

    painter->setFont(font);

    if (staff->GetClef() == Staff::TREBLE_CLEF)
    {
        painter->drawStaticText(0, -6, displayText);
    }
    else
    {
        painter->drawStaticText(0, -21, displayText);
    }
}
