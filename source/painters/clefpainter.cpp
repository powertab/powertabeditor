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

#include <app/pubsub/staffpubsub.h>
#include <painters/musicfont.h>
#include <QCursor>
#include <QPainter>

ClefPainter::ClefPainter(Staff::ClefType clefType, const QFont &musicFont,
                         int system, int staff,
                         boost::shared_ptr<StaffPubSub> pubsub)
    : myClefType(clefType),
      myMusicFont(musicFont),
      mySystemIndex(system),
      myStaffIndex(staff),
      myPubSub(pubsub)
{
    setAcceptHoverEvents(true);

    if (myClefType == Staff::TrebleClef)
        myDisplayText.setText(QChar(MusicFont::TrebleClef));
    else
        myDisplayText.setText(QChar(MusicFont::BassClef));

    myDisplayText.prepare(QTransform(), myMusicFont);
    QFontMetricsF fm(myMusicFont);
    myBounds = QRectF(0, -5, fm.width(MusicFont::TrebleClef), fm.height());
}

void ClefPainter::paint(QPainter *painter, const QStyleOptionGraphicsItem*,
                        QWidget*)
{
    painter->setFont(myMusicFont);

    if (myClefType == Staff::TrebleClef)
        painter->drawStaticText(0, -6, myDisplayText);
    else
        painter->drawStaticText(0, -21, myDisplayText);
}

void ClefPainter::mousePressEvent(QGraphicsSceneMouseEvent *)
{
    // No action is needed here, but we need to override this method in order
    // to get the mouse release event.
}

void ClefPainter::mouseReleaseEvent(QGraphicsSceneMouseEvent *)
{
    myPubSub->publish(mySystemIndex, myStaffIndex);
}

void ClefPainter::hoverEnterEvent(QGraphicsSceneHoverEvent *)
{
    setCursor(Qt::PointingHandCursor);
}

void ClefPainter::hoverLeaveEvent(QGraphicsSceneHoverEvent *)
{
    unsetCursor();
}
