#ifndef STDNOTATIONPAINTER_H
#define STDNOTATIONPAINTER_H

#include "painterbase.h"
#include "staffdata.h"

class Position;
class Guitar;
class QPainter;
class Note;

class StdNotationPainter : public PainterBase
{
public:
    StdNotationPainter(const StaffData& staffInfo, Position* position, Guitar* guitar);

    QRectF boundingRect() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event);
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);

    int getDisplayPosition(const QString& noteName);
    void drawRest(QPainter* painter);
    int getOctaveDiff(const Note* currentNote, const int pitch) const;

    StaffData staffInfo;
    Position* position;
    Guitar* guitar;
    static QFont musicFont;
};

#endif // STDNOTATIONPAINTER_H
