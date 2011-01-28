#include "systempainter.h"

#include <powertabdocument/system.h>

#include <QPainter>

SystemPainter::SystemPainter(System* system) :
    system(system)
{
}

QRectF SystemPainter::boundingRect() const
{
    Rect rect = system->GetRect();

    return QRectF(0, 0, rect.GetWidth(), rect.GetHeight());
}

void SystemPainter::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

    painter->setOpacity(0.5);
    painter->setPen(QPen(QBrush(QColor(0,0,0)), 0.5));

    painter->drawRect(boundingRect());
}
