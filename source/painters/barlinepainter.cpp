#include "barlinepainter.h"

#include <QPainter>
#include <QPen>
#include <QStringBuilder>
#include <QGraphicsSceneMouseEvent>

#include <powertabdocument/barline.h>

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

    x = centerItem(x, x + staffInfo.positionWidth, width);

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

void BarlinePainter::mousePressEvent(QGraphicsSceneMouseEvent* event)
{
    qreal y = event->pos().y();

    // only handle clicks that occur in the standard notation staff
    if (y > staffInfo.getBottomStdNotationLine() || y < staffInfo.getTopStdNotationLine())
    {
        event->ignore();
    }
}

void BarlinePainter::mouseReleaseEvent(QGraphicsSceneMouseEvent*)
{
    emit clicked(barLine->GetPosition());
}

QRectF BarlinePainter::boundingRect() const
{
    return QRectF(0,
                  0,
                  staffInfo.positionWidth,
                  staffInfo.height);
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
        painter->drawText(3, staffInfo.getTopStdNotationLine() - 3, message);
    }

    // draw a single bar line
    drawVerticalLines(painter, x);

    // draw a second line depending on the bar type
    if (barLine->IsDoubleBar() || barLine->IsDoubleBarFine() || barLine->IsRepeatEnd() || barLine->IsRepeatStart())
    {
        if (barLine->IsDoubleBarFine() || barLine->IsRepeatEnd() || barLine->IsRepeatStart())
        {
            painter->setPen(QPen(Qt::black, 2)); // make the line thicker for certain bar types
        }

        // draw the second barline with an offset of the specified width
        drawVerticalLines(painter, x + width);
    }

    // draw the dots for repeats
    if (barLine->IsRepeatEnd() || barLine->IsRepeatStart())
    {
        painter->setPen(QPen(Qt::black, 0.75));
        const double radius = 1.0;

        double height = 0, centre = 0;

        if (staffInfo.numOfStdNotationLines % 2 != 0)
        {
            centre = (int)(staffInfo.numOfStdNotationLines / 2) + 1;
        }

        height = (staffInfo.getStdNotationLineHeight(centre) + staffInfo.getStdNotationLineHeight(centre + 1)) / 2 + 0.5;
        painter->drawRect(QRectF(x - 1.5*width, height, radius, radius));
        height = (staffInfo.getStdNotationLineHeight(centre) + staffInfo.getStdNotationLineHeight(centre - 1)) / 2 + 0.5;
        painter->drawRect(QRectF(x - 1.5*width, height, radius, radius));

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
        painter->drawRect(QRectF(x - 1.5*width, height, radius, radius));
        height = (staffInfo.getTabLineHeight(centre) + staffInfo.getTabLineHeight(centre - 1)) / 2 + 0.5;
        painter->drawRect(QRectF(x - 1.5*width, height, radius, radius));
    }

}

void BarlinePainter::drawVerticalLines(QPainter* painter, double x)
{
    QVector<QLine> lines(2);

    // draw a single bar line
    lines[0] = QLine(x, staffInfo.getTopStdNotationLine() + 1,
                     x, staffInfo.getBottomStdNotationLine());
    lines[1] = QLine(x, staffInfo.getTopTabLine() + 1,
                     x, staffInfo.getBottomTabLine());

    painter->drawLines(lines);
}
