/*
  * Copyright (C) 2014 Cameron White
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
  
#include "systemrenderer.h"

#include <painters/musicfont.h>
#include <painters/simpletextitem.h>
#include <score/system.h>

static QString theDirectionText[] = {
    "Coda",                "Double Coda",       "Segno",
    "Segno Segno",         "Fine",              "D.C.",
    "D.S.",                "D.S.S.",            "To Coda",
    "To Dbl. Coda",        "D.C. al Coda",      "D.C. al Dbl. Coda",
    "D.S. al Coda",        "D.S. al Dbl. Coda", "D.S.S. al Coda",
    "D.S.S. al Dbl. Coda", "D.C. al Fine",      "D.S. al Fine",
    "D.S.S. al Fine"
};

double SystemRenderer::drawDirections(const System &system,
                                      const LayoutInfo &layout, double height)
{
    double maxHeight = 0;

    for (const Direction &direction : system.getDirections())
    {
        double localHeight = 0;
        const double x = layout.getPositionX(direction.getPosition());

        for (const DirectionSymbol &symbol : direction.getSymbols())
        {
            QGraphicsItem *item = nullptr;
            switch (symbol.getSymbolType())
            {
                case DirectionSymbol::Coda:
                    item = new SimpleTextItem(QChar(MusicFont::Coda),
                                              myMusicNotationFont);
                    break;
                case DirectionSymbol::DoubleCoda:
                    item = new SimpleTextItem(QString(2, MusicFont::Coda),
                                              myMusicNotationFont);
                    break;
                case DirectionSymbol::Segno:
                    item = new SimpleTextItem(QChar(MusicFont::Segno),
                                              myMusicNotationFont);
                    break;
                case DirectionSymbol::SegnoSegno:
                    item = new SimpleTextItem(QString(2, MusicFont::Segno),
                                              myMusicNotationFont);
                    break;
                default:
                    // Display plain text.
                    QFont font = myPlainTextFont;
                    font.setItalic(true);
                    item = new SimpleTextItem(
                        theDirectionText[symbol.getSymbolType()], font);
                    break;
            }

            centerHorizontally(*item, x, x + layout.getPositionSpacing());
            centerSymbolVertically(*item, height + localHeight);
            // Compensate a bit for the alignment of the music notation font.
            if (symbol.getSymbolType() <= DirectionSymbol::SegnoSegno)
                item->setY(item->y() + 4);

            item->setParentItem(myParentSystem);
            localHeight += LayoutInfo::SYSTEM_SYMBOL_SPACING;
        }

        maxHeight = std::max(maxHeight, localHeight);
    }

    return maxHeight;
}