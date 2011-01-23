#ifndef STDNOTATIONPAINTER_H
#define STDNOTATIONPAINTER_H

#include "painterbase.h"
#include "staffdata.h"

class Position;
class Tuning;
class QPainter;
class Note;
class KeySignature;

class StdNotationPainter : public PainterBase
{
public:
    StdNotationPainter(const StaffData& staffInfo, Position* position, Note* note, Tuning* tuning, KeySignature* keySignature);

    QRectF boundingRect() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

    enum AccidentalType
    {
        NO_ACCIDENTAL,
        NATURAL,
        SHARP,
        DOUBLE_SHARP,
        FLAT,
        DOUBLE_FLAT,
    };

    inline double getYLocation() const { return yLocation; }
    inline Position* getPositionObject() const { return position; }

    int accidental;

protected:
    int getDisplayPosition(const QString& noteName);
    void drawRest(QPainter* painter);
    int getOctaveDiff(const Note* currentNote, const int pitch) const;
    void init();
    int findAccidentalType(const QString& noteText) const;
    QString getAccidentalText() const;

    StaffData staffInfo;
    Position* position;
    Note* note;
    Tuning* tuning;
    KeySignature* keySignature;
    static QFont musicFont;
    double yLocation;
    double width;
};

#endif // STDNOTATIONPAINTER_H
