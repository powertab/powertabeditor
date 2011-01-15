#ifndef SCOREAREA_H
#define SCOREAREA_H

#include <QGraphicsView>
#include <QGraphicsScene>

#include <musicfont.h>

class StaffData;
class Staff;
class System;
class Score;
class PowerTabDocument;
class TimeSignature;
class Caret;
class Note;

// The visual display of the score

class ScoreArea : public QGraphicsView
{
    Q_OBJECT

public:
    explicit ScoreArea(QWidget *parent = 0);

    void RenderDocument(PowerTabDocument* doc);
    void RenderDocument();

    PowerTabDocument* document;

    Caret* getCaret()
    {
        return caret;
    }

protected:
    void RenderScore(Score* score, int lineSpacing);
    void RenderSystem(System* system, int lineSpacing);
    void DrawStaff(int leftEdge, int currentHeight, int lineSpacing, int width, int numberOfLines);
    void DrawTabNotes(System* system, Staff* staff, const StaffData& currentStaffInfo);
    void RenderStaffLines(Staff* staff, StaffData& currentStaffInfo, int lineSpacing, int width, QPointF& position);
    void RenderBars(const StaffData& currentStaffInfo, System* system);
    void DrawTabClef(int x, const StaffData& staffInfo);
    void DrawChordText(System* system, const StaffData& currentStaffInfo);
    void drawLegato(System* system, Staff* staff, const StaffData& currentStaffInfo);
    void drawComplexSymbolText(Staff* staff, const StaffData& currentStaffInfo, Note* note, const int x);

private:
    QGraphicsScene scene;
    MusicFont musicFont;
    void CenterItem(QGraphicsItem* item, float xmin, float xmax, float y);
    Caret* caret;

public slots:
    void adjustScroll();

};

#endif // SCOREAREA_H
