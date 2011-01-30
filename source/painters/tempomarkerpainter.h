#ifndef TEMPOMARKERPAINTER_H
#define TEMPOMARKERPAINTER_H

#include "painterbase.h"

#include <QFont>
#include <QStaticText>

class TempoMarker;

class TempoMarkerPainter : public PainterBase
{
public:
    TempoMarkerPainter(TempoMarker* tempoMarker);

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

protected:
    TempoMarker* tempoMarker;
    static QFont displayFont;
    QStaticText displayText;

    void init();
};

#endif // TEMPOMARKERPAINTER_H
