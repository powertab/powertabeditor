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
  
#include "scorearea.h"

#include <app/documentmanager.h>
#include <app/pubsub/scorelocationpubsub.h>
#include <boost/foreach.hpp>
#include <boost/make_shared.hpp>
#include <boost/timer.hpp>
#include <painters/caretpainter.h>
#include <painters/systemrenderer.h>
#include <QDebug>
#include <QGraphicsItem>
#include <QProgressDialog>
#include <score/score.h>

static const double SYSTEM_SPACING = 50;

#if 1
ScoreArea::ScoreArea(QWidget *parent)
    : QGraphicsView(parent),
      myViewType(Staff::GuitarView),
      myKeySignatureClicked(boost::make_shared<ScoreLocationPubSub>()),
      myTimeSignatureClicked(boost::make_shared<ScoreLocationPubSub>()),
      myBarlineClicked(boost::make_shared<ScoreLocationPubSub>())
{
    setScene(&myScene);
}

void ScoreArea::renderDocument(const Document &document, Staff::ViewType view)
{
    myScene.clear();
    myRenderedSystems.clear();
    myDocument = document;
    myViewType = view;

    const Score &score = document.getScore();

    boost::timer timer;
    QProgressDialog progressDialog(tr("Rendering ..."), "", 0,
                                   score.getSystems().size());
    progressDialog.setCancelButton(0);
    progressDialog.setWindowModality(Qt::WindowModal);
    progressDialog.show();

    CaretPainter *caretPainter = new CaretPainter(document.getCaret());

    // Render each system.
    int i = 0;
    double height = 0;
    BOOST_FOREACH(const System &system, score.getSystems())
    {
        progressDialog.setValue(i);
        SystemRenderer render(this, score);

        QGraphicsItem *renderedSystem = render(system, i, myViewType);
        renderedSystem->setPos(0, height);
        myRenderedSystems << renderedSystem;
        myScene.addItem(renderedSystem);
        height += renderedSystem->boundingRect().bottom() + SYSTEM_SPACING;
#if 0
        renderer.connectSignals();
#endif
        ++i;

        caretPainter->addSystemRect(renderedSystem->sceneBoundingRect());
    }

    myScene.addItem(caretPainter);

    progressDialog.setValue(i);

    qDebug() << "Score rendered in" << timer.elapsed() << "seconds";
    qDebug() << "Rendered " << myScene.items().size() << "items";
}

boost::shared_ptr<ScoreLocationPubSub> ScoreArea::getKeySignaturePubSub() const
{
    return myKeySignatureClicked;
}

boost::shared_ptr<ScoreLocationPubSub> ScoreArea::getTimeSignaturePubSub() const
{
    return myTimeSignatureClicked;
}

boost::shared_ptr<ScoreLocationPubSub> ScoreArea::getBarlinePubSub() const
{
    return myBarlineClicked;
}

boost::shared_ptr<ScoreLocationPubSub> ScoreArea::getSelectionPubSub() const
{
    return myDocument->getCaret().getSelectionPubSub();
}

#else
ScoreArea::ScoreArea(PowerTabEditor *editor) :
    editor(editor),
    caret(NULL),
    scoreIndex(0),
    keySignatureClicked(boost::make_shared<SystemLocationPubSub>()),
    timeSignatureClicked(boost::make_shared<SystemLocationPubSub>()),
    barlineClicked(boost::make_shared<SystemLocationPubSub>())
{
    setViewportUpdateMode(QGraphicsView::BoundingRectViewportUpdate);

    setScene(&scene);

    redrawOnNextRefresh = false;
}

ScoreArea::~ScoreArea()
{
    // Prevent double deletion.
    if (caret)
    {
        scene.removeItem(caret.get());
    }
}

void ScoreArea::renderDocument(boost::shared_ptr<PowerTabDocument> doc)
{
    bool newCaret = (caret == NULL);
    uint32_t voice = 0;

    if (caret)
    {
        voice = caret->getCurrentVoice();
        scene.removeItem(caret.get());
    }

    scene.clear();
    systemList.clear();
    document = doc;
    int lineSpacing = document->GetTablatureStaffLineSpacing();

    // Set up the caret
    if (newCaret)
    {
        caret.reset(new Caret(doc->GetTablatureStaffLineSpacing()));
        connect(caret.get(), SIGNAL(moved()), this, SLOT(adjustScroll()));
    }

    caret->setScore(doc->GetScore(scoreIndex));
    caret->setCurrentVoice(voice);

    // Adjust the caret to a valid position, since (for example) a system may
    // have been removed.
    caret->adjustToValidLocation();

    if (newCaret)
    {
        editor->registerCaret(caret.get());
    }

    scene.addItem(caret.get());

    // Render each score
    // Only worry about the guitar score so far
    renderScore(document->GetScore(scoreIndex), lineSpacing);
}

/// Updates the system after changes have been made
/// @param systemIndex The index of the system that was modified
void ScoreArea::updateSystem(const uint32_t systemIndex)
{
    if (redrawOnNextRefresh)
    {
        redrawOnNextRefresh = false;
        renderDocument(document);
    }
    else
    {
        // delete and remove the system from the scene
        delete systemList.takeAt(systemIndex);

        // redraw the system
        const Score* currentScore = document->GetScore(scoreIndex);
        
        SystemRenderer renderer(this, currentScore,
                                document->GetTablatureStaffLineSpacing());
        QGraphicsItem* newSystem = renderer(currentScore->GetSystem(systemIndex));
        renderer.connectSignals();
        
        scene.addItem(newSystem);
        systemList.insert(systemIndex, newSystem);

        // Adjust the position of subsequent systems
        for (int i = systemIndex; i < systemList.size(); i++)
        {
            QGraphicsItem* systemPainter = systemList.at(i);
            const Rect currentSystemRect = currentScore->GetSystem(i)->GetRect();
            systemPainter->setPos(currentSystemRect.GetLeft(), currentSystemRect.GetTop());
        }

        caret->updatePosition();
    }
}

/// Used to request that a full redraw is performed when the ScoreArea is next updated
void ScoreArea::requestFullRedraw()
{
    Q_ASSERT(!redrawOnNextRefresh);
    redrawOnNextRefresh = true;
}

// ensures that the caret is visible when it changes sections
void ScoreArea::adjustScroll()
{
    ensureVisible(caret.get(), 0, 100);
}

#endif
