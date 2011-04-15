#ifndef CHORDTEXTPAINTER_H
#define CHORDTEXTPAINTER_H

#include "painterbase.h"

#include <QFont>
#include <QStaticText>

class ChordText;

class ChordTextPainter : public PainterBase
{
public:
    ChordTextPainter(const ChordText* chordText);

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

protected:
    void init();

    const ChordText* chordText;
    static QFont displayFont;
    QStaticText displayText;
};

#endif // CHORDTEXTPAINTER_H
