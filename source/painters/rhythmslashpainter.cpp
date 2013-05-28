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
  
#include "rhythmslashpainter.h"

#include <powertabdocument/rhythmslash.h>
#include <powertabdocument/system.h>
#include <QPainter>
#include <painters/musicfont.h>

const double RhythmSlashPainter::STEM_OFFSET = 3.0;
const double RhythmSlashPainter::NOTE_HEAD_OFFSET = System::RHYTHM_SLASH_SPACING / 2.0;

RhythmSlashPainter::RhythmSlashPainter(boost::shared_ptr<const RhythmSlash> rhythmSlash) :
    rhythmSlash(rhythmSlash), musicFont(MusicFont().getFont())
{
    bounds = QRectF(0, 0, musicFont.pixelSize(), System::RHYTHM_SLASH_SPACING);
}

void RhythmSlashPainter::paint(QPainter* painter, const QStyleOptionGraphicsItem*, QWidget*)
{
    // adjustment for the width difference between the two rhythm slash symbols
    if (rhythmSlash->GetDurationType() >= 4)
    {
        musicFont.setStretch(100);
    }
    else
    {
        musicFont.setStretch(77);
    }

    painter->setFont(musicFont);

    // draw the note flag
    {
        const double y = System::RHYTHM_SLASH_SPACING / 1.5;
        const double x = -(musicFont.pixelSize() / 4.0);

        if (rhythmSlash->GetDurationType() >= 4)
        {
            painter->drawText(x, y, MusicFont::getSymbol(MusicFont::RhythmSlashFilled));
        }
        else
        {
            painter->drawText(x - 1, y, MusicFont::getSymbol(MusicFont::RhythmSlashNoFill));
        }
    }

    // draw note stem
    painter->drawLine(STEM_OFFSET, 0, STEM_OFFSET, NOTE_HEAD_OFFSET);
}
