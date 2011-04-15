#ifndef CHORDTEXTPAINTER_H
#define CHORDTEXTPAINTER_H

#include "painterbase.h"

#include <QFont>
#include <QStaticText>

class ChordText;

class ChordTextPainter : public PainterBase
{
public:
    ChordTextPainter(ChordText* chordText);

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

protected:
    void init();

    ChordText* chordText;
    static QFont displayFont;
    QStaticText displayText;
};

#endif // CHORDTEXTPAINTER_H
