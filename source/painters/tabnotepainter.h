#ifndef TABNOTEPAINTER_H
#define TABNOTEPAINTER_H

#include "painterbase.h"

#include <QFont>
#include <QStaticText>

class Note;

class TabNotePainter : public PainterBase
{
public:
    TabNotePainter(Note* note);

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

protected:
    void init();

    Note* note;
    Qt::GlobalColor textColor;
    static QFont tabFont;
    QStaticText displayText;
};

#endif // TABNOTEPAINTER_H
