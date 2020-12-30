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

#include "settingsmanager.h"

#include <memory>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <score/staff.h>
#include <painters/scoreclickevent.h>

class CaretPainter;
class ConstScoreLocation;
class Document;
class QPrinter;

/// The visual display of the score.
class ScoreArea : public QGraphicsView
{
    Q_OBJECT

    class Scene : public QGraphicsScene
    {
    protected:
        /// The default implementation accepts the event, preventing it from
        /// propagating up to the main window (which is Where we want to handle
        /// drag events).
        void dragEnterEvent(QGraphicsSceneDragDropEvent *event) override;
    };

public:
    explicit ScoreArea(SettingsManager &settings_manager, QWidget *parent);

    void renderDocument(const Document &document);

    void refreshZoom();

    void print(QPrinter &printer);

    void clearSelection() { myScene.clearSelection(); }

    /// Redraws the specified system, and shifts the following systems as
    /// necessary.
    void redrawSystem(int index);

    const ScoreClickEvent &getClickEvent() const { return myClickEvent; }

    /// returns the palette used by scorearea
    const QPalette *getPalette() const;

signals:
    void itemClicked(ScoreItem item, const ConstScoreLocation &location);

protected:
    virtual void focusInEvent(QFocusEvent *event) override;
    virtual void focusOutEvent(QFocusEvent *event) override;
    bool event(QEvent *event) override;

private:
    /// Adjusts the scroll location whenever the caret moves.
    void adjustScroll();

    /// Load the user's preferred color scheme for the score.
    void loadTheme(const SettingsManager &settings_manager, bool redraw = true);

    Scene myScene;
    const Document *myDocument;
    QGraphicsItem *myScoreInfoBlock;
    QList<QGraphicsItem *> myRenderedSystems;
    CaretPainter *myCaretPainter;
    /// The color palette from the parent widget.
    const QPalette *myDefaultPalette;
    /// The color palette for the light theme (and printing).
    QPalette myLightPalette;
    /// The color palette for the dark theme.
    QPalette myDarkPalette;
    /// The palette (default / light / dark) currently used by the score area.
    const QPalette *myActivePalette;

    ScoreClickEvent myClickEvent;
    boost::signals2::scoped_connection mySettingsListener;
    bool myDisableRedraw;
};

#endif
