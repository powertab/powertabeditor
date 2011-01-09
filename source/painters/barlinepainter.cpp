#include "barlinepainter.h"

#include <QPainter>
#include <QPen>
#include <QStringBuilder>

#include "../dialogs/barlinedialog.h"
#include "../powertabdocument/barline.h"

const double BarlinePainter::DOUBLE_BAR_WIDTH = 4;

BarlinePainter::BarlinePainter(StaffData staffInformation, Barline* barLinePtr)
{
    staffInfo = staffInformation;
    barLine = barLinePtr;
}

void BarlinePainter::init()
{
    width = 1;
    x = 0;

    if (barLine->IsBar())
    {
        width = 1;
    }
    else if (barLine->IsRepeatStart())
    {
        width = -(DOUBLE_BAR_WIDTH);
    }
    else
    {
        width = DOUBLE_BAR_WIDTH;
    }

    x = CenterItem(x, x + staffInfo.positionWidth, width);

    // adjust alignment for repeat barlines
    if (barLine->IsRepeatEnd() || barLine->IsRepeatStart())
    {
        x += width;
    }
    // adjust for double barlines
    if (barLine->IsDoubleBar() || barLine->IsDoubleBarFine())
    {
        x -= 2;
    }
}

void BarlinePainter::mousePressEvent(QGraphicsSceneMouseEvent *)
{
}

void BarlinePainter::mouseReleaseEvent(QGraphicsSceneMouseEvent *)
{
    BarlineDialog dialog(barLine);
    dialog.exec();
}

void BarlinePainter::mouseMoveEvent(QGraphicsSceneMouseEvent *)
{
}

QRectF BarlinePainter::boundingRect() const
{
    return QRectF(0,
                  0,
                  staffInfo.positionWidth,
                  staffInfo.getBottomTabLine() - staffInfo.getTopStdNotationLine());
}

void BarlinePainter::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

    init();

    painter->setPen(QPen(Qt::black, 0.75)); // thin line
    painter->setBrush(Qt::black);

    if (barLine->IsFreeTimeBar())
    {
        painter->setPen(Qt::DashLine);
    }

    // print the repeat count for repeat end bars
    if (barLine->IsRepeatEnd())
    {
        QFont repeatFont("Liberation Sans");
        repeatFont.setPixelSize(8.5);
        painter->setFont(repeatFont);

        const QString message = QString().setNum(barLine->GetRepeatCount()) % "x";
        painter->drawText(3, -3, message);
    }

    QVector<QLine> lines(2);

    // draw a single bar line
    lines[0] = QLine(x, 1,
                     x, staffInfo.getStdNotationStaffSize());
    // we are drawing relative to the top of the standard notation staff
    lines[1] = QLine(x, staffInfo.getTopTabLine() - staffInfo.getTopStdNotationLine() + 1,
                     x, staffInfo.getBottomTabLine() - staffInfo.getTopStdNotationLine());

    painter->drawLines(lines);

    // draw a second line depending on the bar type
    if (barLine->IsDoubleBar() || barLine->IsDoubleBarFine() || barLine->IsRepeatEnd() || barLine->IsRepeatStart())
    {
        if (barLine->IsDoubleBarFine() || barLine->IsRepeatEnd() || barLine->IsRepeatStart())
        {
            painter->setPen(QPen(Qt::black, 2)); // make the line thicker for certain bar types
        }

        // draw the second barline with an offset of the specified width
        lines[0] = QLine (x + width, 1,
                          x + width, staffInfo.getStdNotationStaffSize());
        // we are drawing relative to the top of the standard notation staff
        lines[1] = QLine (x + width, staffInfo.getTopTabLine() - staffInfo.getTopStdNotationLine() + 1,
                          x + width, staffInfo.getBottomTabLine() - staffInfo.getTopStdNotationLine());

        painter->drawLines(lines);
    }

    // draw the dots for repeats
    if (barLine->IsRepeatEnd() || barLine->IsRepeatStart())
    {
        painter->setPen(QPen(Qt::black, 0.75));
        const double radius = 1.2;

        double height = 0, centre = 0;

        if (staffInfo.numOfStdNotationLines % 2 != 0)
        {
            centre = (int)(staffInfo.numOfStdNotationLines / 2) + 1;
        }
        height = (staffInfo.getStdNotationLineHeight(centre) + staffInfo.getStdNotationLineHeight(centre + 1)) / 2 + 0.5;
        painter->drawEllipse(QPointF((double)(x - 1.5*width), (double)(height - (staffInfo.getTopStdNotationLine()))), radius, radius);
        height = (staffInfo.getStdNotationLineHeight(centre) + staffInfo.getStdNotationLineHeight(centre - 1)) / 2 + 0.5;
        painter->drawEllipse(QPointF((double)(x - 1.5*width), (double)(height - (staffInfo.getTopStdNotationLine()))), radius, radius);

        centre = height = 0;
        if (staffInfo.numOfStrings % 2 != 0)
        {
            centre = (int)(staffInfo.numOfStrings / 2) + 1;
        }
        else
        {
            centre = staffInfo.numOfStrings / 2;
        }

        height = (staffInfo.getTabLineHeight(centre + 1) + staffInfo.getTabLineHeight(centre + 2)) / 2 + 0.5;
        painter->drawEllipse(QPointF((double)(x - 1.5*width), (double)(height - (staffInfo.getTopStdNotationLine()))), radius, radius);
        height = (staffInfo.getTabLineHeight(centre) + staffInfo.getTabLineHeight(centre - 1)) / 2 + 0.5;
        painter->drawEllipse(QPointF((double)(x - 1.5*width), (double)(height - (staffInfo.getTopStdNotationLine()))), radius, radius);
    }

}

