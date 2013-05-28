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

#include "staffdata.h"
#include <app/pubsub/systemlocationpubsub.h>
#include <powertabdocument/keysignature.h>
#include <painters/musicfont.h>

#include <QPainter>
#include <QVector>

KeySignaturePainter::KeySignaturePainter(const StaffData& staffInformation,
                                         const KeySignature& signature,
                                         const SystemLocation& location,
                                         boost::shared_ptr<SystemLocationPubSub> pubsub) :
    staffInfo(staffInformation),
    keySignature(signature),
    location(location),
    pubsub(pubsub), musicFont(MusicFont().getFont())
{
    initAccidentalPositions();
    init();
}

void KeySignaturePainter::init()
{
    bounds = QRectF(0, -10, keySignature.GetWidth(), staffInfo.getStdNotationStaffSize());
}

void KeySignaturePainter::mousePressEvent(QGraphicsSceneMouseEvent *)
{
}

void KeySignaturePainter::mouseReleaseEvent(QGraphicsSceneMouseEvent *)
{
    pubsub->publish(location);
}

void KeySignaturePainter::paint(QPainter *painter, const QStyleOptionGraphicsItem*, QWidget*)
{
    painter->setFont(musicFont);

    // draw the appropriate accidentals
    if (keySignature.UsesFlats())
    {
        drawAccidentals(flatPositions, MusicFont::AccidentalFlat, painter);
    }
    else
    {
        drawAccidentals(sharpPositions, MusicFont::AccidentalSharp, painter);
    }
}

void KeySignaturePainter::adjustHeightOffset(QVector<double>& lst)
{
    for (int i = 0; i < lst.size(); i++)
    {
        lst[i] -= staffInfo.getTopStdNotationLine();
    }
}

void KeySignaturePainter::drawAccidentals(QVector<double>& positions, QChar accidental, QPainter* painter)
{
    if (keySignature.IsCancellation()) // display natural if a cancellation occurs
    {
        accidental = MusicFont::Natural; // draw using naturals
    }

    for(int i = 0; i < keySignature.GetKeyAccidentalsIncludingCancel(); i++)
    {
        painter->drawText(i * KeySignature::ACCIDENTAL_WIDTH, positions.at(i), accidental);
    }
}

void KeySignaturePainter::initAccidentalPositions()
{
    flatPositions.resize(7);
    sharpPositions.resize(7);

    // generate the positions for the key signature accidentals
    flatPositions.replace(0, staffInfo.getStdNotationLineHeight(3));
    flatPositions.replace(1, staffInfo.getStdNotationSpaceHeight(1));
    flatPositions.replace(2, staffInfo.getStdNotationSpaceHeight(3));
    flatPositions.replace(3, staffInfo.getStdNotationLineHeight(2));
    flatPositions.replace(4, staffInfo.getStdNotationLineHeight(4));
    flatPositions.replace(5, staffInfo.getStdNotationSpaceHeight(2));
    flatPositions.replace(6, staffInfo.getStdNotationSpaceHeight(4));

    sharpPositions.replace(0, staffInfo.getStdNotationLineHeight(1));
    sharpPositions.replace(1, staffInfo.getStdNotationSpaceHeight(2));
    sharpPositions.replace(2, staffInfo.getStdNotationSpaceHeight(0));
    sharpPositions.replace(3, staffInfo.getStdNotationLineHeight(2));
    sharpPositions.replace(4, staffInfo.getStdNotationSpaceHeight(3));
    sharpPositions.replace(5, staffInfo.getStdNotationSpaceHeight(1));
    sharpPositions.replace(6, staffInfo.getStdNotationLineHeight(3));

    adjustHeightOffset(flatPositions);
    adjustHeightOffset(sharpPositions);
}
