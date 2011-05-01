#ifndef DIRECTIONPAINTER_H
#define DIRECTIONPAINTER_H

#include "painterbase.h"
#include <QFont>
#include <QStaticText>
#include <memory>

class Direction;

class DirectionPainter : public PainterBase
{
public:
    DirectionPainter(std::shared_ptr<const Direction> direction, size_t symbolIndex);

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

private:
    void init();

    QFont displayFont;
    QStaticText displayText;

    std::shared_ptr<const Direction> direction;
    const size_t symbolIndex;
    bool usingMusicFont;
};

#endif // DIRECTIONPAINTER_H
