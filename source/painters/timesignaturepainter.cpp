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

#include <painters/musicfont.h>
#include <QCoreApplication>
#include <QCursor>
#include <QPainter>
#include <score/timesignature.h>

TimeSignaturePainter::TimeSignaturePainter(const LayoutConstPtr &layout,
                                           const TimeSignature &time,
                                           const ConstScoreLocation &location,
                                           const ScoreClickEvent &click_event)
    : ClickableItem(QCoreApplication::translate(
                        "ScoreArea", "Click to edit time signature."),
                    click_event, location, ScoreItem::TimeSignature),
      myLayout(layout),
      myTimeSignature(time),
      myBounds(0, 0, LayoutInfo::getWidth(myTimeSignature),
               myLayout->getStdNotationStaffHeight())
{
}

void
TimeSignaturePainter::paint(QPainter *painter,
                            const QStyleOptionGraphicsItem *option,
                            QWidget *widget)
{
    ClickableItem::paint(painter, option, widget);

    const TimeSignature::MeterType meterType = myTimeSignature.getMeterType();
    if (meterType == TimeSignature::CommonTime ||
        meterType == TimeSignature::CutTime)
    {
        painter->setFont(MusicFont::getFont(25));

        const QChar symbol = (meterType == TimeSignature::CommonTime) ?
                    MusicFont::CommonTime : MusicFont::CutTime;
        painter->drawText(0, 2 * LayoutInfo::STD_NOTATION_LINE_SPACING, symbol);
    }
    else
    {
        drawNumber(painter, 2 * LayoutInfo::STD_NOTATION_LINE_SPACING,
                   myTimeSignature.getBeatsPerMeasure());
        drawNumber(painter, 4 * LayoutInfo::STD_NOTATION_LINE_SPACING,
                   myTimeSignature.getBeatValue());
    }
}

void TimeSignaturePainter::drawNumber(QPainter* painter, const double y,
                                      const int number) const
{
    QString text = QString::number(number);
    QFont font = MusicFont::getFont(27);

    const double width = QFontMetricsF(font).width(text);
    const double x = LayoutInfo::centerItem(0, LayoutInfo::getWidth(myTimeSignature),
                                            width);

    painter->setFont(font);
    painter->drawText(x, y, text);
}
