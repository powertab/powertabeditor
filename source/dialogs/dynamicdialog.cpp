#include "dynamicdialog.h"
#include "ui_dynamicdialog.h"

#include <powertabdocument/dynamic.h>

DynamicDialog::DynamicDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DynamicDialog),
    volumeLevels(new QButtonGroup(this)),
    volumeLevel(Dynamic::f)
{
    ui->setupUi(this);

    // add each button to a QButtonGroup (to ensure that only one is ever checked), and
    // associate each with the corresponding value from the Dynamic class
    volumeLevels->addButton(ui->volumeOffButton, Dynamic::off);
    volumeLevels->addButton(ui->pppButton, Dynamic::ppp);
    volumeLevels->addButton(ui->ppButton, Dynamic::pp);
    volumeLevels->addButton(ui->pButton, Dynamic::p);
    volumeLevels->addButton(ui->mpButton, Dynamic::mp);
    volumeLevels->addButton(ui->mfButton, Dynamic::mf);
    volumeLevels->addButton(ui->fButton, Dynamic::f);
    volumeLevels->addButton(ui->ffButton, Dynamic::ff);
    volumeLevels->addButton(ui->fffButton, Dynamic::fff);

    // initialize
    volumeLevels->button(volumeLevel)->setChecked(true);
    ui->buttonBox->setFocus();

    connect(volumeLevels, SIGNAL(buttonClicked(int)),
            this, SLOT(updateSelectedVolumeLevel(int)));
}

DynamicDialog::~DynamicDialog()
{
    delete ui;
}

/// Returns the volume level that was selected by the user
uint8_t DynamicDialog::selectedVolumeLevel() const
{
    return volumeLevel;
}

void DynamicDialog::updateSelectedVolumeLevel(int level)
{
    volumeLevel = level;
}
