#ifndef SYSTEMPAINTER_H
#define SYSTEMPAINTER_H

#include "painterbase.h"

class System;

class SystemPainter : public PainterBase
{
public:
    SystemPainter(System* system);

    QRectF boundingRect() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

protected:
    System* system;

};

#endif // SYSTEMPAINTER_H
