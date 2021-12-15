/*
  * Copyright (C) 2014 Cameron White
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

#include "benddialog.h"
#include "ui_benddialog.h"

#include <score/note.h>

#include <QButtonGroup>

BendDialog::BendDialog(QWidget *parent, const Bend *current_bend)
    : QDialog(parent), ui(new Ui::BendDialog)
{
    ui->setupUi(this);

    connect(ui->buttonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(ui->buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);

    ui->bendTypeComboBox->addItems({
        tr("Bend"), tr("Bend and Release"), tr("Bend and Hold"),
        tr("Pre-Bend"), tr("Pre-Bend and Release"), tr("Pre-Bend and Hold"),
        tr("Gradual Release"), tr("Immediate Release")
    });

    initBendPitches();

    myDurationButtonGroup = new QButtonGroup(this);
    myDurationButtonGroup->addButton(ui->defaultDurationButton);
    myDurationButtonGroup->addButton(ui->customDurationButton);
    ui->defaultDurationButton->setChecked(true);

    initDrawPoints(ui->vertStartingPointComboBox);
    initDrawPoints(ui->vertEndingPointComboBox);

    if (current_bend)
    {
        ui->bendTypeComboBox->setCurrentIndex(current_bend->getType());
        ui->bentPitchComboBox->setCurrentIndex(
            ui->bentPitchComboBox->findData(current_bend->getBentPitch()));
        ui->releasePitchComboBox->setCurrentIndex(
            ui->releasePitchComboBox->findData(
                current_bend->getReleasePitch()));

        if (current_bend->getDuration() == 0)
            ui->defaultDurationButton->setChecked(true);
        else
        {
            ui->customDurationButton->setChecked(true);
            ui->bendDurationSpinBox->setValue(current_bend->getDuration() - 1);
        }

        ui->vertStartingPointComboBox->setCurrentIndex(
            current_bend->getStartPoint());
        ui->vertEndingPointComboBox->setCurrentIndex(
            current_bend->getEndPoint());
    }

    connect(ui->bendTypeComboBox,
            qOverload<int>(&QComboBox::currentIndexChanged), this,
            &BendDialog::handleBendTypeChanged);
    handleBendTypeChanged();
}

BendDialog::~BendDialog() { delete ui; }

Bend BendDialog::getBend() const
{
    return Bend(
        static_cast<Bend::BendType>(ui->bendTypeComboBox->currentIndex()),
        ui->bentPitchComboBox->itemData(ui->bentPitchComboBox->currentIndex()).toInt(),
        ui->releasePitchComboBox->itemData(ui->releasePitchComboBox->currentIndex()).toInt(),
        ui->defaultDurationButton->isChecked() ? 0 : ui->bendDurationSpinBox->value() + 1,
        static_cast<Bend::DrawPoint>(
            ui->vertStartingPointComboBox->currentIndex()),
        static_cast<Bend::DrawPoint>(
            ui->vertEndingPointComboBox->currentIndex()));
}

void BendDialog::initBendPitches()
{
    for (int i = 0; i <= 12; ++i)
    {
        const QString text = QString::fromStdString(Bend::getPitchText(i));

        if (i != 0)
            ui->bentPitchComboBox->addItem(text, i);
        ui->releasePitchComboBox->addItem(text, i);
    }

    // Default bend pitch is "Full".
    ui->bentPitchComboBox->setCurrentIndex(3);
}

void BendDialog::initDrawPoints(QComboBox *c)
{
    c->addItem(tr("Low (Tablature Staff Line)"));
    c->addItem(tr("Middle"));
    c->addItem(tr("High"));
}

void BendDialog::handleBendTypeChanged()
{
    const Bend::BendType bendType =
        static_cast<Bend::BendType>(ui->bendTypeComboBox->currentIndex());

    // First, reset everything.
    ui->bentPitchComboBox->setEnabled(true);
    ui->releasePitchComboBox->setEnabled(true);
    for (auto &button : myDurationButtonGroup->buttons())
        button->setEnabled(true);

    // Enable or disable some of the options depending on the active bend type.
    switch (bendType)
    {
        case Bend::PreBend:
        case Bend::PreBendAndHold:
            for (auto &button : myDurationButtonGroup->buttons())
                button->setDisabled(true);

            [[fallthrough]];

        case Bend::NormalBend:
        case Bend::BendAndHold:
            ui->releasePitchComboBox->setDisabled(true);
            ui->vertStartingPointComboBox->setCurrentIndex(Bend::LowPoint);
            ui->vertEndingPointComboBox->setCurrentIndex(Bend::MidPoint);
            break;

        case Bend::PreBendAndRelease:
        case Bend::BendAndRelease:
            for (auto &button : myDurationButtonGroup->buttons())
                button->setDisabled(true);
            ui->vertStartingPointComboBox->setCurrentIndex(Bend::LowPoint);
            ui->vertEndingPointComboBox->setCurrentIndex(Bend::LowPoint);
            break;

        case Bend::ImmediateRelease:
            for (auto &button : myDurationButtonGroup->buttons())
                button->setDisabled(true);
            ui->releasePitchComboBox->setDisabled(true);

            [[fallthrough]];

        case Bend::GradualRelease:
            ui->bentPitchComboBox->setDisabled(true);
            ui->vertStartingPointComboBox->setCurrentIndex(Bend::MidPoint);
            ui->vertEndingPointComboBox->setCurrentIndex(Bend::LowPoint);
            break;
    }
}
