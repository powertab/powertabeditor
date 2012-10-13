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
  
#include "timesignaturepainter.h"

#include <app/pubsub/systemlocationpubsub.h>
#include <powertabdocument/timesignature.h>
#include <powertabdocument/staff.h>
#include <painters/musicfont.h>

#include <QMessageBox>
#include <QFont>
#include <QPainter>

TimeSignaturePainter::TimeSignaturePainter(const StaffData& staffInformation,
                                           const TimeSignature& signature,
                                           const SystemLocation &location,
                                           boost::shared_ptr<SystemLocationPubSub> pubsub) :
    staffInfo(staffInformation),
    timeSignature(signature),
    location(location),
    pubsub(pubsub)
{
    init();
}

void TimeSignaturePainter::init()
{
    bounds = QRectF(0, 0, timeSignature.GetWidth(), staffInfo.getStdNotationStaffSize());
}

void TimeSignaturePainter::mousePressEvent(QGraphicsSceneMouseEvent *)
{
}

void TimeSignaturePainter::mouseReleaseEvent(QGraphicsSceneMouseEvent *)
{
    pubsub->publish(location);
}

void TimeSignaturePainter::mouseMoveEvent(QGraphicsSceneMouseEvent *)
{
}

void TimeSignaturePainter::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

    if (timeSignature.IsCommonTime() || timeSignature.IsCutTime())
    {
        MusicFont musicFont;
        QFont displayFont = musicFont.getFont();
        displayFont.setPixelSize(25);
        painter->setFont(displayFont);

        QChar symbol;
        if (timeSignature.IsCommonTime())
        {
            symbol = musicFont.getSymbol(MusicFont::CommonTime);
        }
        else
        {
            symbol = musicFont.getSymbol(MusicFont::CutTime);
        }

        painter->drawText(0, 2 * Staff::STD_NOTATION_LINE_SPACING, symbol);
    }
    else
    {
        drawNumber(painter, 2 * Staff::STD_NOTATION_LINE_SPACING, timeSignature.GetBeatsPerMeasure());
        drawNumber(painter, 4 * Staff::STD_NOTATION_LINE_SPACING, timeSignature.GetBeatAmount());
    }
}

void TimeSignaturePainter::drawNumber(QPainter* painter, const double y, const quint8 number) const
{
    QString text = QString().setNum(number);

    MusicFont musicFont;
    QFont displayFont = musicFont.getFont();
    displayFont.setPixelSize(27);

    const double width = QFontMetricsF(displayFont).width(text);
    const double x = centerItem(0, timeSignature.GetWidth(), width);

    painter->setFont(displayFont);
    painter->drawText(x, y, text);
}
