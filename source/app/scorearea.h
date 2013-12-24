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
#include <QGraphicsView>
#include <QGraphicsScene>
#include <score/staff.h>

class CaretPainter;
class Document;
class ScoreLocationPubSub;
class StaffPubSub;

/// The visual display of the score.
class ScoreArea : public QGraphicsView
{
public:
    explicit ScoreArea(QWidget *parent);

    void renderDocument(const Document &document, Staff::ViewType view);

    /// Redraws the specified system, and shifts the following systems as
    /// necessary.
    void redrawSystem(int index);

    std::shared_ptr<ScoreLocationPubSub> getKeySignaturePubSub() const;
    std::shared_ptr<ScoreLocationPubSub> getTimeSignaturePubSub() const;
    std::shared_ptr<ScoreLocationPubSub> getBarlinePubSub() const;
    std::shared_ptr<ScoreLocationPubSub> getSelectionPubSub() const;
    std::shared_ptr<StaffPubSub> getClefPubSub() const;

private:
    /// Adjusts the scroll location whenever the caret moves.
    void adjustScroll();

    QGraphicsScene myScene;
    boost::optional<const Document &> myDocument;
    Staff::ViewType myViewType;
    QList<QGraphicsItem *> myRenderedSystems;
    CaretPainter *myCaretPainter;

    std::shared_ptr<ScoreLocationPubSub> myKeySignatureClicked;
    std::shared_ptr<ScoreLocationPubSub> myTimeSignatureClicked;
    std::shared_ptr<ScoreLocationPubSub> myBarlineClicked;
    std::shared_ptr<StaffPubSub> myClefClicked;
};

#endif
