#ifndef TREMOLOBARPAINTER_H
#define TREMOLOBARPAINTER_H

#include "painterbase.h"
#include <QFont>

class Position;

class TremoloBarPainter : public PainterBase
{
public:
    TremoloBarPainter(const Position* position, double width);

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

private:
    const Position* position;
    double width;

    static QFont textFont;
};

#endif // TREMOLOBARPAINTER_H
