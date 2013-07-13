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

#include <QGraphicsView>
#include <QGraphicsScene>

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

#if 0
    ~ScoreArea();

    void renderDocument(boost::shared_ptr<PowerTabDocument> doc);
    void updateSystem(const uint32_t systemIndex);

    boost::shared_ptr<PowerTabDocument> document;

    inline Caret* getCaret() const
    {
        return caret.get();
    }

    boost::shared_ptr<SystemLocationPubSub> keySignaturePubSub() const;
    boost::shared_ptr<SystemLocationPubSub> timeSignaturePubSub() const;
    boost::shared_ptr<SystemLocationPubSub> barlinePubSub() const;

private:
    void renderScore(const Score* score, int lineSpacing);

    QGraphicsScene scene;
    PowerTabEditor* editor;
    boost::scoped_ptr<Caret> caret;
    QList<QGraphicsItem*> systemList;
    size_t scoreIndex;

    boost::shared_ptr<SystemLocationPubSub> keySignatureClicked;
    boost::shared_ptr<SystemLocationPubSub> timeSignatureClicked;
    boost::shared_ptr<SystemLocationPubSub> barlineClicked;

    bool redrawOnNextRefresh;

public slots:
    void adjustScroll();
    void requestFullRedraw();
    void setScoreIndex(int newScoreIndex);
#endif
};

#endif
