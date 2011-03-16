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

void BarlineDialog::reject()
{
    done(QDialog::Rejected);
}
