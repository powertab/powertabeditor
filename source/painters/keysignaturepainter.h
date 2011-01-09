#ifndef KEYSIGNATUREPAINTER_H
#define KEYSIGNATUREPAINTER_H

#include "painterbase.h"
#include <QVector>

class StaffData;
class KeySignature;

class KeySignaturePainter : public PainterBase
{
public:
    KeySignaturePainter(const StaffData& staffInformation, const KeySignature& signature);

    QRectF boundingRect() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event);
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);

private:
    const StaffData& staffInfo;
    const KeySignature& keySignature;
    static QFont musicFont;
    void adjustHeightOffset(QVector<double>& lst);
    void drawAccidentals(QVector<double>& positions, QChar accidental, QPainter* painter);
    void init();

    QVector<double> flatPositions;
    QVector<double> sharpPositions;
};

#endif // KEYSIGNATUREPAINTER_H
