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

#include <QDebug>
#include <QProgressDialog>

#include <powertabdocument/powertabdocument.h>
#include <powertabdocument/score.h>
#include <powertabdocument/system.h>

#include <painters/caret.h>
#include <painters/systemrenderer.h>

#include <boost/timer.hpp>

ScoreArea::ScoreArea(QWidget *parent) :
    QGraphicsView(parent),
    caret(NULL)
{
    setViewportUpdateMode(QGraphicsView::MinimalViewportUpdate);
    setCacheMode(QGraphicsView::CacheBackground);

    setScene(&scene);
    setRenderHints(QPainter::HighQualityAntialiasing);

    redrawOnNextRefresh = false;
}

void ScoreArea::renderDocument(boost::shared_ptr<PowerTabDocument> doc)
{
    scene.clear();
    systemList.clear();
    document = doc;
    int lineSpacing = document->GetTablatureStaffLineSpacing();

    // Set up the caret
    caret = new Caret(doc->GetTablatureStaffLineSpacing());
    connect(caret, SIGNAL(moved()), this, SLOT(adjustScroll()));

    caret->setScore(doc->GetGuitarScore());
    caret->updatePosition();

    scene.addItem(caret);

    // Render each score
    // Only worry about the guitar score so far
    renderScore(document->GetGuitarScore(), lineSpacing);
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
        const Score* currentScore = document->GetGuitarScore();
        
        SystemRenderer renderer(currentScore, document->GetTablatureStaffLineSpacing());
        QGraphicsItem* newSystem = renderer(currentScore->GetSystem(systemIndex));
        renderer.connectSignals(this);
        
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

void ScoreArea::renderScore(const Score* score, int lineSpacing)
{
    boost::timer timer;
    
    QProgressDialog progressDialog(tr("Rendering ..."), "", 0, score->GetSystemCount());
    progressDialog.setCancelButton(0);
    progressDialog.setWindowModality(Qt::WindowModal);
    progressDialog.show();

    // Render each system (group of staves) in the entire score
    for (uint32_t i=0; i < score->GetSystemCount(); i++)
    {
        progressDialog.setValue(i);
        SystemRenderer renderer(score, lineSpacing);
        systemList << renderer(score->GetSystem(i));
        scene.addItem(systemList.back());
        renderer.connectSignals(this);
    }
    
    progressDialog.setValue(score->GetSystemCount());

    qDebug() << "Score rendered in" << timer.elapsed() << "seconds";
    qDebug() << "Rendered " << scene.items().size() << "items";
}

// ensures that the caret is visible when it changes sections
void ScoreArea::adjustScroll()
{
    ensureVisible(caret, 50, 100);
}
