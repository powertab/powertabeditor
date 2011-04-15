#ifndef DIRECTIONPAINTER_H
#define DIRECTIONPAINTER_H

#include "painterbase.h"
#include <QFont>
#include <QStaticText>

class Direction;

class DirectionPainter : public PainterBase
{
public:
    DirectionPainter(const Direction* direction, size_t symbolIndex);

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

private:
    void init();

    static QFont displayFont;
    QStaticText displayText;

    const Direction* direction;
    const size_t symbolIndex;
};

#endif // DIRECTIONPAINTER_H
