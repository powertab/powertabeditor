#ifndef SCOREAREA_H
#define SCOREAREA_H

#include <QGraphicsView>
#include <QGraphicsScene>
#include <QMultiMap>
#include <memory>

#include <musicfont.h>

class StaffData;
class Staff;
class Score;
class System;
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

    inline Caret* getCaret() const
    {
        return caret;
    }

protected:
    void renderScore(Score* score, int lineSpacing);
    void renderSystem(Score* score, std::shared_ptr<System> system, int lineSpacing);

    void drawTabNotes(std::shared_ptr<System> system, Staff* staff, const StaffData& currentStaffInfo);
    void renderBars(const StaffData& currentStaffInfo, std::shared_ptr<System> system);
    void drawTabClef(int x, const StaffData& staffInfo);

    void drawChordText(std::shared_ptr<System> system, quint32 height, const StaffData& currentStaffInfo);
    void drawDirections(std::shared_ptr<const System> system, quint32 height,
                        const StaffData& currentStaffInfo);

    void drawLegato(std::shared_ptr<System> system, Staff* staff, const StaffData& currentStaffInfo);

    void drawSlides(std::shared_ptr<System> system, Staff* staff, const StaffData& currentStaffInfo);
    void drawSlidesHelper(std::shared_ptr<System> system, const StaffData& currentStaffInfo, quint8 string,
                          bool slideUp, quint32 posIndex1, quint32 posIndex2);

    void drawStdNotation(std::shared_ptr<System> system, Staff* staff, const StaffData& currentStaffInfo);
    void adjustAccidentals(QMultiMap<double, StdNotationPainter*>& accidentalsMap);
    void drawSystemSymbols(Score* score, std::shared_ptr<System> system, const StaffData& currentStaffInfo);
    void drawDividerLine(const StaffData& currentStaffInfo, quint32 y);
    void drawTempoMarkers(std::vector<TempoMarker*> tempoMarkers, std::shared_ptr<System> system, quint32 height);
    void drawSymbolsBelowTabStaff(std::shared_ptr<System> system, Staff *staff, const StaffData &currentStaffInfo);

    void drawSymbols(std::shared_ptr<System> system, Staff* staff, const StaffData& currentStaffInfo);
    void drawArpeggio(Position* position, quint32 x, const StaffData& currentStaffInfo);

    QGraphicsScene scene;
    MusicFont musicFont;
    void centerItem(QGraphicsItem* item, float xmin, float xmax, float y);
    Caret* caret;
    QList<QGraphicsItem*> systemList;

    // holds the parent object of the staff that is being drawn
    // all QGraphicsItems that are a part of the staff have this object as their parent
    QGraphicsItem* activeStaff;
    QGraphicsItem* activeSystem;
    bool redrawOnNextRefresh;

    // Symbol drawing
    struct SymbolInfo
    {
        QRect rect; ///< the bounding rectangle of the symbol group
        QGraphicsItem* symbol; ///< contains the information for rendering the symbol
    };

    enum VolumeSwellType
    {
        VolumeIncreasing,
        VolumeDecreasing
    };

    QGraphicsItem* createVolumeSwell(uint8_t width, const StaffData& currentStaffInfo, VolumeSwellType type);

    QGraphicsItem* createTrill(uint8_t width, const StaffData& currentStaffInfo);
    QGraphicsItem* createTremoloPicking(uint8_t width, const StaffData& currentStaffInfo);

    QGraphicsItem* createPlainText(const QString& text, QFont::Style style);
    QGraphicsItem* createPickStroke(const QString& text);
    QGraphicsItem* createArtificialHarmonicText(Position* position);

    QGraphicsItem* drawContinuousFontSymbols(QChar symbol, uint8_t width, const StaffData& currentStaffInfo);
    QGraphicsItem* createConnectedSymbolGroup(const QString& text, QFont::Style style, uint8_t width,
                                              const StaffData& currentStaffInfo);

signals:
    void barlineClicked(int position);

public slots:
    void adjustScroll();
    void requestFullRedraw();

};

#endif // SCOREAREA_H
