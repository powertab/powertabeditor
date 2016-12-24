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
#include <app/pubsub/clickpubsub.h>
#include <chrono>
#include <future>
#include <painters/caretpainter.h>
#include <painters/scoreinforenderer.h>
#include <painters/systemrenderer.h>
#include <QDebug>
#include <QGraphicsItem>
#include <QGraphicsSceneDragDropEvent>
#include <QPrinter>
#include <QScrollBar>
#include <score/score.h>

static const double SYSTEM_SPACING = 50;

void ScoreArea::Scene::dragEnterEvent(QGraphicsSceneDragDropEvent *event)
{
    event->ignore();
}

ScoreArea::ScoreArea(QWidget *parent)
    : QGraphicsView(parent),
      myScoreInfoBlock(nullptr),
      myCaretPainter(nullptr),
      myClickPubSub(std::make_shared<ClickPubSub>())
{
    setScene(&myScene);
}

void ScoreArea::renderDocument(const Document &document)
{
    myScene.clear();
    myRenderedSystems.clear();
    myDocument = document;

    const Score &score = document.getScore();

    auto start = std::chrono::high_resolution_clock::now();

    myCaretPainter =
        new CaretPainter(document.getCaret(), document.getViewOptions());
    myCaretPainter->subscribeToMovement([=]() {
        adjustScroll();
    });

    myScoreInfoBlock = ScoreInfoRenderer::render(score.getScoreInfo());

    myRenderedSystems.reserve(score.getSystems().size());
    for (unsigned int i = 0; i < score.getSystems().size(); ++i)
        myRenderedSystems.append(nullptr);

#if 0
    const int num_threads = std::thread::hardware_concurrency();
#else
    const int num_threads = 1;
#endif
    std::vector<std::future<void>> tasks;
    const int work_size = myRenderedSystems.size() / num_threads;
    qDebug() << "Using" << num_threads << "worker thread(s)";

    for (int i = 0; i < num_threads; ++i)
    {
        const int left = i * work_size;
        const int right = (i == num_threads - 1) ? myRenderedSystems.size()
                                                 : (i + 1) * work_size;

        tasks.push_back(std::async(std::launch::async, [&](int left, int right)
        {
            for (int i = left; i < right; ++i)
            {
                SystemRenderer render(this, score, document.getViewOptions());
                myRenderedSystems[i] = render(score.getSystems()[i], i);
            }
        }, left, right));
    }

    for (auto &&task : tasks)
        task.get();

    double height = 0;
    // Score info.
    myScene.addItem(myScoreInfoBlock);
    height += myScoreInfoBlock->boundingRect().height() + 0.5 * SYSTEM_SPACING;

    // Layout the systems.
    for (QGraphicsItem *system : myRenderedSystems)
    {
        system->setPos(0, height);
        myScene.addItem(system);
        height += system->boundingRect().height() + SYSTEM_SPACING;

        myCaretPainter->addSystemRect(system->sceneBoundingRect());
    }

    myScene.addItem(myCaretPainter);

    auto end = std::chrono::high_resolution_clock::now();
    qDebug() << "Score rendered in"
             << std::chrono::duration_cast<std::chrono::milliseconds>(
                    end - start).count() << "ms";
    qDebug() << "Rendered " << myScene.items().size() << "items";
}

void ScoreArea::redrawSystem(int index)
{
    // Delete and remove the system from the scene.
    delete myRenderedSystems.takeAt(index);

    const Score &score = myDocument->getScore();
    SystemRenderer render(this, score, myDocument->getViewOptions());
    QGraphicsItem *newSystem = render(score.getSystems()[index], index);

    double height = 0;
    if (index > 0)
    {
        height = myRenderedSystems.at(index - 1)->sceneBoundingRect().bottom() +
                SYSTEM_SPACING;
    }

    newSystem->setPos(0, height);
    height += newSystem->boundingRect().height() + SYSTEM_SPACING;
    myCaretPainter->setSystemRect(index, newSystem->sceneBoundingRect());

    myScene.addItem(newSystem);
    myRenderedSystems.insert(index, newSystem);

    // Shift the following systems.
    for (int i = index + 1; i < myRenderedSystems.size(); ++i)
    {
        QGraphicsItem *system = myRenderedSystems[i];
        system->setPos(0, height);
        height += system->boundingRect().height() + SYSTEM_SPACING;
        myCaretPainter->setSystemRect(i, system->sceneBoundingRect());
    }

    // The spacing may have changed, so update the caret's position and redraw
    // it.
    myCaretPainter->updatePosition();
}

void ScoreArea::print(QPrinter &printer)
{
    QPainter painter;
    painter.begin(&printer);

    // Hide the caret when printing.
    myCaretPainter->hide();

    QRectF target_rect(0, 0, painter.device()->width(),
                       painter.device()->height());

    QList<QGraphicsItem*> items;
    items.append(myScoreInfoBlock);
    items.append(myRenderedSystems);

    for (int i = 0, n = items.length(); i < n; ++i)
    {
        const QGraphicsItem *item = items[i];

        const QRectF source_rect = item->sceneBoundingRect();
        const float ratio =
            std::min(target_rect.width() / source_rect.width(),
                     target_rect.height() / source_rect.height());

        if (i > 0)
        {
            const QGraphicsItem *prev_item = items[i - 1];
            const double spacing =
                source_rect.y() - prev_item->sceneBoundingRect().bottom();
            target_rect.moveTop(target_rect.y() + spacing * ratio);
        }

        // Figure out how much space the item will take up on the page, and
        // determine if we need a page break.
        const float height = source_rect.height() * ratio;
        if (target_rect.y() + height > painter.device()->height())
        {
            printer.newPage();
            target_rect.moveTop(0);
        }

        // Draw the system on the page.
        scene()->render(&painter, target_rect, source_rect);

        // Set the location for the next item.
        target_rect.moveTop(target_rect.y() + height);
    }

    myCaretPainter->show();
    painter.end();
}

std::shared_ptr<ClickPubSub> ScoreArea::getClickPubSub() const
{
    return myClickPubSub;
}

void ScoreArea::adjustScroll()
{
    if (myDocument->getCaret().isInPlaybackMode())
    {
        QPoint point(0, myCaretPainter->getCurrentSystemRect().y());
        point = transform().map(point);
        verticalScrollBar()->setValue(point.y());
    }
    else
        ensureVisible(myCaretPainter->sceneBoundingRect(), 0, 0);
}

void ScoreArea::focusInEvent(QFocusEvent *)
{
    myScene.update(myCaretPainter->sceneBoundingRect());
}

void ScoreArea::focusOutEvent(QFocusEvent *)
{
    // Redraw the caret to indicate that the score has lost focus.
    myScene.update(myCaretPainter->sceneBoundingRect());
}

void ScoreArea::refreshZoom()
{
    double scale_factor = myDocument->getViewOptions().getZoom() / 100.0;

    QTransform xform;
    xform.scale(scale_factor, scale_factor);
    setTransform(xform);
}
