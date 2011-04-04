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
    StdNotationPainter(const StaffData& staffInfo, const Position* position, const Note* note,
                       const Tuning* tuning, const KeySignature* keySignature);

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
    inline const Position* getPositionObject() const { return position; }

    static double getNoteHeadWidth();

    int accidental;

protected:
    int getDisplayPosition(const QString& noteName);
    void drawRest(QPainter* painter);
    int getOctaveDiff(const Note* currentNote, const int pitch) const;
    void init();
    int findAccidentalType(const QString& noteText) const;
    QString getAccidentalText() const;
    void addDots(QPainter* painter, double x, double y) const;

    StaffData staffInfo;
    const Position* position;
    const Note* note;
    const Tuning* tuning;
    const KeySignature* keySignature;
    static QFont musicFont;
    double yLocation;
    double width;
};

#endif // STDNOTATIONPAINTER_H
