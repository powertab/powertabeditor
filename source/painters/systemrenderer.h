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

class SystemRenderer
{
public:
    SystemRenderer(const Score* score, const int lineSpacing);
    
    QGraphicsItem* operator()(boost::shared_ptr<const System> system);
    void connectSignals(ScoreArea* scoreArea);

private:
    const Score* score;
    const int lineSpacing;
    boost::shared_ptr<const System> system;
    boost::shared_ptr<const Staff> staff;
    QGraphicsItem* parentSystem;
    QGraphicsItem* parentStaff;
    MusicFont musicFont;
    
    // store these items so that their signals can be connected back to the ScoreArea later
    mutable std::vector<StaffPainter*> staffPainters;
    mutable std::vector<BarlinePainter*> barlinePainters;
    mutable std::vector<KeySignaturePainter*> keySignaturePainters;

    enum VolumeSwellType
    {
        VolumeIncreasing,
        VolumeDecreasing
    };

    // Centers an item, by using it's width to calculate the necessary offset from xmin
    void centerItem(QGraphicsItem* item, float xmin, float xmax, float y) const;

    void drawTabClef(int x, const StaffData& staffInfo);
    void renderBars(const StaffData& currentStaffInfo) const;

    void drawTabNotes(const StaffData& currentStaffInfo) const;
    void drawArpeggio(const Position* position, uint32_t x, const StaffData& currentStaffInfo) const;

    void drawSystemSymbols(const StaffData& currentStaffInfo) const;
    void drawDividerLine(const StaffData& currentStaffInfo, quint32 y) const;
    void drawAltEndings(const std::vector<boost::shared_ptr<AlternateEnding> >& altEndings,
                        uint32_t height) const;
    void drawTempoMarkers(const std::vector<boost::shared_ptr<TempoMarker> >& tempoMarkers,
                          uint32_t height) const;
    void drawDirections(uint32_t height, const StaffData& currentStaffInfo) const;
    void drawChordText(uint32_t height, const StaffData& currentStaffInfo) const;

    void drawRhythmSlashes() const;

    void drawLegato(const StaffData& currentStaffInfo) const;
    void drawSlides(const StaffData& currentStaffInfo) const;
    void drawSlidesHelper(const StaffData& currentStaffInfo, quint8 string, bool slideUp,
                          quint32 posIndex1, quint32 posIndex2) const;

    void drawSymbolsBelowTabStaff(const StaffData& staffInfo) const;
    QGraphicsItem* createPickStroke(const QString& text) const;
    QGraphicsItem* createPlainText(const QString& text, QFont::Style style) const;
    QGraphicsItem* createArtificialHarmonicText(const Position* position) const;

    void drawSymbols(const StaffData& staffInfo) const;
    QGraphicsItem* createConnectedSymbolGroup(const QString& text, QFont::Style style, int width,
                                              const StaffData& currentStaffInfo) const;
    QGraphicsItem* createVolumeSwell(uint8_t width, const StaffData& currentStaffInfo,
                                     VolumeSwellType type) const;
    QGraphicsItem* drawContinuousFontSymbols(QChar symbol, int width) const;
    QGraphicsItem* createTremoloPicking(const StaffData& currentStaffInfo) const;
    QGraphicsItem* createTrill(const StaffData& currentStaffInfo) const;
    QGraphicsItem* createDynamic(boost::shared_ptr<const Dynamic> dynamic) const;

    void drawStdNotation(const StaffData& currentStaffInfo) const;
    void adjustAccidentals(QMultiMap<int, StdNotationPainter*>& accidentalsMap) const;
    void drawMultiBarRest(boost::shared_ptr<const Barline> currentBarline,
                          const StaffData& currentStaffInfo, int measureCount) const;
    void drawLedgerLines(const std::vector<double> &noteLocations, const double xLocation,
                         const StaffData& staffData) const;
};

#endif // SYSTEMRENDERER_H
