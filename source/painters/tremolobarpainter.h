#ifndef TREMOLOBARPAINTER_H
#define TREMOLOBARPAINTER_H

#include "painterbase.h"
#include <QFont>

class Position;

class TremoloBarPainter : public PainterBase
{
public:
    TremoloBarPainter(const Position* position, uint8_t numPositions, uint8_t positionWidth);

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

private:
    const Position* position;
    const double width;
    const uint8_t positionWidth;

    static QFont textFont;
};

#endif // TREMOLOBARPAINTER_H
