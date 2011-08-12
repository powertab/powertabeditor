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
  
#include "preferencesdialog.h"

#include <audio/rtmidiwrapper.h>

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

#include <app/settings.h>

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
    midiPort->setCurrentIndex(settings.value(Settings::MIDI_PREFERRED_PORT,
                                             Settings::MIDI_PREFFERED_PORT_DEFAULT).toInt());

    metronomeEnabled->setChecked(settings.value(Settings::MIDI_METRONOME_ENABLED,
                                                Settings::MIDI_METRONOME_ENABLED_DEFAULT).toBool());

    vibratoStrength->setValue(settings.value(Settings::MIDI_VIBRATO_LEVEL,
                                             Settings::MIDI_VIBRATO_LEVEL_DEFAULT).toUInt());

    wideVibratoStrength->setValue(settings.value(Settings::MIDI_WIDE_VIBRATO_LEVEL,
                                                 Settings::MIDI_WIDE_VIBRATO_LEVEL_DEFAULT).toUInt());
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
    // save the settings
    QSettings settings;
    settings.setValue(Settings::MIDI_PREFERRED_PORT, midiTab->midiPort->currentIndex());
    settings.setValue(Settings::MIDI_METRONOME_ENABLED, midiTab->metronomeEnabled->isChecked());

    settings.setValue(Settings::MIDI_VIBRATO_LEVEL, midiTab->vibratoStrength->value());
    settings.setValue(Settings::MIDI_WIDE_VIBRATO_LEVEL, midiTab->wideVibratoStrength->value());
    settings.sync();

    done(Accepted);
}
