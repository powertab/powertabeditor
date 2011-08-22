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
  
#ifndef SCOREAREA_H
#define SCOREAREA_H

#include <QGraphicsView>
#include <QGraphicsScene>
#include <boost/shared_ptr.hpp>
#include <boost/cstdint.hpp>

class PowerTabDocument;
class Caret;
class Score;

/// The visual display of the score
class ScoreArea : public QGraphicsView
{
    Q_OBJECT
public:
    explicit ScoreArea(QWidget *parent = 0);

    void renderDocument(boost::shared_ptr<PowerTabDocument> doc);
    void updateSystem(const uint32_t systemIndex);

    boost::shared_ptr<PowerTabDocument> document;

    inline Caret* getCaret() const
    {
        return caret;
    }

private:
    void renderScore(const Score* score, int lineSpacing);

    QGraphicsScene scene;
    Caret* caret;
    QList<QGraphicsItem*> systemList;

    bool redrawOnNextRefresh;

signals:
    void barlineClicked(int position);
    void keySignatureClicked(int position);

public slots:
    void adjustScroll();
    void requestFullRedraw();
};

#endif // SCOREAREA_H
