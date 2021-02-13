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

#include <QButtonGroup>
#include <QCompleter>

TempoMarkerDialog::TempoMarkerDialog(QWidget *parent,
                                     const TempoMarker *marker)
    : QDialog(parent),
      ui(new Ui::TempoMarkerDialog),
      myBeatTypes(new QButtonGroup(this)),
      myListessoBeatTypes(new QButtonGroup(this)),
      myTripletFeelTypes(new QButtonGroup(this))
{
    ui->setupUi(this);

    connect(ui->buttonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(ui->buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);

    QStringList descriptions = { QStringLiteral("Fast Rock"),
                                 QStringLiteral("Faster"),
                                 QStringLiteral("Moderate Rock"),
                                 QStringLiteral("Moderately"),
                                 QStringLiteral("Moderately Fast Rock"),
                                 QStringLiteral("Moderately Slow Funk"),
                                 QStringLiteral("Moderately Slow Rock"),
                                 QStringLiteral("Slow Blues"),
                                 QStringLiteral("Slow Rock"),
                                 QStringLiteral("Slower"),
                                 QStringLiteral("Slowly") };
    ui->descriptionComboBox->addItems(descriptions);

    // Autocomplete for description choices.
    auto completer = new QCompleter(descriptions, this);
    completer->setCaseSensitivity(Qt::CaseInsensitive);
    ui->descriptionComboBox->setCompleter(completer);

    if (marker)
    {
        ui->descriptionComboBox->setCurrentText(
            QString::fromStdString(marker->getDescription()));
    }
    else
        ui->descriptionComboBox->clearEditText();

    // Prevent multiple beat types from being selected at once.
    myBeatTypes->addButton(ui->note2Button, TempoMarker::Half);
    myBeatTypes->addButton(ui->dottedNote2Button, TempoMarker::HalfDotted);
    myBeatTypes->addButton(ui->note4Button, TempoMarker::Quarter);
    myBeatTypes->addButton(ui->dottedNote4Button, TempoMarker::QuarterDotted);
    myBeatTypes->addButton(ui->note8Button, TempoMarker::Eighth);
    myBeatTypes->addButton(ui->dottedNote8Button, TempoMarker::EighthDotted);
    myBeatTypes->addButton(ui->note16Button, TempoMarker::Sixteenth);
    myBeatTypes->addButton(ui->dottedNote16Button, TempoMarker::SixteenthDotted);
    myBeatTypes->addButton(ui->note32Button, TempoMarker::ThirtySecond);
    myBeatTypes->addButton(ui->dottedNote32Button,
                           TempoMarker::ThirtySecondDotted);
    auto beat_type = marker ? marker->getBeatType() : TempoMarker::Quarter;
    myBeatTypes->button(beat_type)->setChecked(true);

    // Set the bpm range.
    ui->bpmSpinBox->setMinimum(TempoMarker::MIN_BEATS_PER_MINUTE);
    ui->bpmSpinBox->setMaximum(TempoMarker::MAX_BEATS_PER_MINUTE);
    ui->bpmSpinBox->setValue(marker ? marker->getBeatsPerMinute()
                                    : TempoMarker::DEFAULT_BEATS_PER_MINUTE);

    // Prevent multiple listesso beat types from being selected at once.
    myListessoBeatTypes->addButton(ui->listessoNote2Button, TempoMarker::Half);
    myListessoBeatTypes->addButton(ui->listessoDottedNote2Button,
                                 TempoMarker::HalfDotted);
    myListessoBeatTypes->addButton(ui->listessoNote4Button, TempoMarker::Quarter);
    myListessoBeatTypes->addButton(ui->listessoDottedNote4Button,
                                 TempoMarker::QuarterDotted);
    myListessoBeatTypes->addButton(ui->listessoNote8Button, TempoMarker::Eighth);
    myListessoBeatTypes->addButton(ui->listessoDottedNote8Button,
                                 TempoMarker::EighthDotted);
    myListessoBeatTypes->addButton(ui->listessoNote16Button,
                                 TempoMarker::Sixteenth);
    myListessoBeatTypes->addButton(ui->listessoDottedNote16Button,
                                 TempoMarker::SixteenthDotted);
    myListessoBeatTypes->addButton(ui->listessoNote32Button,
                                 TempoMarker::ThirtySecond);
    myListessoBeatTypes->addButton(ui->listessoDottedNote32Button,
                                 TempoMarker::ThirtySecondDotted);
    const auto listesso_type =
        marker ? marker->getListessoBeatType() : TempoMarker::Half;
    myListessoBeatTypes->button(listesso_type)->setChecked(true);

    // Prevent triplet feel types from being selected at once.
    myTripletFeelTypes->addButton(ui->tripletFeelNoneCheckBox,
                                TempoMarker::NoTripletFeel);
    myTripletFeelTypes->addButton(ui->tripletFeel8thCheckBox,
                                TempoMarker::TripletFeelEighth);
    myTripletFeelTypes->addButton(ui->tripletFeel8thOffCheckBox,
                                TempoMarker::TripletFeelEighthOff);
    myTripletFeelTypes->addButton(ui->tripletFeel16thCheckBox,
                                TempoMarker::TripletFeelSixteenth);
    myTripletFeelTypes->addButton(ui->tripletFeel16thOffCheckBox,
                                TempoMarker::TripletFeelSixteenthOff);
    const auto triplet_feel =
        marker ? marker->getTripletFeel() : TempoMarker::NoTripletFeel;
    myTripletFeelTypes->button(triplet_feel)->setChecked(true);

    connect(ui->enableListessoCheckBox, &QCheckBox::clicked, this,
            &TempoMarkerDialog::onListessoChanged);
    const bool listesso =
        marker ? marker->getMarkerType() == TempoMarker::ListessoMarker : false;
    ui->enableListessoCheckBox->setChecked(listesso);
    onListessoChanged(listesso);

    connect(ui->showMetronomeMarkerCheckBox, &QCheckBox::clicked, this,
            &TempoMarkerDialog::onShowMetronomeMarkerChanged);
    ui->showMetronomeMarkerCheckBox->setChecked(
        marker ? marker->getMarkerType() != TempoMarker::NotShown : true);

    ui->descriptionComboBox->setFocus();
}

TempoMarkerDialog::~TempoMarkerDialog()
{
    delete ui;
}

TempoMarker TempoMarkerDialog::getTempoMarker() const
{
    TempoMarker marker;

    marker.setBeatType(static_cast<TempoMarker::BeatType>(
                           myBeatTypes->checkedId()));
    marker.setListessoBeatType(static_cast<TempoMarker::BeatType>(
                                   myListessoBeatTypes->checkedId()));
    marker.setTripletFeel(static_cast<TempoMarker::TripletFeelType>(
                              myTripletFeelTypes->checkedId()));
    marker.setDescription(ui->descriptionComboBox->currentText().toStdString());
    marker.setBeatsPerMinute(ui->bpmSpinBox->value());

    if (!ui->showMetronomeMarkerCheckBox->isChecked())
        marker.setMarkerType(TempoMarker::NotShown);
    else if (ui->enableListessoCheckBox->isChecked())
        marker.setMarkerType(TempoMarker::ListessoMarker);
    else
        marker.setMarkerType(TempoMarker::StandardMarker);

    return marker;
}

void TempoMarkerDialog::onListessoChanged(bool enabled)
{
    for (auto &button : myListessoBeatTypes->buttons())
        button->setEnabled(enabled);

    ui->bpmSpinBox->setEnabled(!enabled);
}

void TempoMarkerDialog::onShowMetronomeMarkerChanged(bool enabled)
{
    QList<QAbstractButton *> buttons;
    buttons << myBeatTypes->buttons() << myListessoBeatTypes->buttons();

    for (auto &button : buttons)
        button->setEnabled(enabled);

    ui->bpmSpinBox->setEnabled(enabled);

    // Keep the state of the listesso buttons consistent.
    onListessoChanged(ui->enableListessoCheckBox->isChecked());
}
