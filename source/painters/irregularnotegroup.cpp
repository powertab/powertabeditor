#include "irregularnotegroup.h"

#include <QGraphicsItem>
#include <QBrush>
#include <QPen>

#include <algorithm>
#include <powertabdocument/position.h>
#include <musicfont.h>

IrregularNoteGroup::IrregularNoteGroup(const std::vector<NoteStem>& noteStems) :
    noteStems(noteStems)
{
}

void IrregularNoteGroup::draw(QGraphicsItem* parent)
{
    const NoteStem::StemDirection direction = findDirectionForGroup(noteStems);

    double y1 = 0, y2 = 0;

    if (direction == NoteStem::StemUp)
    {
        NoteStem highestStem = *std::min_element(noteStems.begin(), noteStems.end(),
                                                 CompareStemPositions(&NoteStem::top));

        y1 = y2 = highestStem.top() - GROUP_SPACING;
        y2 -= GROUP_HEIGHT;
    }
    else
    {
        NoteStem lowestStem = *std::max_element(noteStems.begin(), noteStems.end(),
                                                CompareStemPositions(&NoteStem::bottom));

        y1 = y2 = lowestStem.bottom() + GROUP_SPACING;
        y2 += GROUP_HEIGHT;
    }

    const double leftX = noteStems.front().x();
    const double rightX = noteStems.back().x();

    // draw the value of the irregular grouping
    const QString text = QString().fromStdString(noteStems.front().position()->GetIrregularGroupingText());
    const double centreX = (leftX + rightX) / 2.0 - 2.0;

    QFont font = MusicFont().getFont();
    font.setItalic(true);
    font.setPixelSize(18);

    QGraphicsSimpleTextItem* textItem = new QGraphicsSimpleTextItem(text);
    textItem->setPos(centreX, y2 - font.pixelSize());
    textItem->setFont(font);
    textItem->setParentItem(parent);

    const double textWidth = font.pixelSize() * text.length();

    // draw the two horizontal line segments across the group, and the two vertical lines on either end
    QGraphicsLineItem* horizLine1 = new QGraphicsLineItem;
    horizLine1->setLine(leftX, y2, centreX - textWidth / 4, y2);
    horizLine1->setParentItem(parent);

    QGraphicsLineItem* horizLine2 = new QGraphicsLineItem;
    horizLine2->setLine(centreX + textWidth * 0.75, y2, rightX, y2);
    horizLine2->setParentItem(parent);

    QGraphicsLineItem* vertLine1 = new QGraphicsLineItem;
    vertLine1->setLine(leftX, y1, leftX, y2);
    vertLine1->setParentItem(parent);

    QGraphicsLineItem* vertLine2 = new QGraphicsLineItem;
    vertLine2->setLine(rightX, y1, rightX, y2);
    vertLine2->setParentItem(parent);
}
