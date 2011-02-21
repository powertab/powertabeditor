#ifndef SCOREAREA_H
#define SCOREAREA_H

#include <QGraphicsView>
#include <QGraphicsScene>
#include <QMultiMap>
#include <memory>

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
class Position;
class TempoMarker;

// The visual display of the score

class ScoreArea : public QGraphicsView
{
    Q_OBJECT

public:
    explicit ScoreArea(QWidget *parent = 0);

    void renderDocument(std::shared_ptr<PowerTabDocument> doc);
    void updateSystem(const quint32 systemIndex);

    std::shared_ptr<PowerTabDocument> document;

    inline Caret* getCaret()
    {
        return caret;
    }

protected:
    void renderScore(Score* score, int lineSpacing);
    void renderSystem(Score* score, System* system, int lineSpacing);
    void drawTabNotes(System* system, Staff* staff, const StaffData& currentStaffInfo);
    void renderBars(const StaffData& currentStaffInfo, System* system);
    void drawTabClef(int x, const StaffData& staffInfo);
    void drawChordText(System* system, quint32 height, const StaffData& currentStaffInfo);
    void drawLegato(System* system, Staff* staff, const StaffData& currentStaffInfo);
    void drawComplexSymbolText(Staff* staff, const StaffData& currentStaffInfo, Note* note, const int x);
    void drawSlides(System* system, Staff* staff, const StaffData& currentStaffInfo);
    void drawStdNotation(System* system, Staff* staff, const StaffData& currentStaffInfo);
    void adjustAccidentals(QMultiMap<double, StdNotationPainter*>& accidentalsMap);
    void drawSystemSymbols(Score* score, System* system, const StaffData& currentStaffInfo);
    void drawDividerLine(const StaffData& currentStaffInfo, quint32 y);
    void drawTempoMarkers(std::vector<TempoMarker*> tempoMarkers, System* system, quint32 height);
    void drawFermata(const StaffData& currentStaffInfo, double x, double beamConnectorHeight, bool beamDirectionUp);
    void drawSymbols(System* system, Staff* staff, const StaffData& currentStaffInfo);

    QGraphicsScene scene;
    MusicFont musicFont;
    void centerItem(QGraphicsItem* item, float xmin, float xmax, float y);
    Caret* caret;
    QList<QGraphicsItem*> systemList;

    // holds the parent object of the staff that is being drawn
    // all QGraphicsItems that are a part of the staff have this object as their parent
    QGraphicsItem* activeStaff;
    QGraphicsItem* activeSystem;

    struct BeamingInfo
    {
        Position* position;
        double topNotePos;
        double bottomNotePos;
        bool beamUp;
        double location;

        BeamingInfo()
        {
            position = NULL;
            beamUp = false;
            bottomNotePos = -100000;
            topNotePos = 100000; // needs to be larger than any possible position, for using std::min
        }
    };

    // Symbol drawing
    struct SymbolInfo
    {
        QRect rect; ///< the bounding rectangle of the symbol group
        QGraphicsItem* symbol; ///< contains the information for rendering the symbol
    };

    QGraphicsItem* createPalmMute(uint8_t width, const StaffData& currentStaffInfo);
    QGraphicsItem* createLetRing(uint8_t width, const StaffData& currentStaffInfo);
    QGraphicsItem* createNaturalHarmonic(uint8_t width, const StaffData& currentStaffInfo);
    QGraphicsItem* createArtificialHarmonic(uint8_t width, const StaffData& currentStaffInfo);
    QGraphicsItem* createVibrato(uint8_t width, const StaffData& currentStaffInfo);
    QGraphicsItem* createWideVibrato(uint8_t width, const StaffData& currentStaffInfo);
    QGraphicsItem* createTrill(uint8_t width, const StaffData& currentStaffInfo);
    QGraphicsItem* createTremoloPicking(uint8_t width, const StaffData& currentStaffInfo);

    QGraphicsItem* drawContinuousFontSymbols(QChar symbol, uint8_t width, const StaffData& currentStaffInfo);
    QGraphicsItem* createConnectedSymbolGroup(const QString& text, uint8_t width,
                                              const StaffData& currentStaffInfo, QFont::Style style = QFont::StyleNormal);

public slots:
    void adjustScroll();

};

#endif // SCOREAREA_H
