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
#include <QCheckBox>

MIDITab::MIDITab(QWidget *parent) :
    QWidget(parent)
{
    rootLayout = new QVBoxLayout(this);

    formLayout = new QFormLayout;

    QLabel *tab_description = new QLabel();
    tab_description->setText(tr("These settings allow customization of the MIDI output."));
    rootLayout->addWidget(tab_description);

    rootLayout->addStretch(1);

    midiPort = new QComboBox();

    RtMidiWrapper rtMidiWrapper;
    for(quint32 i = 0;i < rtMidiWrapper.getPortCount(); i++)
    {
        midiPort->addItem(QString().fromStdString(rtMidiWrapper.getPortName(i)));
    }

    formLayout->addRow(tr("MIDI Output Device:"), midiPort);

    metronomeEnabled = new QCheckBox;
    formLayout->addRow(tr("Metronome Enabled:"), metronomeEnabled);

    vibratoStrength = new QSpinBox;
    vibratoStrength->setRange(1, 127);
    formLayout->addRow(tr("Vibrato Strength:"), vibratoStrength);

    wideVibratoStrength = new QSpinBox;
    wideVibratoStrength->setRange(1, 127);
    formLayout->addRow(tr("Wide Vibrato Strength:"), wideVibratoStrength);

    rootLayout->addLayout(formLayout);

    bottomLine = new QHBoxLayout();

    bottomLine->setSpacing(100);

    rootLayout->addLayout(bottomLine,1);

    // initialize the widget values
    QSettings settings;
    midiPort->setCurrentIndex(settings.value("midi/preferredPort").toInt());
    metronomeEnabled->setChecked(settings.value("midi/metronomeEnabled").toBool());
    vibratoStrength->setValue(settings.value("midi/vibrato", 85).toUInt());
    wideVibratoStrength->setValue(settings.value("midi/wide_vibrato", 127).toUInt());
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

void PreferencesDialog::accept()
{
    QSettings settings;
    // save the preferred midi port
    settings.setValue("midi/preferredPort", midiTab->midiPort->currentIndex());
    // save the metronome settings
    settings.setValue("midi/metronomeEnabled", midiTab->metronomeEnabled->isChecked());

    settings.setValue("midi/vibrato", midiTab->vibratoStrength->value());
    settings.setValue("midi/wide_vibrato", midiTab->wideVibratoStrength->value());
    settings.sync();

    done(Accepted);
}

void PreferencesDialog::reject()
{
    done(Rejected);
}
