#ifndef RHYTHMSLASHPAINTER_H
#define RHYTHMSLASHPAINTER_H

#include "painterbase.h"
#include <QFont>
#include <memory>

class RhythmSlash;

class RhythmSlashPainter : public PainterBase
{
public:
    RhythmSlashPainter(std::shared_ptr<const RhythmSlash> rhythmSlash);

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

private:
    std::shared_ptr<const RhythmSlash> rhythmSlash;
    static QFont musicFont;
};

#endif // RHYTHMSLASHPAINTER_H
