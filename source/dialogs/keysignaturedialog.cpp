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
  
#include "keysignaturedialog.h"
#include "ui_keysignaturedialog.h"

#include <score/keysignature.h>
#include <util/tostring.h>

KeySignatureDialog::KeySignatureDialog(QWidget *parent,
                                       const KeySignature &currentKey)
    : QDialog(parent),
      ui(new Ui::KeySignatureDialog),
      myIsModified(false),
      myPreviousKey(currentKey)
{
    ui->setupUi(this);

    connect(ui->buttonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(ui->buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);

    // Initialize based on the original value of the key signature, and
    // create any signal connections
    if (currentKey.getKeyType() == KeySignature::Major)
        ui->majorKeyButton->setChecked(true);
    else
        ui->minorKeyButton->setChecked(true);

    connect(ui->majorKeyButton, &QAbstractButton::clicked, [=]() {
        populateKeyTypes(KeySignature::Major);
    });
    connect(ui->minorKeyButton, &QAbstractButton::clicked, [=]() {
        populateKeyTypes(KeySignature::Minor);
    });

    populateKeyTypes(currentKey.getKeyType());

    // Set the initial index for the keys combo box.
    int index = currentKey.getNumAccidentals();
    if (index > 0 && !currentKey.usesSharps())
        index += KeySignature::MAX_NUM_ACCIDENTALS;
    ui->keysComboBox->setCurrentIndex(index);

    ui->visibilityCheckBox->setChecked(currentKey.isVisible());

    connect(ui->majorKeyButton, &QAbstractButton::clicked, this,
            &KeySignatureDialog::handleModification);
    connect(ui->minorKeyButton, &QAbstractButton::clicked, this,
            &KeySignatureDialog::handleModification);
    connect(ui->keysComboBox, qOverload<int>(&QComboBox::currentIndexChanged),
            this, &KeySignatureDialog::handleModification);
}

KeySignatureDialog::~KeySignatureDialog()
{
    delete ui;
}

KeySignature KeySignatureDialog::getNewKey() const
{
    KeySignature key;
    key.setKeyType(ui->majorKeyButton->isChecked() ? KeySignature::Major
                                                   : KeySignature::Minor);
    key.setVisible(ui->visibilityCheckBox->isChecked());

    int numAccidentals = ui->keysComboBox->currentIndex();

    // Create a cancellation if necessary.
    if (numAccidentals == 0 && myPreviousKey.getNumAccidentals() > 0)
    {
        key.setCancellation();
        key.setNumAccidentals(myPreviousKey.getNumAccidentals());
        key.setSharps(myPreviousKey.usesSharps());
    }
    else
    {
        if (numAccidentals > KeySignature::MAX_NUM_ACCIDENTALS)
        {
            key.setSharps(false);
            numAccidentals -= KeySignature::MAX_NUM_ACCIDENTALS;
        }

        key.setNumAccidentals(numAccidentals);
    }

    return key;
}

void KeySignatureDialog::populateKeyTypes(KeySignature::KeyType type)
{
    // Store the original selected index, so we can reset it after repopulating
    // the list.
    int originalSelection = ui->keysComboBox->currentIndex();
    if (originalSelection == -1)
        originalSelection = 0;

    ui->keysComboBox->clear();

    KeySignature tempKey(type, 0, true);

    for (uint8_t i = 0; i <= KeySignature::MAX_NUM_ACCIDENTALS; ++i)
    {
        tempKey.setNumAccidentals(i);
        ui->keysComboBox->addItem(
            QString::fromStdString(Util::toString(tempKey)));
    }

    tempKey.setSharps(false);
    for (uint8_t i = 1; i <= KeySignature::MAX_NUM_ACCIDENTALS; ++i)
    {
        tempKey.setNumAccidentals(i);
        ui->keysComboBox->addItem(
            QString::fromStdString(Util::toString(tempKey)));
    }

    ui->keysComboBox->setCurrentIndex(originalSelection);
}

void KeySignatureDialog::handleModification()
{
    if (!myIsModified)
        ui->visibilityCheckBox->setChecked(true);

    myIsModified = true;
}
