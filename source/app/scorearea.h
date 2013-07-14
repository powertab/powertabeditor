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
  
#ifndef APP_SCOREAREA_H
#define APP_SCOREAREA_H

#include <boost/optional.hpp>
#include <boost/shared_ptr.hpp>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <score/staff.h>

class Score;
class ScoreLocationPubSub;

/// The visual display of the score.
class ScoreArea : public QGraphicsView
{
    Q_OBJECT

public:
#if 0
    explicit ScoreArea(PowerTabEditor* editor);
#else
    explicit ScoreArea(QWidget *parent);
#endif

    void renderScore(const Score &score, Staff::ViewType view);

#if 0
    ~ScoreArea();

    boost::shared_ptr<SystemLocationPubSub> keySignaturePubSub() const;
#endif
    boost::shared_ptr<ScoreLocationPubSub> getTimeSignaturePubSub() const;
    boost::shared_ptr<ScoreLocationPubSub> getBarlinePubSub() const;

private:
    QGraphicsScene myScene;
    boost::optional<const Score &> myScore;
    Staff::ViewType myViewType;
    QList<QGraphicsItem *> myRenderedSystems;

#if 0
    PowerTabEditor* editor;
    boost::scoped_ptr<Caret> caret;

    boost::shared_ptr<SystemLocationPubSub> keySignatureClicked;
#endif
    boost::shared_ptr<ScoreLocationPubSub> myTimeSignatureClicked;
    boost::shared_ptr<ScoreLocationPubSub> myBarlineClicked;
#if 0
    bool redrawOnNextRefresh;

public slots:
    void adjustScroll();
    void requestFullRedraw();
#endif
};

#endif
