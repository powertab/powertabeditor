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

#include <QButtonGroup>
#include <util/tostring.h>

ChordNameDialog::ChordNameDialog(QWidget *parent,
                                 const ChordName &initial_chord)
    : QDialog(parent), ui(new Ui::ChordNameDialog), myChord(initial_chord)
{
    ui->setupUi(this);

    connect(ui->buttonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(ui->buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);

    initCheckBox(ui->noChordCheckBox, myChord.isNoChord());
    initCheckBox(ui->bracketsCheckBox, myChord.hasBrackets());

    myTonicVariations = new QButtonGroup(this);
    myTonicVariations->addButton(ui->tonicFlatButton);
    myTonicVariations->addButton(ui->tonicSharpButton);
    // Allow none of the buttons to be clicked, which requires a little extra
    // work.
    myTonicVariations->setExclusive(false);
    connect(myTonicVariations,
            qOverload<QAbstractButton *>(&QButtonGroup::buttonClicked), this,
            &ChordNameDialog::onTonicVariationClicked);

    ui->tonicSharpButton->setChecked(myChord.getTonicVariation() ==
                                     ChordName::Sharp);
    ui->tonicFlatButton->setChecked(myChord.getTonicVariation() ==
                                    ChordName::Flat);

    myBassVariations = new QButtonGroup(this);
    myBassVariations->addButton(ui->bassFlatButton);
    myBassVariations->addButton(ui->bassSharpButton);
    myBassVariations->setExclusive(false);
    connect(myBassVariations,
            qOverload<QAbstractButton *>(&QButtonGroup::buttonClicked), this,
            &ChordNameDialog::onBassVariationClicked);

    ui->bassSharpButton->setChecked(myChord.getBassVariation() ==
                                    ChordName::Sharp);
    ui->bassFlatButton->setChecked(myChord.getBassVariation() ==
                                   ChordName::Flat);

    myTonicKeys = new QButtonGroup(this);
    myTonicKeys->addButton(ui->tonicCButton, ChordName::C);
    myTonicKeys->addButton(ui->tonicDButton, ChordName::D);
    myTonicKeys->addButton(ui->tonicEButton, ChordName::E);
    myTonicKeys->addButton(ui->tonicFButton, ChordName::F);
    myTonicKeys->addButton(ui->tonicGButton, ChordName::G);
    myTonicKeys->addButton(ui->tonicAButton, ChordName::A);
    myTonicKeys->addButton(ui->tonicBButton, ChordName::B);
    connect(myTonicKeys,
#if (QT_VERSION >= QT_VERSION_CHECK(5,15,0))
            &QButtonGroup::idClicked,
#else
            qOverload<int>(&QButtonGroup::buttonClicked),
#endif
            this, &ChordNameDialog::onTonicChanged);

    myTonicKeys->button(myChord.getTonicKey())->setChecked(true);

    myBassKeys = new QButtonGroup(this);
    myBassKeys->addButton(ui->bassCButton, ChordName::C);
    myBassKeys->addButton(ui->bassDButton, ChordName::D);
    myBassKeys->addButton(ui->bassEButton, ChordName::E);
    myBassKeys->addButton(ui->bassFButton, ChordName::F);
    myBassKeys->addButton(ui->bassGButton, ChordName::G);
    myBassKeys->addButton(ui->bassAButton, ChordName::A);
    myBassKeys->addButton(ui->bassBButton, ChordName::B);
    connect(myBassKeys,
#if (QT_VERSION >= QT_VERSION_CHECK(5,15,0))
            &QButtonGroup::idClicked,
#else
            qOverload<int>(&QButtonGroup::buttonClicked),
#endif
            this, &ChordNameDialog::updateState);

    myBassKeys->button(myChord.getBassKey())->setChecked(true);

    ui->formulaListWidget->setCurrentIndex(myChord.getFormula());
    connect(ui->formulaListWidget,
            qOverload<int>(&QComboBox::currentIndexChanged), this,
            &ChordNameDialog::updateState);

    initCheckBox(ui->add2CheckBox,
                 myChord.hasModification(ChordName::Added2nd));
    initCheckBox(ui->add4CheckBox,
                 myChord.hasModification(ChordName::Added4th));
    initCheckBox(ui->add6CheckBox,
                 myChord.hasModification(ChordName::Added6th));
    initCheckBox(ui->add9CheckBox,
                 myChord.hasModification(ChordName::Added9th));
    initCheckBox(ui->add11CheckBox,
                 myChord.hasModification(ChordName::Added11th));

    initCheckBox(ui->extend9CheckBox,
                 myChord.hasModification(ChordName::Extended9th));
    initCheckBox(ui->extend11CheckBox,
                 myChord.hasModification(ChordName::Extended11th));
    initCheckBox(ui->extend13CheckBox,
                 myChord.hasModification(ChordName::Extended13th));

    initCheckBox(ui->flat5CheckBox,
                 myChord.hasModification(ChordName::Flatted5th));
    initCheckBox(ui->raised5CheckBox,
                 myChord.hasModification(ChordName::Raised5th));
    initCheckBox(ui->flat9CheckBox,
                 myChord.hasModification(ChordName::Flatted9th));
    initCheckBox(ui->raised9CheckBox,
                 myChord.hasModification(ChordName::Raised9th));
    initCheckBox(ui->raised11CheckBox,
                 myChord.hasModification(ChordName::Raised11th));
    initCheckBox(ui->flat13CheckBox,
                 myChord.hasModification(ChordName::Flatted13th));
    initCheckBox(ui->sus2CheckBox,
                 myChord.hasModification(ChordName::Suspended2nd));
    initCheckBox(ui->sus4CheckBox,
                 myChord.hasModification(ChordName::Suspended4th));

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
        static_cast<ChordName::Formula>(ui->formulaListWidget->currentIndex()));

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
        QString::fromStdString(Util::toString(myChord)));
}

void ChordNameDialog::onTonicVariationClicked(QAbstractButton *clickedButton)
{
    for (auto &button : myTonicVariations->buttons())
    {
        if (button != clickedButton)
            button->setChecked(false);
    }

    onTonicChanged();
}

void ChordNameDialog::onBassVariationClicked(QAbstractButton *clickedButton)
{
    for (auto &button : myBassVariations->buttons())
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

void ChordNameDialog::initCheckBox(QCheckBox *checkbox, bool value)
{
    checkbox->setChecked(value);
    connect(checkbox, &QCheckBox::clicked, this, &ChordNameDialog::updateState);
}
