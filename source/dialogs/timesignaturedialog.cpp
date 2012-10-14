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
  
#include "timesignaturedialog.h"
#include "ui_timesignaturedialog.h"

TimeSignatureDialog::TimeSignatureDialog(QWidget *parent, const TimeSignature &originalTimeSignature) :
    QDialog(parent),
    ui(new Ui::TimeSignatureDialog),
    newTimeSignature(originalTimeSignature)
{
    ui->setupUi(this);
    init();

    connect(ui->showTimeSignature, SIGNAL(toggled(bool)),
            this, SLOT(editTimeSignatureVisible(bool)));

    connect(ui->metronomePulses, SIGNAL(currentIndexChanged(int)),
            this, SLOT(editMetronomePulses(int)));

    connect(ui->cutTime, SIGNAL(toggled(bool)),
            this, SLOT(editCutTime(bool)));

    connect(ui->commonTime, SIGNAL(toggled(bool)),
            this, SLOT(editCommonTime(bool)));

    connect(ui->beatValue, SIGNAL(currentIndexChanged(int)),
            this, SLOT(editBeatValue(int)));

    connect(ui->beatsPerMeasure, SIGNAL(valueChanged(int)),
            this, SLOT(editBeatsPerMeasure(int)));
}

TimeSignatureDialog::~TimeSignatureDialog()
{
    delete ui;
}

/// Initialize widgets and set default values
void TimeSignatureDialog::init()
{
    ui->beatsPerMeasure->setMinimum(TimeSignature::MIN_BEATSPERMEASURE);
    ui->beatsPerMeasure->setMaximum(TimeSignature::MAX_BEATSPERMEASURE);

    boost::array<int, 5> beatValues = {{2, 4, 8, 16, 32}};
    for (size_t i = 0; i < beatValues.size(); i++)
    {
        ui->beatValue->addItem(QString::number(beatValues[i]), beatValues[i]);
    }

    beamingPatterns[0] = ui->beamingPattern1;
    beamingPatterns[1] = ui->beamingPattern2;
    beamingPatterns[2] = ui->beamingPattern3;
    beamingPatterns[3] = ui->beamingPattern4;

    for (size_t i = 0; i < beamingPatterns.size(); i++)
    {
        beamingPatterns[i]->setValidator(new QIntValidator(0, TimeSignature::MAX_BEATAMOUNT, 0));
    }

    ui->showTimeSignature->setChecked(newTimeSignature.IsShown());
    ui->commonTime->setChecked(newTimeSignature.IsCommonTime());
    ui->cutTime->setChecked(newTimeSignature.IsCutTime());

    ui->beatsPerMeasure->setValue(newTimeSignature.GetBeatsPerMeasure());
    ui->beatValue->setCurrentIndex(ui->beatValue->findData(newTimeSignature.GetBeatAmount()));

    updatePossiblePulseValues();
    ui->metronomePulses->setCurrentIndex(ui->metronomePulses->findData(newTimeSignature.GetPulses()));

    boost::array<uint8_t, 4> beamingPatternValues = {{0, 0, 0, 0}};
    newTimeSignature.GetBeamingPattern(beamingPatternValues[0], beamingPatternValues[1],
                                       beamingPatternValues[2], beamingPatternValues[3]);
    for (size_t i = 0; i < beamingPatterns.size(); i++)
    {
        beamingPatterns[i]->setText(QString::number(beamingPatternValues[i]));
    }
}

/// Update the options in the metronome pulses combo box (the available options
/// for metronome pulses depend on the number of beats in the measure).
void TimeSignatureDialog::updatePossiblePulseValues()
{
    ui->metronomePulses->clear();

    for (uint8_t i = TimeSignature::MIN_PULSES;
         i < TimeSignature::MAX_PULSES; ++i)
    {
        if (newTimeSignature.IsValidPulses(i))
        {
            ui->metronomePulses->addItem(QString::number(i), i);
        }
    }
}

/// Returns a new time signature using the values selected in the dialog
TimeSignature TimeSignatureDialog::getNewTimeSignature() const
{
    return newTimeSignature;
}

/// Save the new beaming pattern values.
void TimeSignatureDialog::accept()
{
    boost::array<uint8_t, 4> beamingPatternValues = {{0, 0, 0, 0}};
    for (size_t i = 0; i < beamingPatterns.size(); ++i)
    {
        QLineEdit* value = beamingPatterns[i];
        if (value->isEnabled())
        {
            beamingPatternValues[i] = value->text().toUInt();
        }
    }

    newTimeSignature.SetBeamingPattern(
                beamingPatternValues[0],beamingPatternValues[1],
                beamingPatternValues[2], beamingPatternValues[3]);

    done(Accepted);
}

/// Toggles whether or not the time signature is visible
void TimeSignatureDialog::editTimeSignatureVisible(bool isVisible)
{
    newTimeSignature.SetShown(isVisible);
}

/// @param selectedIndex - index in the combo box of the selected pulses value
void TimeSignatureDialog::editMetronomePulses(int selectedIndex)
{
    newTimeSignature.SetPulses(ui->metronomePulses->itemData(selectedIndex).toUInt());
}

/// Toggles whether the time signature uses cut time
void TimeSignatureDialog::editCutTime(bool enabled)
{
    if (enabled)
    {
        newTimeSignature.SetCutTime();
    }
    else
    {
        // reset to a regular time signature
        newTimeSignature.SetMeter(ui->beatsPerMeasure->value(),
                                  ui->beatValue->itemData(ui->beatValue->currentIndex()).toUInt());
    }

    ui->beatsPerMeasure->setEnabled(!enabled);
    ui->beatValue->setEnabled(!enabled);
}

/// @param selectedIndex - index in the combo box of the selected beat value
void TimeSignatureDialog::editBeatValue(int selectedIndex)
{
    newTimeSignature.SetBeatAmount(ui->beatValue->itemData(selectedIndex).toUInt());
}

/// Update the number of beats in the measure
///  - This will also update the available values for metronome pulses
void TimeSignatureDialog::editBeatsPerMeasure(int numBeats)
{
    newTimeSignature.SetBeatsPerMeasure(numBeats);

    // update the metronome pulses combo box
    updatePossiblePulseValues();
    ui->metronomePulses->setCurrentIndex(ui->metronomePulses->findData(numBeats));
}

/// Toggles whether the time signature uses common time
void TimeSignatureDialog::editCommonTime(bool enabled)
{
    if (enabled)
    {
        newTimeSignature.SetCommonTime();
    }
    else
    {
        // reset to a regular time signature
        newTimeSignature.SetMeter(ui->beatsPerMeasure->value(),
                                  ui->beatValue->itemData(ui->beatValue->currentIndex()).toUInt());
    }

    ui->beatsPerMeasure->setEnabled(!enabled);
    ui->beatValue->setEnabled(!enabled);
}
