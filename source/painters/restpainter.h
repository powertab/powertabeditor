#ifndef RESTPAINTER_H
#define RESTPAINTER_H

#include <QGraphicsItem>
#include <QStaticText>

class Position;

class RestPainter : public QGraphicsItem
{
public:
    RestPainter(const Position& position, QGraphicsItem* parent = 0);

    QRectF boundingRect() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

private:
    void init();

    const Position& position;

    QStaticText textToDraw; /// store the rest symbol that will be drawn
    static const QFont musicFont;
    QRectF bounds;
    int restHeight;
};

#endif // RESTPAINTER_H
