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

#include <QMessageBox>

TimeSignatureDialog::TimeSignatureDialog(
        QWidget *parent, const TimeSignature &currentTimeSignature)
    : QDialog(parent),
      ui(new Ui::TimeSignatureDialog),
      myTimeSignature(currentTimeSignature)
{
    ui->setupUi(this);

    connect(ui->buttonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(ui->buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);

    ui->beatsPerMeasure->setMinimum(TimeSignature::MIN_BEATS_PER_MEASURE);
    ui->beatsPerMeasure->setMaximum(TimeSignature::MAX_BEATS_PER_MEASURE);

    for (int i = 0; i <= 64; ++i)
    {
        if (TimeSignature::isValidBeatValue(i))
            ui->beatValue->addItem(QString::number(i), i);
    }

    myBeamingPatterns[0] = ui->beamingPattern1;
    myBeamingPatterns[1] = ui->beamingPattern2;
    myBeamingPatterns[2] = ui->beamingPattern3;
    myBeamingPatterns[3] = ui->beamingPattern4;

    for (auto &pattern : myBeamingPatterns)
    {
        pattern->setValidator(new QIntValidator(0, 64, pattern));
    }

    myTimeSignature.setVisible(true);   // make the time signature visible by default

    ui->showTimeSignature->setChecked(myTimeSignature.isVisible());
    ui->commonTime->setChecked(myTimeSignature.getMeterType() ==
                               TimeSignature::CommonTime);
    ui->cutTime->setChecked(myTimeSignature.getMeterType() ==
                            TimeSignature::CutTime);

    ui->beatsPerMeasure->setValue(myTimeSignature.getBeatsPerMeasure());
    ui->beatValue->setCurrentIndex(ui->beatValue->findData(
                                       myTimeSignature.getBeatValue()));

    updatePossiblePulseValues();
    ui->metronomePulses->setCurrentIndex(
                ui->metronomePulses->findData(myTimeSignature.getNumPulses()));

    TimeSignature::BeamingPattern pattern = myTimeSignature.getBeamingPattern();
    for (size_t i = 0; i < myBeamingPatterns.size(); i++)
        myBeamingPatterns[i]->setText(QString::number(pattern[i]));

    connect(ui->showTimeSignature, &QCheckBox::toggled, this,
            &TimeSignatureDialog::editTimeSignatureVisible);

    connect(ui->metronomePulses,
            qOverload<int>(&QComboBox::currentIndexChanged), this,
            &TimeSignatureDialog::editMetronomePulses);

    connect(ui->cutTime, &QCheckBox::toggled, this,
            &TimeSignatureDialog::editCutTime);

    connect(ui->commonTime, &QCheckBox::toggled, this,
            &TimeSignatureDialog::editCommonTime);

    connect(ui->beatValue, qOverload<int>(&QComboBox::currentIndexChanged),
            this, &TimeSignatureDialog::editBeatValue);

    connect(ui->beatsPerMeasure, qOverload<int>(&QSpinBox::valueChanged), this,
            &TimeSignatureDialog::editBeatsPerMeasure);
}

TimeSignatureDialog::~TimeSignatureDialog()
{
    delete ui;
}

void TimeSignatureDialog::updatePossiblePulseValues()
{
    ui->metronomePulses->clear();

    for (int i = TimeSignature::MIN_PULSES;
         i < TimeSignature::MAX_PULSES; ++i)
    {
        if (myTimeSignature.isValidNumPulses(i))
        {
            ui->metronomePulses->addItem(QString::number(i), i);
        }
    }
}

TimeSignature TimeSignatureDialog::getTimeSignature() const
{
    return myTimeSignature;
}

void TimeSignatureDialog::accept()
{
    // Save the new beaming pattern values.
    TimeSignature::BeamingPattern pattern = {{0, 0, 0, 0}};
    for (size_t i = 0; i < myBeamingPatterns.size(); ++i)
    {
        QLineEdit *value = myBeamingPatterns[i];
        if (value->isEnabled())
            pattern[i] = value->text().toUInt();
    }

    if (pattern[0] == 0)
    {
        QMessageBox msgBox(this);
        msgBox.setIcon(QMessageBox::Warning);
        msgBox.setWindowTitle(tr("Time Signature"));
        msgBox.setText(tr("Invalid beaming pattern."));
        msgBox.exec();
        return;
    }

    myTimeSignature.setBeamingPattern(pattern);
    done(Accepted);
}

void TimeSignatureDialog::editTimeSignatureVisible(bool isVisible)
{
    myTimeSignature.setVisible(isVisible);
}

void TimeSignatureDialog::editMetronomePulses(int selectedIndex)
{
    myTimeSignature.setNumPulses(
                ui->metronomePulses->itemData(selectedIndex).toUInt());
}

void TimeSignatureDialog::editCutTime(bool enabled)
{
    if (enabled)
        myTimeSignature.setMeterType(TimeSignature::CutTime);
    else
    {
        // Reset to a regular time signature.
        myTimeSignature.setMeterType(TimeSignature::Normal);
        myTimeSignature.setBeatsPerMeasure(ui->beatsPerMeasure->value());
        myTimeSignature.setBeatValue(
                    ui->beatValue->itemData(ui->beatValue->currentIndex()).toUInt());
    }

    ui->beatsPerMeasure->setEnabled(!enabled);
    ui->beatValue->setEnabled(!enabled);
}

void TimeSignatureDialog::editBeatValue(int selectedIndex)
{
    myTimeSignature.setBeatValue(
                ui->beatValue->itemData(selectedIndex).toUInt());
}

void TimeSignatureDialog::editBeatsPerMeasure(int beats)
{
    myTimeSignature.setBeatsPerMeasure(beats);

    // Update the metronome pulses combo box.
    updatePossiblePulseValues();
    ui->metronomePulses->setCurrentIndex(ui->metronomePulses->findData(beats));
}

void TimeSignatureDialog::editCommonTime(bool enabled)
{
    if (enabled)
        myTimeSignature.setMeterType(TimeSignature::CommonTime);
    else
    {
        // Reset to a regular time signature.
        myTimeSignature.setMeterType(TimeSignature::Normal);
        myTimeSignature.setBeatsPerMeasure(ui->beatsPerMeasure->value());
        myTimeSignature.setBeatValue(
                    ui->beatValue->itemData(ui->beatValue->currentIndex()).toUInt());
    }

    ui->beatsPerMeasure->setEnabled(!enabled);
    ui->beatValue->setEnabled(!enabled);
}
