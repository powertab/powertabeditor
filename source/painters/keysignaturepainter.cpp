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
  
#include "keysignaturepainter.h"

#include <app/pubsub/clickpubsub.h>
#include <painters/musicfont.h>
#include <QCursor>
#include <QPainter>
#include <score/keysignature.h>

KeySignaturePainter::KeySignaturePainter(
    const LayoutConstPtr &layout, const KeySignature &key,
    const ScoreLocation &location, const std::shared_ptr<ClickPubSub> &pubsub)
    : myLayout(layout),
      myKeySignature(key),
      myLocation(location),
      myPubSub(pubsub),
      myMusicFont(MusicFont::getFont(MusicFont::DEFAULT_FONT_SIZE)),
      myBounds(0, -10, LayoutInfo::getWidth(myKeySignature),
               layout->getStdNotationStaffHeight())
{
    setAcceptHoverEvents(true);
    setToolTip(QObject::tr("Click to edit key signature."));
    initAccidentalPositions();
}

void KeySignaturePainter::mousePressEvent(QGraphicsSceneMouseEvent *)
{
    // No action is needed here, but this will let us grab future mouse events.
}

void KeySignaturePainter::mouseReleaseEvent(QGraphicsSceneMouseEvent *)
{
    myPubSub->publish(ClickType::KeySignature, myLocation);
}

void KeySignaturePainter::hoverEnterEvent(QGraphicsSceneHoverEvent *)
{
    setCursor(Qt::PointingHandCursor);
}

void KeySignaturePainter::hoverLeaveEvent(QGraphicsSceneHoverEvent *)
{
    unsetCursor();
}

void KeySignaturePainter::paint(QPainter *painter,
                                const QStyleOptionGraphicsItem*, QWidget*)
{
    painter->setFont(myMusicFont);

    // Draw the appropriate accidentals.
    if (myKeySignature.usesSharps())
        drawAccidentals(mySharpPositions, MusicFont::AccidentalSharp, painter);
    else
        drawAccidentals(myFlatPositions, MusicFont::AccidentalFlat, painter);
}

void KeySignaturePainter::adjustHeightOffset(QVector<double> &lst)
{
    for (auto &elem : lst)
    {
        elem -= myLayout->getTopStdNotationLine();
    }
}

void KeySignaturePainter::drawAccidentals(QVector<double> &positions,
                                          QChar accidental,
                                          QPainter *painter)
{
    // Display natural if a cancellation occurs.
    if (myKeySignature.isCancellation())
        accidental = MusicFont::Natural;

    for (int i = 0; i < myKeySignature.getNumAccidentals(true); ++i)
    {
        painter->drawText(i * LayoutInfo::ACCIDENTAL_WIDTH,
                          positions.at(i), accidental);
    }
}

void KeySignaturePainter::initAccidentalPositions()
{
    myFlatPositions.resize(7);
    mySharpPositions.resize(7);

    // Generate the positions for the key signature accidentals.
    myFlatPositions.replace(0, myLayout->getStdNotationLine(3));
    myFlatPositions.replace(1, myLayout->getStdNotationSpace(1));
    myFlatPositions.replace(2, myLayout->getStdNotationSpace(3));
    myFlatPositions.replace(3, myLayout->getStdNotationLine(2));
    myFlatPositions.replace(4, myLayout->getStdNotationLine(4));
    myFlatPositions.replace(5, myLayout->getStdNotationSpace(2));
    myFlatPositions.replace(6, myLayout->getStdNotationSpace(4));

    mySharpPositions.replace(0, myLayout->getStdNotationLine(1));
    mySharpPositions.replace(1, myLayout->getStdNotationSpace(2));
    mySharpPositions.replace(2, myLayout->getStdNotationSpace(0));
    mySharpPositions.replace(3, myLayout->getStdNotationLine(2));
    mySharpPositions.replace(4, myLayout->getStdNotationSpace(3));
    mySharpPositions.replace(5, myLayout->getStdNotationSpace(1));
    mySharpPositions.replace(6, myLayout->getStdNotationLine(3));

    adjustHeightOffset(myFlatPositions);
    adjustHeightOffset(mySharpPositions);
}
