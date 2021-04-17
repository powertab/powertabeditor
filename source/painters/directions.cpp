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

#include <app/scorearea.h>
#include <painters/clickableitem.h>
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

double
SystemRenderer::drawDirections(const ConstScoreLocation &location,
                               const LayoutInfo &layout, double height)
{
    double maxHeight = 0;

    for (const Direction &direction : location.getSystem().getDirections())
    {
        double localHeight = 0;
        const double x = layout.getPositionX(direction.getPosition());

        ConstScoreLocation dir_location(location);
        dir_location.setPositionIndex(direction.getPosition());
        auto group = new ClickableGroup(
            QObject::tr("Double-click to edit musical direction."),
            myScoreArea->getClickEvent(), dir_location, ScoreItem::Direction);
        group->setParentItem(myParentSystem);

        for (const DirectionSymbol &symbol : direction.getSymbols())
        {
            double y =
                height + localHeight + 0.5 * LayoutInfo::SYSTEM_SYMBOL_SPACING;
            QGraphicsItem *item = nullptr;
            const QPen pen = myPalette.text().color();
            switch (symbol.getSymbolType())
            {
                case DirectionSymbol::Coda:
                    item = new SimpleTextItem(QChar(MusicFont::Coda),
                                              myMusicNotationFont,
                                              TextAlignment::Baseline, pen);
                    break;
                case DirectionSymbol::DoubleCoda:
                    item = new SimpleTextItem(QString(2, MusicFont::Coda),
                                              myMusicNotationFont,
                                              TextAlignment::Baseline, pen);
                    break;
                case DirectionSymbol::Segno:
                    item = new SimpleTextItem(QChar(MusicFont::Segno),
                                              myMusicNotationFont,
                                              TextAlignment::Baseline, pen);
                    break;
                case DirectionSymbol::SegnoSegno:
                    item = new SimpleTextItem(QString(2, MusicFont::Segno),
                                              myMusicNotationFont,
                                              TextAlignment::Baseline, pen);
                    break;
                default:
                    // Display plain text.
                    QFont font = myPlainTextFont;
                    font.setItalic(true);
                    item = new SimpleTextItem(
                        theDirectionText[symbol.getSymbolType()], font,
                        TextAlignment::Top, pen);

                    // Vertically center the text.
                    y -= 0.5 * item->boundingRect().height();
                    break;
            }

            centerHorizontally(*item, x, x + layout.getPositionSpacing());
            item->setY(y);

            group->addToGroup(item);
            localHeight += LayoutInfo::SYSTEM_SYMBOL_SPACING;
        }

        maxHeight = std::max(maxHeight, localHeight);
    }

    return maxHeight;
}
