/*
  * Copyright (C) 2011 Cameron White
  *
  * This program is free software: you can redistribute it and/or modify
  * it under the terms of the GNU General Public License as published by
  * the Free Software Foundation, either version 3 of the License, or
  * (at your option) any later version.
  *
  * This program is distributed in the hope that it will be useful,
  * but WITHOUT ANY WARRANTY; without even the implied warranty of
  * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  * GNU General Public License for more details.
  *
  * You should have received a copy of the GNU General Public License
  * along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef SYSTEMRENDERER_H
#define SYSTEMRENDERER_H

#include <boost/shared_ptr.hpp>
#include <boost/cstdint.hpp>
#include <vector>
#include <QMultiMap>
#include "musicfont.h"

class Score;
class System;
class QGraphicsItem;
class StaffData;
class Staff;
class Position;
class AlternateEnding;
class TempoMarker;
class Dynamic;
class StdNotationPainter;
class Barline;
class ScoreArea;
class StaffPainter;
class BarlinePainter;
class KeySignaturePainter;
class TimeSignaturePainter;

class SystemRenderer
{
public:
    SystemRenderer(const ScoreArea* scoreArea,
                   const Score* score, const int lineSpacing);
    
    QGraphicsItem* operator()(boost::shared_ptr<const System> system);
    void connectSignals();

private:
    const ScoreArea* scoreArea;
    const Score* score;
    const int lineSpacing;
    boost::shared_ptr<const System> system;
    boost::shared_ptr<const Staff> staff;
    QGraphicsItem* parentSystem;
    QGraphicsItem* parentStaff;
    MusicFont musicFont;
    static QFont plainTextFont;
    static QFont symbolTextFont;
    static QFont rehearsalSignFont;
    static QFont musicNotationFont;
    
    // store these items so that their signals can be connected back to the ScoreArea later
    std::vector<StaffPainter*> staffPainters;
    std::vector<BarlinePainter*> barlinePainters;

    enum VolumeSwellType
    {
        VolumeIncreasing,
        VolumeDecreasing
    };

    void centerItem(QGraphicsItem* item, float xmin, float xmax, float y);

    void drawSystemBox(boost::shared_ptr<const System> system);

    void drawTabClef(int x, const StaffData& staffInfo);
    void renderBars(const StaffData& currentStaffInfo);

    void drawTabNotes(const StaffData& currentStaffInfo);
    void drawArpeggio(const Position* position, uint32_t x, const StaffData& currentStaffInfo);

    void drawSystemSymbols(const StaffData& currentStaffInfo);
    void drawDividerLine(const StaffData& currentStaffInfo, quint32 y);
    void drawAltEndings(const std::vector<boost::shared_ptr<AlternateEnding> >& altEndings,
                        uint32_t height);
    void drawTempoMarkers(const std::vector<boost::shared_ptr<TempoMarker> >& tempoMarkers,
                          uint32_t height);
    void drawDirections(uint32_t height, const StaffData& currentStaffInfo);
    void drawChordText(uint32_t height, const StaffData& currentStaffInfo);

    void drawRhythmSlashes();

    void drawLegato(const StaffData& currentStaffInfo);
    void drawSlides(const StaffData& currentStaffInfo);
    void drawSlidesHelper(const StaffData& currentStaffInfo, quint8 string, bool slideUp,
                          quint32 posIndex1, quint32 posIndex2);

    void drawSymbolsBelowTabStaff(const StaffData& staffInfo);
    QGraphicsItem* createPickStroke(const QString& text);
    QGraphicsItem* createPlainText(const QString& text, QFont::Style style);
    QGraphicsItem* createArtificialHarmonicText(const Position* position);

    void drawSymbols(const StaffData& staffInfo);
    QGraphicsItem* createConnectedSymbolGroup(const QString& text, QFont::Style style, int width,
                                              const StaffData& currentStaffInfo);
    QGraphicsItem* createVolumeSwell(uint8_t width, const StaffData& currentStaffInfo,
                                     VolumeSwellType type);
    QGraphicsItem* drawContinuousFontSymbols(QChar symbol, int width);
    QGraphicsItem* createTremoloPicking(const StaffData& currentStaffInfo);
    QGraphicsItem* createTrill(const StaffData& currentStaffInfo);
    QGraphicsItem* createDynamic(boost::shared_ptr<const Dynamic> dynamic);
    QGraphicsItem* createBend(const Position* position, const StaffData& staffInfo);

    void drawStdNotation(const StaffData& currentStaffInfo);
    void adjustAccidentals(QMultiMap<int, StdNotationPainter*>& accidentalsMap);
    void drawMultiBarRest(boost::shared_ptr<const Barline> currentBarline,
                          const StaffData& currentStaffInfo, int measureCount);
    void drawLedgerLines(const std::vector<int> &noteLocations, const double xLocation,
                         const StaffData& staffData);
};

#endif // SYSTEMRENDERER_H
