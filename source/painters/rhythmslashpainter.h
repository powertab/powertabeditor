#ifndef RHYTHMSLASHPAINTER_H
#define RHYTHMSLASHPAINTER_H

#include "painterbase.h"
#include <QFont>

class RhythmSlash;

class RhythmSlashPainter : public PainterBase
{
public:
    RhythmSlashPainter(const RhythmSlash* rhythmSlash);

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

private:
    const RhythmSlash* rhythmSlash;
    static QFont musicFont;
};

#endif // RHYTHMSLASHPAINTER_H
