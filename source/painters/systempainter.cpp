#include "systempainter.h"

#include <powertabdocument/system.h>

#include <QPainter>

SystemPainter::SystemPainter(std::shared_ptr<const System> system) :
    system(system)
{
    init();
}

void SystemPainter::init()
{
    Rect rect = system->GetRect();

    bounds = QRectF(0, 0, rect.GetWidth(), rect.GetHeight());

    path.addRect(bounds);
}

void SystemPainter::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

    painter->setOpacity(0.5);
    painter->setPen(QPen(QBrush(QColor(0,0,0)), 0.5));

    painter->drawPath(path);
}
