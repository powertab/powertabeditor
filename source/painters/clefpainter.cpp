#include "clefpainter.h"

#include "staffdata.h"
#include "../powertabdocument/staff.h"
#include "../musicfont.h"

#include <QPainter>
#include <QFontMetricsF>

ClefPainter::ClefPainter(const StaffData& staffData, Staff* staff) :
        staffInfo(staffData)
{
    this->staff = staff;
}

void ClefPainter::mousePressEvent(QGraphicsSceneMouseEvent *)
{
}

void ClefPainter::mouseReleaseEvent(QGraphicsSceneMouseEvent *)
{
    // on mouse click, switch clef type from treble to bass, or vice versa
    if (staff->GetClef() == Staff::TREBLE_CLEF)
    {
        staff->SetClef(Staff::BASS_CLEF);
    }
    else
    {
        staff->SetClef(Staff::TREBLE_CLEF);
    }

    // redraw clef
    update(boundingRect());
}

void ClefPainter::mouseMoveEvent(QGraphicsSceneMouseEvent *)
{
}

QRectF ClefPainter::boundingRect() const
{
    MusicFont musicFont;
    QFontMetricsF fm(musicFont.getFont());
    return QRectF(0,
                  -10, // adjust for the extra height of treble clef above the staff
                  fm.width(MusicFont::TrebleClef),
                  fm.height());
}

void ClefPainter::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

    MusicFont musicFont;
    painter->setFont(musicFont.getFont());

    // draw the correct staff type
    if (staff->GetClef() == Staff::TREBLE_CLEF)
    {
        painter->drawText(0,
                          staffInfo.getStdNotationStaffSize() - 5,
                          musicFont.getSymbol(MusicFont::TrebleClef));
    }
    // Draw a bass clef otherwise
    else
    {
        painter->drawText(0,
                          staffInfo.getStdNotationLineHeight(2) - staffInfo.getTopStdNotationLine(),
                          musicFont.getSymbol(MusicFont::BassClef));
    }

}
