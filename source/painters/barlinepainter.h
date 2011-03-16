#ifndef BARLINEPAINTER_H
#define BARLINEPAINTER_H

#include "painterbase.h"
#include "staffdata.h"

class Barline;

class BarlinePainter : public QObject, public PainterBase
{
    Q_OBJECT

public:
    BarlinePainter(StaffData staffInformation, Barline* barLinePtr);

    QRectF boundingRect() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

signals:
    void clicked(int);

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);

    void init();
    void drawVerticalLines(QPainter* painter, double x);

    StaffData staffInfo;
    Barline* barLine;
    double x;
    double width;

    static const double DOUBLE_BAR_WIDTH;
};

#endif // BARLINEPAINTER_H
