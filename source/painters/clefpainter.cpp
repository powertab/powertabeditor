#include "clefpainter.h"

#include "staffdata.h"
#include <powertabdocument/staff.h>
#include <painters/musicfont.h>

#include <QPainter>
#include <QFontMetricsF>

QFont ClefPainter::font = MusicFont().getFont();

ClefPainter::ClefPainter(const StaffData& staffData, std::shared_ptr<const Staff> staff) :
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
