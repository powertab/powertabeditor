#ifndef SYSTEMPAINTER_H
#define SYSTEMPAINTER_H

#include "painterbase.h"
#include <memory>

class System;

class SystemPainter : public PainterBase
{
public:
    SystemPainter(std::shared_ptr<System> system);

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

protected:
    std::shared_ptr<System> system;

    void init();
};

#endif // SYSTEMPAINTER_H
