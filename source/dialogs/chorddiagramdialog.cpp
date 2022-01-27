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

#include "chorddiagramdialog.h"
#include "ui_chorddiagramdialog.h"

#include "chordnamedialog.h"
#include <painters/chorddiagrampainter.h>
#include <score/score.h>
#include <util/tostring.h>

ChordDiagramDialog::ChordDiagramDialog(QWidget *parent, const Score &score,
                                       const ChordDiagram *current_diagram)
    : QDialog(parent),
      ui(new Ui::ChordDiagramDialog),
      myScore(score)
{
    ui->setupUi(this);

    connect(ui->buttonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(ui->buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);

    if (current_diagram)
        myDiagram = *current_diagram;
    else
    {
        // Default to 6 strings, all muted.
        // TODO - could probably initialize using the # of strings from a
        // player the score, or remember the last # of strings that was used.
        std::vector<int> frets(6, -1);
        myDiagram.setFretNumbers(frets);
    }

    ui->chordNameButton->setToolTip(tr("Click to edit chord name."));
    ui->chordNameButton->setText(
        QString::fromStdString(Util::toString(myDiagram.getChordName())));
    connect(ui->chordNameButton, &ClickableLabel::clicked, this,
            &ChordDiagramDialog::editChordName);

    ui->numStringsSpinBox->setMinimum(Tuning::MIN_STRING_COUNT);
    ui->numStringsSpinBox->setMaximum(Tuning::MAX_STRING_COUNT);
    ui->numStringsSpinBox->setValue(myDiagram.getStringCount());
    connect(ui->numStringsSpinBox, qOverload<int>(&QSpinBox::valueChanged),
            [&](int value)
            {
                myDiagram.setStringCount(value);
                onDiagramChanged();
            });

    ui->topFretSpinBox->setValue(myDiagram.getTopFret());
    connect(ui->topFretSpinBox, qOverload<int>(&QSpinBox::valueChanged), this,
            &ChordDiagramDialog::onTopFretChanged);

    // Set up a graphics view for the rendered diagram. This is scaled up more
    // than in the score view.
    ui->diagramView->setScene(&myScene);
    ui->diagramView->setSceneRect(QRectF(0,15,46, 40));

    QTransform xform;
    xform.scale(4, 4);
    ui->diagramView->setTransform(xform);
    ui->diagramView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->diagramView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    // Create the diagram and add it to the scene.
    onDiagramChanged();
}

ChordDiagramDialog::~ChordDiagramDialog()
{
}

void
ChordDiagramDialog::editChordName()
{
    ChordNameDialog dialog(this, myDiagram.getChordName(),
                           ScoreUtils::findAllChordNames(myScore));

    if (dialog.exec() == QDialog::Accepted)
    {
        ChordName name = dialog.getChordName();
        myDiagram.setChordName(name);
        ui->chordNameButton->setText(
            QString::fromStdString(Util::toString(name)));
    }
}

void
ChordDiagramDialog::onDiagramChanged()
{
    delete myPainter;
    myPainter = new ChordDiagramPainter(myDiagram, palette().text().color());
    myScene.addItem(myPainter);

    connect(myPainter, &ChordDiagramPainter::clicked, this,
            &ChordDiagramDialog::onDiagramClicked);
}

void
ChordDiagramDialog::onDiagramClicked(int string, int fret)
{
    if (fret == 0)
    {
        // Toggle between open and muted.
        fret = (myDiagram.getFretNumber(string) == 0) ? -1 : 0;
    }
    else
    {
        fret += myDiagram.getTopFret();
        // Allow toggling to a muted string.
        if (fret == myDiagram.getFretNumber(string))
            fret = -1;
    }

    myDiagram.setFretNumber(string, fret);
    onDiagramChanged();
}

void
ChordDiagramDialog::onTopFretChanged(int top_fret)
{
    // Fret numbers aren't stored relative to the top fret, but the workflow
    // here is that shifting the top fret should shift all the non-empty
    // strings.
    std::vector<int> frets = myDiagram.getFretNumbers();
    for (int &fret:  frets)
    {
        if (fret > 0)
            fret = top_fret + (fret - myDiagram.getTopFret());
    }

    myDiagram.setFretNumbers(frets);
    myDiagram.setTopFret(top_fret);
    onDiagramChanged();
}
