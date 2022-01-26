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
#include <score/score.h>
#include <util/tostring.h>

ChordDiagramDialog::ChordDiagramDialog(QWidget *parent, const Score &score,
                                       const ChordDiagram *current_diagram)
    : QDialog(parent), ui(new Ui::ChordDiagramDialog), myScore(score)
{
    ui->setupUi(this);

    connect(ui->buttonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(ui->buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);

    if (current_diagram)
        myDiagram = *current_diagram;

    ui->chordNameButton->setToolTip(tr("Click to edit chord name."));
    ui->chordNameButton->setText(
        QString::fromStdString(Util::toString(myDiagram.getChordName())));
    connect(ui->chordNameButton, &ClickableLabel::clicked, this,
            &ChordDiagramDialog::editChordName);

    ui->numStringsSpinBox->setMinimum(Tuning::MIN_STRING_COUNT);
    ui->numStringsSpinBox->setMaximum(Tuning::MAX_STRING_COUNT);
    ui->numStringsSpinBox->setValue(myDiagram.getStringCount());
    connect(ui->topFretSpinBox, qOverload<int>(&QSpinBox::valueChanged),
            [&](int value)
            {
                myDiagram.setStringCount(value);
                onDiagramChanged();
            });

    ui->topFretSpinBox->setValue(myDiagram.getTopFret());
    connect(ui->topFretSpinBox, qOverload<int>(&QSpinBox::valueChanged),
            [&](int value)
            {
                myDiagram.setTopFret(value);
                onDiagramChanged();
            });
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
    // TODO
}
