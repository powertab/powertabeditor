#ifndef TEMPOMARKERPAINTER_H
#define TEMPOMARKERPAINTER_H

#include "painterbase.h"

#include <QFont>

class TempoMarker;

class TempoMarkerPainter : public PainterBase
{
public:
    TempoMarkerPainter(TempoMarker* tempoMarker);

    QRectF boundingRect() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

protected:
    TempoMarker* tempoMarker;
    static QFont displayFont;
    QString displayText;
    QRectF boundingRectangle;

    void setDisplayText();
};

#endif // TEMPOMARKERPAINTER_H
