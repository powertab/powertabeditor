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

static const QString theDirectionText[] = {
    QStringLiteral("Coda"),           QStringLiteral("Double Coda"),
    QStringLiteral("Segno"),          QStringLiteral("Segno Segno"),
    QStringLiteral("Fine"),           QStringLiteral("D.C."),
    QStringLiteral("D.S."),           QStringLiteral("D.S.S."),
    QStringLiteral("To Coda"),        QStringLiteral("To Dbl. Coda"),
    QStringLiteral("D.C. al Coda"),   QStringLiteral("D.C. al Dbl. Coda"),
    QStringLiteral("D.S. al Coda"),   QStringLiteral("D.S. al Dbl. Coda"),
    QStringLiteral("D.S.S. al Coda"), QStringLiteral("D.S.S. al Dbl. Coda"),
    QStringLiteral("D.C. al Fine"),   QStringLiteral("D.S. al Fine"),
    QStringLiteral("D.S.S. al Fine")
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
            double y =
                height + localHeight + 0.5 * LayoutInfo::SYSTEM_SYMBOL_SPACING;
            QGraphicsItem *item = nullptr;
            switch (symbol.getSymbolType())
            {
                case DirectionSymbol::Coda:
                    item = new SimpleTextItem2(QChar(MusicFont::Coda),
                                              myMusicNotationFont);
                    break;
                case DirectionSymbol::DoubleCoda:
                    item = new SimpleTextItem2(QString(2, MusicFont::Coda),
                                              myMusicNotationFont);
                    break;
                case DirectionSymbol::Segno:
                    item = new SimpleTextItem2(QChar(MusicFont::Segno),
                                              myMusicNotationFont);
                    break;
                case DirectionSymbol::SegnoSegno:
                    item = new SimpleTextItem2(QString(2, MusicFont::Segno),
                                              myMusicNotationFont);
                    break;
                default:
                    // Display plain text.
                    QFont font = myPlainTextFont;
                    font.setItalic(true);
                    item = new SimpleTextItem2(
                        theDirectionText[symbol.getSymbolType()], font);

                    // The coda / segno symbols are already aligned so that the
                    // baseline passes through the middle of the symbol. For
                    // plain text, adjust by the ascent to vertically center
                    // the text.
                    QFontMetrics fm(font);
                    y += 0.5 * fm.ascent();
                    break;
            }

            centerHorizontally(*item, x, x + layout.getPositionSpacing());
            item->setY(y);

            item->setParentItem(myParentSystem);
            localHeight += LayoutInfo::SYSTEM_SYMBOL_SPACING;
        }

        maxHeight = std::max(maxHeight, localHeight);
    }

    return maxHeight;
}
