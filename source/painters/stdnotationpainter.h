#ifndef STDNOTATIONPAINTER_H
#define STDNOTATIONPAINTER_H

#include "painterbase.h"
#include "staffdata.h"
#include <memory>

class Position;
class Tuning;
class QPainter;
class Note;
class KeySignature;
class Staff;

class StdNotationPainter : public PainterBase
{
public:
    StdNotationPainter(const StaffData& staffInfo, std::shared_ptr<const Staff> staff, const Position* position, const Note* note,
                       const Tuning& tuning, const KeySignature* keySignature);

    QRectF boundingRect() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

    enum AccidentalType
    {
        NO_ACCIDENTAL,
        NATURAL,
        SHARP,
        DOUBLE_SHARP,
        FLAT,
        DOUBLE_FLAT
    };

    inline double getYLocation() const { return yLocation; }
    inline const Position* getPositionObject() const { return position; }

    static double getNoteHeadWidth();

    int accidental;

protected:
    void drawRest(QPainter* painter);
    void init();
    int findAccidentalType(const QString& noteText) const;
    QString getAccidentalText() const;
    void addDots(QPainter* painter, double x, double y) const;

    StaffData staffInfo;
    std::shared_ptr<const Staff> staff;
    const Position* position;
    const Note* note;
    const Tuning& tuning;
    const KeySignature* keySignature;
    static QFont musicFont;
    double yLocation;
    double width;
};

#endif // STDNOTATIONPAINTER_H
