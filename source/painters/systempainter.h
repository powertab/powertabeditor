#ifndef SYSTEMPAINTER_H
#define SYSTEMPAINTER_H

#include "painterbase.h"

class System;

class SystemPainter : public PainterBase
{
public:
    SystemPainter(System* system);

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

protected:
    System* system;

    void init();
};

#endif // SYSTEMPAINTER_H
