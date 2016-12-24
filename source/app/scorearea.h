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
#include <memory>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <score/staff.h>

class CaretPainter;
class ClickPubSub;
class Document;
class QPrinter;

/// The visual display of the score.
class ScoreArea : public QGraphicsView
{
    class Scene : public QGraphicsScene
    {
    protected:
        /// The default implementation accepts the event, preventing it from
        /// propagating up to the main window (which is Where we want to handle
        /// drag events).
        void dragEnterEvent(QGraphicsSceneDragDropEvent *event) override;
    };

public:
    explicit ScoreArea(QWidget *parent);

    void renderDocument(const Document &document);

    void refreshZoom();

    void print(QPrinter &printer);

    /// Redraws the specified system, and shifts the following systems as
    /// necessary.
    void redrawSystem(int index);

    std::shared_ptr<ClickPubSub> getClickPubSub() const;

protected:
    virtual void focusInEvent(QFocusEvent *event) override;
    virtual void focusOutEvent(QFocusEvent *event) override;

private:
    /// Adjusts the scroll location whenever the caret moves.
    void adjustScroll();

    Scene myScene;
    boost::optional<const Document &> myDocument;
    QGraphicsItem *myScoreInfoBlock;
    QList<QGraphicsItem *> myRenderedSystems;
    CaretPainter *myCaretPainter;

    std::shared_ptr<ClickPubSub> myClickPubSub;
};

#endif
