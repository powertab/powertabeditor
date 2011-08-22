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

#include <sigfwd/sigfwd.hpp>
#include <boost/bind.hpp>
using boost::bind;

KeySignatureDialog::KeySignatureDialog(const KeySignature& key) :
    ui(new Ui::KeySignatureDialog),
    newKey(key)
{
    ui->setupUi(this);

    // Initialize based on the original value of the key signature, and create any signal connections
    if (newKey.IsMajorKey())
    {
        ui->majorKeyButton->setChecked(true);
    }
    else
    {
        ui->minorKeyButton->setChecked(true);
    }

    sigfwd::connect(ui->majorKeyButton, SIGNAL(clicked()),
                    bind(&KeySignatureDialog::populateKeyTypes, this, KeySignature::majorKey));
    sigfwd::connect(ui->minorKeyButton, SIGNAL(clicked()),
                    bind(&KeySignatureDialog::populateKeyTypes, this, KeySignature::minorKey));

    populateKeyTypes(newKey.GetKeyType());

    ui->keysComboBox->setCurrentIndex(newKey.GetKeyAccidentals());

    ui->visibilityCheckBox->setChecked(newKey.IsShown());
}

KeySignatureDialog::~KeySignatureDialog()
{
    delete ui;
}

/// Populate the list of key types, depending on whether we are using major or minor keys
void KeySignatureDialog::populateKeyTypes(uint8_t type)
{
    // store the original selected index, so we can reset it after repopulating the list
    int originalSelection = ui->keysComboBox->currentIndex();
    if (originalSelection == -1)
    {
        originalSelection = 0;
    }

    ui->keysComboBox->clear();

    KeySignature tempKey(type, 0);

    for (int i = KeySignature::noAccidentals; i <= KeySignature::sevenFlats; i++)
    {
        tempKey.SetKeyAccidentals(i);
        ui->keysComboBox->addItem(QString::fromStdString(tempKey.GetText()));
    }

    ui->keysComboBox->setCurrentIndex(originalSelection);
}

/// Return the new key, as selected by the user
KeySignature KeySignatureDialog::getNewKey() const
{
    return newKey;
}

void KeySignatureDialog::accept()
{
    // update the new key with the values selected in the dialog
    newKey.SetShown(ui->visibilityCheckBox->isChecked());

    if (ui->majorKeyButton->isChecked())
    {
        newKey.SetKeyType(KeySignature::majorKey);
    }
    else if (ui->minorKeyButton->isChecked())
    {
        newKey.SetKeyType(KeySignature::minorKey);
    }

    newKey.SetKeyAccidentals(ui->keysComboBox->currentIndex());

    done(Accepted);
}
