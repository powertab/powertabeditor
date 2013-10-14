/*
 * Copyright (C) 2013 Cameron White
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

#include "chordnamedialog.h"
#include "ui_chordnamedialog.h"

#include <boost/lexical_cast.hpp>

ChordNameDialog::ChordNameDialog(QWidget *parent)
    : QDialog(parent), ui(new Ui::ChordNameDialog)
{
    ui->setupUi(this);

    initCheckBox(ui->noChordCheckBox);
    initCheckBox(ui->bracketsCheckBox);

    myTonicVariations = new QButtonGroup(this);
    myTonicVariations->addButton(ui->tonicFlatButton);
    myTonicVariations->addButton(ui->tonicSharpButton);
    // Allow none of the buttons to be clicked, which requires a little extra
    // work.
    myTonicVariations->setExclusive(false);
    connect(myTonicVariations, SIGNAL(buttonClicked(QAbstractButton *)), this,
            SLOT(onTonicVariationClicked(QAbstractButton *)));

    myBassVariations = new QButtonGroup(this);
    myBassVariations->addButton(ui->bassFlatButton);
    myBassVariations->addButton(ui->bassSharpButton);
    myBassVariations->setExclusive(false);
    connect(myBassVariations, SIGNAL(buttonClicked(QAbstractButton *)), this,
            SLOT(onBassVariationClicked(QAbstractButton *)));

    myTonicKeys = new QButtonGroup(this);
    myTonicKeys->addButton(ui->tonicCButton, ChordName::C);
    myTonicKeys->addButton(ui->tonicDButton, ChordName::D);
    myTonicKeys->addButton(ui->tonicEButton, ChordName::E);
    myTonicKeys->addButton(ui->tonicFButton, ChordName::F);
    myTonicKeys->addButton(ui->tonicGButton, ChordName::G);
    myTonicKeys->addButton(ui->tonicAButton, ChordName::A);
    myTonicKeys->addButton(ui->tonicBButton, ChordName::B);
    connect(myTonicKeys, SIGNAL(buttonClicked(int)), this,
            SLOT(onTonicChanged()));

    myBassKeys = new QButtonGroup(this);
    myBassKeys->addButton(ui->bassCButton, ChordName::C);
    myBassKeys->addButton(ui->bassDButton, ChordName::D);
    myBassKeys->addButton(ui->bassEButton, ChordName::E);
    myBassKeys->addButton(ui->bassFButton, ChordName::F);
    myBassKeys->addButton(ui->bassGButton, ChordName::G);
    myBassKeys->addButton(ui->bassAButton, ChordName::A);
    myBassKeys->addButton(ui->bassBButton, ChordName::B);
    connect(myBassKeys, SIGNAL(buttonClicked(int)), this, SLOT(updateState()));

    connect(ui->formulaListWidget, SIGNAL(currentRowChanged(int)), this,
            SLOT(updateState()));

    initCheckBox(ui->add2CheckBox);
    initCheckBox(ui->add4CheckBox);
    initCheckBox(ui->add6CheckBox);
    initCheckBox(ui->add9CheckBox);
    initCheckBox(ui->add11CheckBox);

    initCheckBox(ui->extend9CheckBox);
    initCheckBox(ui->extend11CheckBox);
    initCheckBox(ui->extend13CheckBox);

    initCheckBox(ui->flat5CheckBox);
    initCheckBox(ui->raised5CheckBox);
    initCheckBox(ui->flat9CheckBox);
    initCheckBox(ui->raised9CheckBox);
    initCheckBox(ui->raised11CheckBox);
    initCheckBox(ui->flat13CheckBox);
    initCheckBox(ui->sus2CheckBox);
    initCheckBox(ui->sus4CheckBox);

    // Set up the initial state.
    ui->tonicCButton->setChecked(true);
    ui->bassCButton->setChecked(true);
    ui->formulaListWidget->setCurrentRow(0);
    updateState();
}

ChordNameDialog::~ChordNameDialog()
{
    delete ui;
}

const ChordName &ChordNameDialog::getChordName() const
{
    return myChord;
}

void ChordNameDialog::updateState()
{
    myChord.setNoChord(ui->noChordCheckBox->isChecked());
    myChord.setBrackets(ui->bracketsCheckBox->isChecked());

    myChord.setTonicKey(static_cast<ChordName::Key>(myTonicKeys->checkedId()));

    ChordName::Variation tonicVariation = ChordName::NoVariation;
    if (ui->tonicSharpButton->isChecked())
        tonicVariation = ChordName::Sharp;
    else if (ui->tonicFlatButton->isChecked())
        tonicVariation = ChordName::Flat;
    myChord.setTonicVariation(tonicVariation);

    myChord.setBassKey(static_cast<ChordName::Key>(myBassKeys->checkedId()));

    ChordName::Variation bassVariation = ChordName::NoVariation;
    if (ui->bassSharpButton->isChecked())
        bassVariation = ChordName::Sharp;
    else if (ui->bassFlatButton->isChecked())
        bassVariation = ChordName::Flat;
    myChord.setBassVariation(bassVariation);

    myChord.setFormula(
        static_cast<ChordName::Formula>(ui->formulaListWidget->currentRow()));

    myChord.setModification(ChordName::Added2nd, ui->add2CheckBox->isChecked());
    myChord.setModification(ChordName::Added4th, ui->add4CheckBox->isChecked());
    myChord.setModification(ChordName::Added6th, ui->add6CheckBox->isChecked());
    myChord.setModification(ChordName::Added9th, ui->add9CheckBox->isChecked());
    myChord.setModification(ChordName::Added11th,
                            ui->add11CheckBox->isChecked());

    myChord.setModification(ChordName::Extended9th,
                            ui->extend9CheckBox->isChecked());
    myChord.setModification(ChordName::Extended11th,
                            ui->extend11CheckBox->isChecked());
    myChord.setModification(ChordName::Extended13th,
                            ui->extend13CheckBox->isChecked());

    myChord.setModification(ChordName::Flatted5th,
                            ui->flat5CheckBox->isChecked());
    myChord.setModification(ChordName::Raised5th,
                            ui->raised5CheckBox->isChecked());
    myChord.setModification(ChordName::Flatted9th,
                            ui->flat9CheckBox->isChecked());
    myChord.setModification(ChordName::Raised9th,
                            ui->raised9CheckBox->isChecked());
    myChord.setModification(ChordName::Raised11th,
                            ui->raised11CheckBox->isChecked());
    myChord.setModification(ChordName::Flatted13th,
                            ui->flat13CheckBox->isChecked());
    myChord.setModification(ChordName::Suspended2nd,
                            ui->sus2CheckBox->isChecked());
    myChord.setModification(ChordName::Suspended4th,
                            ui->sus4CheckBox->isChecked());

    ui->previewLineEdit->setText(
        QString::fromStdString(boost::lexical_cast<std::string>(myChord)));
}

void ChordNameDialog::onTonicVariationClicked(QAbstractButton *clickedButton)
{
    for (QAbstractButton *button : myTonicVariations->buttons())
    {
        if (button != clickedButton)
            button->setChecked(false);
    }

    onTonicChanged();
}

void ChordNameDialog::onBassVariationClicked(QAbstractButton *clickedButton)
{
    for (QAbstractButton *button : myBassVariations->buttons())
    {
        if (button != clickedButton)
            button->setChecked(false);
    }

    updateState();
}

void ChordNameDialog::onTonicChanged()
{
    // Make the bass note match the tonic note.
    myBassKeys->button(myTonicKeys->checkedId())->setChecked(true);
    ui->bassSharpButton->setChecked(ui->tonicSharpButton->isChecked());
    ui->bassFlatButton->setChecked(ui->tonicFlatButton->isChecked());

    updateState();
}

void ChordNameDialog::initCheckBox(QCheckBox *checkbox)
{
    connect(checkbox, SIGNAL(clicked()), this, SLOT(updateState()));
}
