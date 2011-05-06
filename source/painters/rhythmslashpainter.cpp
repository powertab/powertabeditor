#include "rhythmslashpainter.h"

#include <powertabdocument/rhythmslash.h>
#include <powertabdocument/system.h>
#include <QPainter>
#include <musicfont.h>

const double RhythmSlashPainter::NOTE_HEAD_OFFSET = System::RHYTHM_SLASH_SPACING / 2.0;

QFont RhythmSlashPainter::musicFont = MusicFont().getFont();

RhythmSlashPainter::RhythmSlashPainter(std::shared_ptr<const RhythmSlash> rhythmSlash) :
    rhythmSlash(rhythmSlash)
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
