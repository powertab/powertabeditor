#include "timesignaturepainter.h"

#include "staffdata.h"
#include <powertabdocument/timesignature.h>
#include <musicfont.h>

#include <QMessageBox>
#include <QFont>
#include <QPainter>

TimeSignaturePainter::TimeSignaturePainter(const StaffData& staffInformation, const TimeSignature& signature) :
        staffInfo(staffInformation),
        timeSignature(signature)
{
}

void TimeSignaturePainter::mousePressEvent(QGraphicsSceneMouseEvent *)
{
}

void TimeSignaturePainter::mouseReleaseEvent(QGraphicsSceneMouseEvent *)
{
    QMessageBox message;
    message.setText("Time Signature");
    message.exec();
}

void TimeSignaturePainter::mouseMoveEvent(QGraphicsSceneMouseEvent *)
{
}

QRectF TimeSignaturePainter::boundingRect() const
{
    return QRectF(0, 0, timeSignature.GetWidth(), staffInfo.getStdNotationStaffSize());
}

void TimeSignaturePainter::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

    MusicFont musicFont;
    QFont displayFont = musicFont.getFont();
    if (timeSignature.IsCommonTime() || timeSignature.IsCutTime())
    {
        displayFont.setPixelSize(25);
    }
    else
    {
        displayFont.setPixelSize(27);
    }
    painter->setFont(displayFont);

    if (timeSignature.IsCommonTime())
    {
        painter->drawText(0, staffInfo.getStdNotationLineHeight(3) - staffInfo.getTopStdNotationLine(),
                          musicFont.getSymbol(MusicFont::CommonTime));
    }
    else if (timeSignature.IsCutTime())
    {
        painter->drawText(0, staffInfo.getStdNotationLineHeight(3) - staffInfo.getTopStdNotationLine(),
                          musicFont.getSymbol(MusicFont::CutTime));
    }
    else
    {
        const int offset = -5;
        uint8_t beatsPerMeasure = timeSignature.GetBeatsPerMeasure();
        uint8_t beatAmount = timeSignature.GetBeatAmount();

        painter->drawText(beatsPerMeasure <= 10 ? 0 : offset,
                          staffInfo.getStdNotationLineHeight(3) - staffInfo.getTopStdNotationLine(),
                          QString().setNum(beatsPerMeasure));

        painter->drawText(beatAmount <= 10 ? 0 : offset,
                          staffInfo.getStdNotationLineHeight(5) - staffInfo.getTopStdNotationLine(),
                          QString().setNum(beatAmount));
    }
}
