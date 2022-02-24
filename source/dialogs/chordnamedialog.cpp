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
                                 const ChordName &initial_chord,
                                 std::vector<ChordName> available_chords)
    : QDialog(parent),
      ui(new Ui::ChordNameDialog),
      myScoreChords(std::move(available_chords))
{
    ui->setupUi(this);

    for (const ChordName &name : myScoreChords)
        ui->chordsList->addItem(QString::fromStdString(Util::toString(name)));

    connect(ui->chordsList, &QListWidget::currentRowChanged,
            [&](int row)
            {
                if (row >= 0)
                    setToChord(myScoreChords[row]);
            });

    connect(ui->buttonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(ui->buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);

    initCheckBox(ui->noChordCheckBox);
    initCheckBox(ui->bracketsCheckBox);

    myTonicVariations = new QButtonGroup(this);
    myTonicVariations->addButton(ui->tonicFlatButton);
    myTonicVariations->addButton(ui->tonicSharpButton);
    // Allow none of the buttons to be clicked, which requires a little extra
    // work.
    myTonicVariations->setExclusive(false);
    connect(myTonicVariations,
            qOverload<QAbstractButton *>(&QButtonGroup::buttonClicked), this,
            &ChordNameDialog::onTonicVariationClicked);

    myBassVariations = new QButtonGroup(this);
    myBassVariations->addButton(ui->bassFlatButton);
    myBassVariations->addButton(ui->bassSharpButton);
    myBassVariations->setExclusive(false);
    connect(myBassVariations,
            qOverload<QAbstractButton *>(&QButtonGroup::buttonClicked), this,
            &ChordNameDialog::onBassVariationClicked);

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

    connect(ui->formulaListWidget,
            qOverload<int>(&QComboBox::currentIndexChanged), this,
            &ChordNameDialog::updateState);

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
    initCheckBox(ui->flat6CheckBox);
    initCheckBox(ui->flat9CheckBox);
    initCheckBox(ui->raised9CheckBox);
    initCheckBox(ui->raised11CheckBox);
    initCheckBox(ui->flat13CheckBox);
    initCheckBox(ui->sus2CheckBox);
    initCheckBox(ui->sus4CheckBox);

    setToChord(initial_chord);
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
    myChord.setModification(ChordName::Flatted6th,
                            ui->flat6CheckBox->isChecked());
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

void ChordNameDialog::initCheckBox(QCheckBox *checkbox)
{
    connect(checkbox, &QCheckBox::clicked, this, &ChordNameDialog::updateState);
}

void
ChordNameDialog::setToChord(const ChordName &chord)
{
    ui->noChordCheckBox->setChecked(chord.isNoChord());
    ui->bracketsCheckBox->setChecked(chord.hasBrackets());

    ui->tonicSharpButton->setChecked(chord.getTonicVariation() ==
                                     ChordName::Sharp);
    ui->tonicFlatButton->setChecked(chord.getTonicVariation() ==
                                    ChordName::Flat);

    ui->bassSharpButton->setChecked(chord.getBassVariation() ==
                                    ChordName::Sharp);
    ui->bassFlatButton->setChecked(chord.getBassVariation() == ChordName::Flat);

    myTonicKeys->button(chord.getTonicKey())->setChecked(true);
    myBassKeys->button(chord.getBassKey())->setChecked(true);

    ui->formulaListWidget->setCurrentIndex(chord.getFormula());

    ui->add2CheckBox->setChecked(chord.hasModification(ChordName::Added2nd));
    ui->add4CheckBox->setChecked(chord.hasModification(ChordName::Added4th));
    ui->add6CheckBox->setChecked(chord.hasModification(ChordName::Added6th));
    ui->add9CheckBox->setChecked(chord.hasModification(ChordName::Added9th));
    ui->add11CheckBox->setChecked(chord.hasModification(ChordName::Added11th));

    ui->extend9CheckBox->setChecked(
        chord.hasModification(ChordName::Extended9th));
    ui->extend11CheckBox->setChecked(
        chord.hasModification(ChordName::Extended11th));
    ui->extend13CheckBox->setChecked(
        chord.hasModification(ChordName::Extended13th));

    ui->flat5CheckBox->setChecked(chord.hasModification(ChordName::Flatted5th));
    ui->raised5CheckBox->setChecked(
        chord.hasModification(ChordName::Raised5th));
    ui->flat6CheckBox->setChecked(chord.hasModification(ChordName::Flatted6th));
    ui->flat9CheckBox->setChecked(chord.hasModification(ChordName::Flatted9th));
    ui->raised9CheckBox->setChecked(
        chord.hasModification(ChordName::Raised9th));
    ui->raised11CheckBox->setChecked(
        chord.hasModification(ChordName::Raised11th));
    ui->flat13CheckBox->setChecked(
        chord.hasModification(ChordName::Flatted13th));
    ui->sus2CheckBox->setChecked(
        chord.hasModification(ChordName::Suspended2nd));
    ui->sus4CheckBox->setChecked(
        chord.hasModification(ChordName::Suspended4th));

    updateState();
}
