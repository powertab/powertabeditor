/*
  * Copyright (C) 2012 Cameron White
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

#include "tempomarkerdialog.h"
#include "ui_tempomarkerdialog.h"

#include <QCompleter>
#include <QButtonGroup>

TempoMarkerDialog::TempoMarkerDialog(QWidget* parent) :
    QDialog(parent),
    ui(new Ui::TempoMarkerDialog),
    beatTypes(new QButtonGroup(this)),
    listessoBeatTypes(new QButtonGroup(this)),
    tripletFeelTypes(new QButtonGroup(this))
{
    ui->setupUi(this);

    QStringList descriptions;
    descriptions << "Fast Rock" << "Faster" << "Moderate Rock" << "Moderately"
                 << "Moderately Fast Rock" << "Moderately Slow Funk"
                 << "Moderately Slow Rock" << "Slow Blues" << "Slow Rock"
                 << "Slower" << "Slowly";
    ui->descriptionComboBox->addItems(descriptions);

    // Autocomplete for description choices.
    QCompleter* completer = new QCompleter(descriptions, this);
    completer->setCaseSensitivity(Qt::CaseInsensitive);
    ui->descriptionComboBox->setCompleter(completer);
    ui->descriptionComboBox->clearEditText();

    // Prevent multiple beat types from being selected at once.
    beatTypes->addButton(ui->note2Button, TempoMarker::half);
    beatTypes->addButton(ui->dottedNote2Button, TempoMarker::halfDotted);
    beatTypes->addButton(ui->note4Button, TempoMarker::quarter);
    beatTypes->addButton(ui->dottedNote4Button, TempoMarker::quarterDotted);
    beatTypes->addButton(ui->note8Button, TempoMarker::eighth);
    beatTypes->addButton(ui->dottedNote8Button, TempoMarker::eighthDotted);
    beatTypes->addButton(ui->note16Button, TempoMarker::sixteenth);
    beatTypes->addButton(ui->dottedNote16Button, TempoMarker::sixteenDotted);
    beatTypes->addButton(ui->note32Button, TempoMarker::thirtySecond);
    beatTypes->addButton(ui->dottedNote32Button,
                         TempoMarker::thirtySecondDotted);
    ui->note4Button->setChecked(true);

    // Set the bpm range.
    ui->bpmSpinBox->setMinimum(40);
    ui->bpmSpinBox->setMaximum(300);
    ui->bpmSpinBox->setValue(120);

    // Prevent multiple listesso beat types from being selected at once.
    listessoBeatTypes->addButton(ui->listessoNote2Button, TempoMarker::half);
    listessoBeatTypes->addButton(ui->listessoDottedNote2Button,
                                 TempoMarker::halfDotted);
    listessoBeatTypes->addButton(ui->listessoNote4Button, TempoMarker::quarter);
    listessoBeatTypes->addButton(ui->listessoDottedNote4Button,
                                 TempoMarker::quarterDotted);
    listessoBeatTypes->addButton(ui->listessoNote8Button, TempoMarker::eighth);
    listessoBeatTypes->addButton(ui->listessoDottedNote8Button,
                                 TempoMarker::eighthDotted);
    listessoBeatTypes->addButton(ui->listessoNote16Button,
                                 TempoMarker::sixteenth);
    listessoBeatTypes->addButton(ui->listessoDottedNote16Button,
                                 TempoMarker::sixteenDotted);
    listessoBeatTypes->addButton(ui->listessoNote32Button,
                                 TempoMarker::thirtySecond);
    listessoBeatTypes->addButton(ui->listessoDottedNote32Button,
                                 TempoMarker::thirtySecondDotted);
    ui->listessoNote2Button->setChecked(true);

    // Prevent triplet feel types from being selected at once.
    tripletFeelTypes->addButton(ui->tripletFeelNoneCheckBox,
                                TempoMarker::noTripletFeel);
    tripletFeelTypes->addButton(ui->tripletFeel8thCheckBox,
                                TempoMarker::tripletFeelEighth);
    tripletFeelTypes->addButton(ui->tripletFeel8thOffCheckBox,
                                TempoMarker::tripletFeelEighthOff);
    tripletFeelTypes->addButton(ui->tripletFeel16thCheckBox,
                                TempoMarker::tripletFeelSixteenth);
    tripletFeelTypes->addButton(ui->tripletFeel16thOffCheckBox,
                                TempoMarker::tripletFeelSixteenthOff);
    ui->tripletFeelNoneCheckBox->setChecked(true);

    connect(ui->enableListessoCheckBox, SIGNAL(clicked(bool)),
            this, SLOT(onListessoChanged(bool)));
    ui->enableListessoCheckBox->setChecked(false);
    onListessoChanged(false);

    connect(ui->showMetronomeMarkerCheckBox, SIGNAL(clicked(bool)),
            this, SLOT(onShowMetronomeMarkerChanged(bool)));
    ui->showMetronomeMarkerCheckBox->setChecked(true);

    ui->descriptionComboBox->setFocus();
}

TempoMarkerDialog::~TempoMarkerDialog()
{
    delete ui;
}

/// Returns the type of the tempo marker.
TempoMarker::Type TempoMarkerDialog::markerType() const
{
    if (!ui->showMetronomeMarkerCheckBox->isChecked())
    {
        return TempoMarker::notShown;
    }
    else if (ui->enableListessoCheckBox->isChecked())
    {
        return TempoMarker::listesso;
    }
    else
    {
        return TempoMarker::standardMarker;
    }
}

/// Returns the beat type that was selected.
TempoMarker::BeatType TempoMarkerDialog::beatType() const
{
    return static_cast<TempoMarker::BeatType>(beatTypes->checkedId());
}

/// Returns the listesso beat type that was selected.
TempoMarker::BeatType TempoMarkerDialog::listessoBeatType() const
{
    return static_cast<TempoMarker::BeatType>(listessoBeatTypes->checkedId());
}

/// Returns the triplet feel type that was selected.
TempoMarker::TripletFeelType TempoMarkerDialog::tripletFeelType() const
{
    return static_cast<TempoMarker::TripletFeelType>(tripletFeelTypes->checkedId());
}

/// Returns the tempo marker description.
std::string TempoMarkerDialog::description() const
{
    return ui->descriptionComboBox->currentText().toStdString();
}

/// Returns the beats per minute value that was selected.
int TempoMarkerDialog::beatsPerMinute() const
{
    return ui->bpmSpinBox->value();
}

/// Disable the BPM spinner if listesso is enabled.
void TempoMarkerDialog::onListessoChanged(bool enabled)
{
    foreach (QAbstractButton* button, listessoBeatTypes->buttons())
    {
        button->setEnabled(enabled);
    }

    ui->bpmSpinBox->setEnabled(!enabled);
}

/// Disable the beat types, BPM spinner, and listesso beat types if the
/// metronome marker will be hidden.
void TempoMarkerDialog::onShowMetronomeMarkerChanged(bool enabled)
{
    QList<QAbstractButton*> buttons;
    buttons << beatTypes->buttons() << listessoBeatTypes->buttons();

    foreach (QAbstractButton* button, buttons)
    {
        button->setEnabled(enabled);
    }

    ui->bpmSpinBox->setEnabled(enabled);

    // Keep the state of the listesso buttons consistent.
    onListessoChanged(ui->enableListessoCheckBox->isChecked());
}
