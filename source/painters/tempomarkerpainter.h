#ifndef TEMPOMARKERPAINTER_H
#define TEMPOMARKERPAINTER_H

#include "painterbase.h"

#include <memory>
#include <QFont>
#include <QStaticText>

class TempoMarker;

class TempoMarkerPainter : public PainterBase
{
public:
    TempoMarkerPainter(std::shared_ptr<const TempoMarker> tempoMarker);

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

protected:
    std::shared_ptr<const TempoMarker> tempoMarker;
    static QFont displayFont;
    QStaticText displayText;

    void init();
};

#endif // TEMPOMARKERPAINTER_H
