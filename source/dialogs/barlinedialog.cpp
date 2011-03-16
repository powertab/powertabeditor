#include "barlinedialog.h"
#include <powertabeditor.h>
#include <actions/changebarlinetype.h>
#include <powertabdocument/barline.h>

#include <QDialogButtonBox>
#include <QVBoxLayout>
#include <QFormLayout>
#include <QSpinBox>
#include <QComboBox>

BarlineDialog::BarlineDialog(Barline* bar, QWidget *parent) :
        QDialog(parent)
{
    barLine = bar;
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
    repeatCount->setMinimum(barLine->MIN_REPEAT_COUNT);
    repeatCount->setMaximum(barLine->MAX_REPEAT_COUNT);

    // get repeat count
    repeatCount->setValue(barLine->GetRepeatCount());

    // set bar line type choice
    barLineType->setCurrentIndex(barLine->GetType());
}

void BarlineDialog::disableRepeatCount(int newBarlineType)
{
    if (newBarlineType == barLine->repeatEnd || newBarlineType == barLine->repeatStart)
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
    if (barLine->GetType() == barLineType->currentIndex() &&
        (int)barLine->GetRepeatCount() == repeatCount->value())
    {
        done(QDialog::Rejected);
    }
    else
    {
        PowerTabEditor::undoManager->push(new ChangeBarLineType(barLine, barLineType->currentIndex(), repeatCount->value()));
        done(QDialog::Accepted);
    }
}

void BarlineDialog::reject()
{
    done(QDialog::Rejected);
}
