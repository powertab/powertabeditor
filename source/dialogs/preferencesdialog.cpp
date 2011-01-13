#include "preferencesdialog.h"
#include <rtmidiwrapper.h>

#include <QDialogButtonBox>
#include <QFormLayout>
#include <QSpinBox>
#include <QSettings>
#include <QTabWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QComboBox>

MIDITab::MIDITab(QWidget *parent) :
    QWidget(parent)
{
    root_layout = new QVBoxLayout(this);

    QLabel *tab_description = new QLabel();
    tab_description->setText(tr("These settings allow customization of the MIDI output."));
    root_layout->addWidget(tab_description);

    root_layout->addStretch(1);

    label = new QLabel();
    label->setText(tr("MIDI Input Device:"));

    root_layout->addWidget(label);

    box = new QComboBox();


    RtMidiWrapper rtMidiWrapper;
    for(int i=0;i<rtMidiWrapper.getPortCount();i++)
    {
        box->addItem(QString(rtMidiWrapper.getPortName(i).c_str()));
    }

    root_layout->addWidget(box);

    bottom_line = new QHBoxLayout();

    bottom_line->setSpacing(100);

    root_layout->addLayout(bottom_line,1);
}

PreferencesDialog::PreferencesDialog(QWidget *parent) :
        QDialog(parent)
{
    setWindowTitle(tr("Editor Preferences"));
    setMinimumSize(500,400);
    setModal(true);

    QDialogButtonBox* buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);

    midiTab = new MIDITab();

    tabWidget = new QTabWidget();
    tabWidget->setTabPosition(QTabWidget::West);
    //tabWidget->setIconSize(QSize(32,32));
    //tabWidget->addTab(midiTab,tr("&General"));
    tabWidget->addTab(midiTab,tr("&MIDI"));
    //tabWidget->addTab(midiTab,tr("&Style"));

    QVBoxLayout *buttonsLayout = new QVBoxLayout;
    buttonsLayout->addWidget(tabWidget);
    buttonsLayout->addStretch(1);
    buttonsLayout->addWidget(buttonBox);

    buttonsLayout->setSizeConstraint(QLayout::SetFixedSize);
    setLayout(buttonsLayout);

    connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
    connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
}

/*
void PreferencesDialog::disableRepeatCount(int newBarlineType)
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
*/

void PreferencesDialog::accept()
{
    QSettings settings;
    // save the preferred midi port
    settings.setValue("midi/preferredPort", midiTab->box->currentIndex());
    settings.sync();

    done(1);
}

void PreferencesDialog::reject()
{
    done(1);
}
