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
  
#include "barlinedialog.h"

#include <powertabdocument/barline.h>

#include <QDialogButtonBox>
#include <QVBoxLayout>
#include <QFormLayout>
#include <QSpinBox>
#include <QComboBox>

/// Constructor: the barType and repeats references will be updated with the values that the user selects
BarlineDialog::BarlineDialog(quint8& barType, quint8& repeats) :
    barType(barType),
    repeats(repeats)
{
    setWindowTitle(tr("Music Bar"));
    setModal(true);

    QDialogButtonBox* buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);

    repeatCount = new QSpinBox;

    barLineType = new QComboBox;
    barLineType->addItem(tr("Single"));
    barLineType->addItem(tr("Double"));
    barLineType->addItem(tr("Free Time"));
    barLineType->addItem(tr("Repeat Start"));
    barLineType->addItem(tr("Repeat End"));
    barLineType->addItem(tr("Double Bar Fine"));

    connect(barLineType, SIGNAL(currentIndexChanged(int)), this, SLOT(disableRepeatCount(int)));

    QFormLayout* formLayout = new QFormLayout;
    formLayout->addRow(tr("&Type:"), barLineType);
    formLayout->addRow(tr("&Play:"), repeatCount);

    QVBoxLayout *buttonsLayout = new QVBoxLayout;
    buttonsLayout->addStretch(1);
    buttonsLayout->addLayout(formLayout);
    buttonsLayout->addWidget(buttonBox);

    buttonsLayout->setSizeConstraint(QLayout::SetFixedSize);
    setLayout(buttonsLayout);

    init();
    disableRepeatCount(barLineType->currentIndex());

    connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
    connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
}

void BarlineDialog::init()
{
    // set limits for repeat count
    repeatCount->setMinimum(Barline::MIN_REPEAT_COUNT);
    repeatCount->setMaximum(Barline::MAX_REPEAT_COUNT);

    // set default repeat count
    repeatCount->setValue(repeats);

    // set bar line type choice
    barLineType->setCurrentIndex(barType);
}

void BarlineDialog::disableRepeatCount(int newBarlineType)
{
    if (newBarlineType == Barline::repeatEnd)
    {
        repeatCount->setEnabled(true);
    }
    else
    {
        repeatCount->setEnabled(false);
    }
}

void BarlineDialog::accept()
{
    barType = barLineType->currentIndex();
    repeats = repeatCount->value();

    done(QDialog::Accepted);
}
