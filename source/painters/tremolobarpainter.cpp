#include "tremolobarpainter.h"

#include <powertabdocument/position.h>
#include <powertabdocument/staff.h>
#include <QPainter>
#include <QFontMetricsF>

QFont TremoloBarPainter::textFont = QFont("Liberation Sans");

TremoloBarPainter::TremoloBarPainter(const Position* position, uint8_t numPositions, uint8_t positionWidth) :
    position(position),
    width(numPositions * positionWidth),
    positionWidth(positionWidth)
{
    textFont.setPixelSize(9.5);
}

void TremoloBarPainter::paint(QPainter* painter, const QStyleOptionGraphicsItem*, QWidget*)
{
    Q_ASSERT(position->HasTremoloBar());

    uint8_t type = 0, duration = 0, pitch = 0;
    position->GetTremoloBar(type, duration, pitch);

    const QString displayText = QString::fromStdString(position->GetTremoloBarText());

    if (type == Position::dip || type == Position::invertedDip)
    {
        const double topHeight = Staff::TAB_SYMBOL_HEIGHT / 2.0;
        const double bottomHeight = Staff::TAB_SYMBOL_HEIGHT;
        const double middleX = positionWidth / 2.0;

        if (type == Position::dip)
        {
            painter->drawLine(0, topHeight, middleX, bottomHeight);
            painter->drawLine(middleX, bottomHeight, positionWidth, topHeight);
        }
        else // invertedDip
        {
            painter->drawLine(0, bottomHeight, middleX, topHeight);
            painter->drawLine(middleX, topHeight, positionWidth, bottomHeight);
        }

        painter->setFont(textFont);
        painter->drawText(centerItem(0, positionWidth, QFontMetricsF(textFont).width(displayText)),
                          topHeight - 2, displayText);
    }
}
