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

#include <QVBoxLayout>
#include <QDialogButtonBox>
#include <QFormLayout>
#include <QRadioButton>
#include <QCheckBox>
#include <QComboBox>

#include <sigfwd/sigfwd.hpp>
#include <boost/bind.hpp>
using boost::bind;

KeySignatureDialog::KeySignatureDialog(const KeySignature& key) :
    newKey(key)
{
    setModal(true);
    setWindowTitle(tr("Key Signature"));

    QVBoxLayout* mainLayout = new QVBoxLayout;

    QFormLayout* formLayout = new QFormLayout;

    // visibility selection
    visibilityToggle = new QCheckBox;
    formLayout->addRow(tr("Visible:"), visibilityToggle);

    // key type selection
    majorKey = new QRadioButton(tr("Major"));
    minorKey = new QRadioButton(tr("Minor"));

    QHBoxLayout* keyTypeLayout = new QHBoxLayout;
    keyTypeLayout->addWidget(majorKey);
    keyTypeLayout->addWidget(minorKey);

    formLayout->addRow(tr("Key Type:"), keyTypeLayout);

    // key accidentals selection
    keyList = new QComboBox;
    formLayout->addRow(tr("Key:"), keyList);

    mainLayout->addLayout(formLayout);

    QDialogButtonBox* buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
    connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
    mainLayout->addWidget(buttonBox);

    setLayout(mainLayout);

    init();
}

/// Initialize based on the original value of the key signature, and create any signal connections
void KeySignatureDialog::init()
{
    if (newKey.IsMajorKey())
    {
        majorKey->setChecked(true);
    }
    else
    {
        minorKey->setChecked(true);
    }

    sigfwd::connect(majorKey, SIGNAL(clicked()),
                    bind(&KeySignatureDialog::populateKeyTypes, this, KeySignature::majorKey));
    sigfwd::connect(minorKey, SIGNAL(clicked()),
                    bind(&KeySignatureDialog::populateKeyTypes, this, KeySignature::minorKey));

    populateKeyTypes(newKey.GetKeyType());

    keyList->setCurrentIndex(newKey.GetKeyAccidentals());

    visibilityToggle->setChecked(newKey.IsShown());
}

/// Populate the list of key types, depending on whether we are using major or minor keys
void KeySignatureDialog::populateKeyTypes(uint8_t type)
{
    // store the original selected index, so we can reset it after repopulating the list
    int originalSelection = keyList->currentIndex();
    if (originalSelection == -1)
    {
        originalSelection = 0;
    }

    keyList->clear();

    KeySignature tempKey(type, 0);

    for (int i = KeySignature::noAccidentals; i <= KeySignature::sevenFlats; i++)
    {
        tempKey.SetKeyAccidentals(i);
        keyList->addItem(QString::fromStdString(tempKey.GetText()));
    }

    keyList->setCurrentIndex(originalSelection);
}

/// Return the new key, as selected by the user
KeySignature KeySignatureDialog::getNewKey() const
{
    return newKey;
}

void KeySignatureDialog::accept()
{
    // update the new key with the values selected in the dialog
    newKey.SetShown(visibilityToggle->isChecked());

    if (majorKey->isChecked())
    {
        newKey.SetKeyType(KeySignature::majorKey);
    }
    else if (minorKey->isChecked())
    {
        newKey.SetKeyType(KeySignature::minorKey);
    }

    newKey.SetKeyAccidentals(keyList->currentIndex());

    done(Accepted);
}
