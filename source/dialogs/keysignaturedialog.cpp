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
    originalKey(key),
    newKey(originalKey)
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

/// Initialize based on the current value of the key signature, and create any signal connections
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
                    bind(&KeySignatureDialog::toggleKeyType, this, KeySignature::majorKey));
    sigfwd::connect(minorKey, SIGNAL(clicked()),
                    bind(&KeySignatureDialog::toggleKeyType, this, KeySignature::minorKey));

    populateKeyTypes();

    keyList->setCurrentIndex(newKey.GetKeyAccidentals());

    connect(keyList, SIGNAL(currentIndexChanged(int)), this, SLOT(setKeyAccidentals(int)));

    visibilityToggle->setChecked(newKey.IsShown());
    connect(visibilityToggle, SIGNAL(clicked(bool)), this, SLOT(toggleVisible(bool)));
}

/// Populate the list of key types, depending on whether we are using major or minor keys
void KeySignatureDialog::populateKeyTypes()
{
    // store the original selected index, so we can reset it after repopulating the list
    int originalSelection = keyList->currentIndex();
    if (originalSelection == -1)
    {
        originalSelection = 0;
    }

    keyList->clear();

    KeySignature tempKey(newKey.GetKeyType(), 0);

    for (int i = KeySignature::noAccidentals; i <= KeySignature::sevenFlats; i++)
    {
        tempKey.SetKeyAccidentals(i);
        keyList->addItem(QString::fromStdString(tempKey.GetText()));
    }

    keyList->setCurrentIndex(originalSelection);
}

/// Toggles the key type (major/minor) of the new key signature
void KeySignatureDialog::toggleKeyType(uint8_t type)
{
    newKey.SetKeyType(type);
    populateKeyTypes();
}

/// Toggles whether the new key signature will be visible
void KeySignatureDialog::toggleVisible(bool visible)
{
    newKey.SetShown(visible);
}

/// Updates the selected number of accidentals in the new key signature
void KeySignatureDialog::setKeyAccidentals(int accidentals)
{
    // if this function is called after keyList->clear(), adjust the accidentals value to be valid
    if (accidentals == -1)
    {
        accidentals = KeySignature::noAccidentals;
    }

    newKey.SetKeyAccidentals(accidentals);

    // by default, force the key to be visible if it differs from the original key
    if (!newKey.IsSameKey(originalKey))
    {
        visibilityToggle->click();
    }
}

/// Return the new key, as selected by the user
KeySignature KeySignatureDialog::getNewKey() const
{
    return newKey;
}
