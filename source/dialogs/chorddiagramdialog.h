/*
  * Copyright (C) 2022 Cameron White
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

#ifndef DIALOGS_CHORDDIAGRAMDIALOG_H
#define DIALOGS_CHORDDIAGRAMDIALOG_H

#include <QDialog>
#include <QGraphicsScene>
#include <memory>
#include <score/chorddiagram.h>

namespace Ui
{
class ChordDiagramDialog;
}

class ChordDiagramPainter;
class Score;

class ChordDiagramDialog final : public QDialog
{
    Q_OBJECT

public:
    ChordDiagramDialog(QWidget *parent, const Score &score,
                       const ChordDiagram *current_diagram);
    ~ChordDiagramDialog();

    ChordDiagram getChordDiagram() const { return myDiagram; }

private slots:
    void editChordName();
    void onDiagramClicked(int string, int fret);
    void onTopFretChanged(int top_fret);

private:
    void onDiagramChanged();

    std::unique_ptr<Ui::ChordDiagramDialog> ui;
    const Score &myScore;
    ChordDiagram myDiagram;

    QGraphicsScene myScene;
    ChordDiagramPainter *myPainter = nullptr;
};

#endif
