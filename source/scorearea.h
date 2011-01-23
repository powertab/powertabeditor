#ifndef SCOREAREA_H
#define SCOREAREA_H

#include <QGraphicsView>
#include <QGraphicsScene>
#include <QMultiMap>

#include <musicfont.h>

class StaffData;
class Staff;
class System;
class Score;
class PowerTabDocument;
class TimeSignature;
class Caret;
class Note;
class StdNotationPainter;

// The visual display of the score

class ScoreArea : public QGraphicsView
{
    Q_OBJECT

public:
    explicit ScoreArea(QWidget *parent = 0);

    void renderDocument(PowerTabDocument* doc);
    void renderDocument();

    PowerTabDocument* document;

    inline Caret* getCaret()
    {
        return caret;
    }

protected:
    void renderScore(Score* score, int lineSpacing);
    void renderSystem(System* system, int lineSpacing);
    void drawTabNotes(System* system, Staff* staff, const StaffData& currentStaffInfo);
    void renderBars(const StaffData& currentStaffInfo, System* system);
    void drawTabClef(int x, const StaffData& staffInfo);
    void drawChordText(System* system, const StaffData& currentStaffInfo);
    void drawLegato(System* system, Staff* staff, const StaffData& currentStaffInfo);
    void drawComplexSymbolText(Staff* staff, const StaffData& currentStaffInfo, Note* note, const int x);
    void drawSlides(System* system, Staff* staff, const StaffData& currentStaffInfo);
    void drawStdNotation(System* system, Staff* staff, const StaffData& currentStaffInfo);
    void adjustAccidentals(QMultiMap<double, StdNotationPainter*>& accidentalsMap);

    QGraphicsScene scene;
    MusicFont musicFont;
    void centerItem(QGraphicsItem* item, float xmin, float xmax, float y);
    Caret* caret;

public slots:
    void adjustScroll();

};

#endif // SCOREAREA_H
