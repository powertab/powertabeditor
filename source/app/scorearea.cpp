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
#include <app/settings.h>
#include <chrono>
#include <future>
#include <painters/caretpainter.h>
#include <painters/chorddiagrampainter.h>
#include <painters/scoreclickevent.h>
#include <painters/scoreinforenderer.h>
#include <painters/systemrenderer.h>
#include <QDebug>
#include <QGraphicsItem>
#include <QGraphicsSceneDragDropEvent>
#include <QPrinter>
#include <QScrollBar>
#include <score/score.h>

void ScoreArea::Scene::dragEnterEvent(QGraphicsSceneDragDropEvent *event)
{
    event->ignore();
}

ScoreArea::ScoreArea(SettingsManager &settings_manager, QWidget *parent)
    : QGraphicsView(parent),
      myScoreInfoBlock(nullptr),
      myChordDiagramList(nullptr),
      myCaretPainter(nullptr),
      myDefaultPalette(&parent->palette()),
      myActivePalette(nullptr),
      myDisableRedraw(false)
{
    setScene(&myScene);

    // Configure the palette for the light theme and printing.
    myLightPalette.setColor(QPalette::Base, Qt::white);
    myLightPalette.setColor(QPalette::Text, Qt::black);
    myLightPalette.setColor(QPalette::Light, Qt::white);
    myLightPalette.setColor(QPalette::Dark, Qt::lightGray);

    // Configure the palette for the dark theme.
    myDarkPalette.setColor(QPalette::Base, QColor(30, 30, 30));
    myDarkPalette.setColor(QPalette::Text, QColor(255, 255, 255, 216));
    myDarkPalette.setColor(QPalette::Light, QColor(55, 55, 55));
    myDarkPalette.setColor(QPalette::Dark, QColor(191, 191, 191));

    // Load the user's preferred theme, and re-render when the theme setting
    // changes.
    loadTheme(settings_manager, /* redraw */ false);
    loadSystemSpacing(settings_manager, false);
    mySettingsListener = settings_manager.subscribeToChanges(
        [&]()
        {
            loadTheme(settings_manager);
            loadSystemSpacing(settings_manager);
        });

    // Connect the click event handler to our public signals.
    myClickEvent.connect(
        [&](ScoreItem item, const ConstScoreLocation &location,
            ScoreItemAction action) { itemClicked(item, location, action); });
}

void ScoreArea::renderDocument(const Document &document)
{
    myScene.clear();
    myRenderedSystems.clear();
    myDocument = &document;

    refreshZoom();

    const Score &score = document.getScore();

    auto start = std::chrono::high_resolution_clock::now();

    myCaretPainter = new CaretPainter(
        document.getCaret(), document.getViewOptions(), *myActivePalette);
    myCaretPainter->subscribeToMovement([=]() {
        adjustScroll();
    });

    myScoreInfoBlock = ScoreInfoRenderer::render(
        score, myActivePalette->text().color(), myClickEvent);

    myChordDiagramList = ChordDiagramPainter::renderDiagrams(
        score, myActivePalette->text().color(), myClickEvent,
        LayoutInfo::STAFF_WIDTH);

    myRenderedSystems.reserve(static_cast<int>(score.getSystems().size()));
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
    height += myScoreInfoBlock->boundingRect().height() + 0.5 * mySystemSpacing;

    myChordDiagramList->setPos(0, height);
    myScene.addItem(myChordDiagramList);
    height += myChordDiagramList->boundingRect().height() + 0.5 * mySystemSpacing;
    myHeaderSize = height;

    // Layout the systems.
    for (QGraphicsItem *system : myRenderedSystems)
    {
        system->setPos(0, height);
        myScene.addItem(system);
        height += system->boundingRect().height() + mySystemSpacing;

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
                mySystemSpacing;
    }
    else
        height = myHeaderSize;

    newSystem->setPos(0, height);
    height += newSystem->boundingRect().height() + mySystemSpacing;
    myCaretPainter->setSystemRect(index, newSystem->sceneBoundingRect());

    myScene.addItem(newSystem);
    myRenderedSystems.insert(index, newSystem);

    // Shift the following systems.
    for (int i = index + 1; i < myRenderedSystems.size(); ++i)
    {
        QGraphicsItem *system = myRenderedSystems[i];
        system->setPos(0, height);
        height += system->boundingRect().height() + mySystemSpacing;
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

    // Use the light palette for printing.
    const QPalette *orig_palette = myActivePalette;
    myActivePalette = &myLightPalette;

    // Hide the caret when printing.
    myCaretPainter->hide();

    //render the document after the palette has been set to print colors
    this->renderDocument(*myDocument);

    // Scale the score based on the ratio between the device's width and our
    // normal staff width in the UI.
    QRectF target_rect(0, 0, painter.device()->width(),
                       painter.device()->height());
    const double ratio = target_rect.width() / LayoutInfo::STAFF_WIDTH;

    QList<QGraphicsItem*> items;
    items.append(myScoreInfoBlock);
    items.append(myChordDiagramList);
    items.append(myRenderedSystems);

    for (int i = 0, n = items.length(); i < n; ++i)
    {
        const QGraphicsItem *item = items[i];
        const QRectF source_rect = item->sceneBoundingRect();
        // Skip if e.g. the score info block is completely empty to avoid
        // division by zero and other issues.
        if (source_rect.height() == 0.0)
            continue;

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
        target_rect.setLeft(source_rect.left() * ratio);
        target_rect.setWidth(source_rect.width() * ratio);
        target_rect.setHeight(source_rect.height() * ratio);
        scene()->render(&painter, target_rect, source_rect);

        // Set the location for the next item.
        target_rect.moveTop(target_rect.y() + height);
    }

    myCaretPainter->show();
    painter.end();

    // Revert to the original app palette and re-render the document
    myActivePalette = orig_palette;
    renderDocument(*myDocument);
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

const QPalette *ScoreArea::getPalette() const
{
    return myActivePalette;
}

bool ScoreArea::event(QEvent *event)
{
    QGraphicsView::event(event);

    // Redraw when the parent widget's (default) palette changes, or our
    // palette is changed.
    if (event->type() == QEvent::PaletteChange)
    {
        if (!myDisableRedraw)
            this->renderDocument(*myDocument);

        return true;
    }

    return false;
}

void
ScoreArea::loadTheme(const SettingsManager &settings_manager, bool redraw)
{
    auto settings = settings_manager.getReadHandle();
    const QPalette *prev_palette = myActivePalette;

    switch (settings->get(Settings::Theme))
    {
        case ScoreTheme::SystemDefault:
            myActivePalette = myDefaultPalette;
            break;
        case ScoreTheme::Light:
            myActivePalette = &myLightPalette;
            break;
        case ScoreTheme::Dark:
            myActivePalette = &myDarkPalette;
            break;
    }

    if (myActivePalette != prev_palette)
    {
        // The palette change will be detected by the ScoreArea::event()
        // listener, which is where the redraw decision is handled.
        if (!redraw)
            myDisableRedraw = true;

        setPalette(*myActivePalette);
        myDisableRedraw = false;
    }
}

void
ScoreArea::loadSystemSpacing(const SettingsManager &settings_manager, bool redraw)
{
    auto settings = settings_manager.getReadHandle();
    mySystemSpacing = settings->get(Settings::SystemSpacing);
    if (redraw)
        this->renderDocument(*myDocument);
}
