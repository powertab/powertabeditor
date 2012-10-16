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
  
#include "mixerinstrument.h"

#include <QHBoxLayout>
#include <QLabel>
#include <QCheckBox>
#include <QSlider>
#include <QDial>
#include <QComboBox>
#include <QLineEdit>

#include <widgets/clickablelabel.h>

#include <dialogs/tuningdialog.h>

#include <powertabdocument/guitar.h>
#include <powertabdocument/generalmidi.h>

#include <app/powertabeditor.h>
#include <actions/edittuning.h>

using boost::shared_ptr;

MixerInstrument::MixerInstrument(shared_ptr<Guitar> instrument,
                                 shared_ptr<TuningDictionary> tuningDictionary,
                                 QWidget *parent) :
    QWidget(parent),
    guitar(instrument),
    tuningDictionary(tuningDictionary)
{
    layout = new QHBoxLayout;

    instrumentIndex = new QLabel(QString("%1.").arg(guitar->GetNumber() + 1));
    instrumentIndex->setMinimumWidth(25);
    layout->addWidget(instrumentIndex);

    instrumentName = new ClickableLabel;
    instrumentName->setMinimumWidth(150);
    instrumentName->setMaximumWidth(150);
    instrumentName->setToolTip(tr("Click to change instrument name."));
    layout->addWidget(instrumentName);

    instrumentNameEditor = new QLineEdit;
    instrumentNameEditor->setMinimumWidth(150);
    instrumentNameEditor->setMaximumWidth(150);
    instrumentNameEditor->hide();
    layout->addWidget(instrumentNameEditor);

    // when the name is clicked, switch to the text editor
    connect(instrumentName, SIGNAL(clicked()), instrumentName, SLOT(hide()));
    connect(instrumentName, SIGNAL(clicked()), instrumentNameEditor, SLOT(show()));
    connect(instrumentName, SIGNAL(clicked()), instrumentNameEditor, SLOT(setFocus()));

    // after editing is done, update and switch back to the label
    connect(instrumentNameEditor, SIGNAL(textEdited(QString)), this, SLOT(changeInstrumentName(QString)));
    connect(instrumentNameEditor, SIGNAL(editingFinished()), instrumentName, SLOT(show()));
    connect(instrumentNameEditor, SIGNAL(editingFinished()), instrumentNameEditor, SLOT(hide()));

    isVisible = new QCheckBox(tr("Show/Hide"));
    isVisible->setChecked(guitar->IsShown());
    isVisible->setToolTip(tr("Click to toggle whether the instrument is displayed in the score."));
    connect(isVisible, SIGNAL(toggled(bool)), this, SLOT(toggleVisible(bool)));
    layout->addWidget(isVisible);

    trackVolume = new QSlider(Qt::Horizontal);
    trackVolume->setMaximumWidth(75);
    trackVolume->setRange(Guitar::MIN_INITIAL_VOLUME, Guitar::MAX_INITIAL_VOLUME);
    trackVolume->setTickInterval(Guitar::MAX_INITIAL_VOLUME / 8 );
    trackVolume->setToolTip(tr("Drag to adjust volume."));
    connect(trackVolume, SIGNAL(valueChanged(int)), this, SLOT(changeVolume(int)));
    layout->addWidget(trackVolume);

    trackPan = new QDial;
    trackPan->setMaximumSize(25, 25);
    trackPan->setRange(Guitar::MIN_PAN, Guitar::MAX_PAN);
    trackPan->setSingleStep(1);
    trackPan->setPageStep(Guitar::MAX_PAN / 8);
    trackPan->setToolTip(tr("Drag to adjust panning."));
    connect(trackPan, SIGNAL(valueChanged(int)), this, SLOT(changePan(int)));
    layout->addWidget(trackPan);

    trackPatch = new QComboBox;
    trackPatch->setMaximumWidth(200);
    trackPatch->addItem("Acoustic Grand Piano");
    trackPatch->addItem("Bright Acoustic Piano");
    trackPatch->addItem("Electric Grand Piano");
    trackPatch->addItem("Honky-tonk Piano");
    trackPatch->addItem("Electric Piano 1");
    trackPatch->addItem("Electric Piano 2");
    trackPatch->addItem("Harpsichord");
    trackPatch->addItem("Clavinet");
    trackPatch->addItem("Celesta");
    trackPatch->addItem("Glockenspiel");
    trackPatch->addItem("Music Box");
    trackPatch->addItem("Vibraphone");
    trackPatch->addItem("Marimba");
    trackPatch->addItem("Xylophone");
    trackPatch->addItem("Tubular Bells");
    trackPatch->addItem("Dulcimer");
    trackPatch->addItem("Drawbar Organ");
    trackPatch->addItem("Percussive Organ");
    trackPatch->addItem("Rock Organ");
    trackPatch->addItem("Church Organ");
    trackPatch->addItem("Reed Organ");
    trackPatch->addItem("Accordion");
    trackPatch->addItem("Harmonica");
    trackPatch->addItem("Tango Accordion");
    trackPatch->addItem("Acoustic Guitar (nylon)");
    trackPatch->addItem("Acoustic Guitar (steel)");
    trackPatch->addItem("Electric Guitar (jazz)");
    trackPatch->addItem("Electric Guitar (clean)");
    trackPatch->addItem("Electric Guitar (muted)");
    trackPatch->addItem("Overdriven Guitar");
    trackPatch->addItem("Distortion Guitar");
    trackPatch->addItem("Guitar harmonics");
    trackPatch->addItem("Acoustic Bass");
    trackPatch->addItem("Electric Bass (finger)");
    trackPatch->addItem("Electric Bass (pick)");
    trackPatch->addItem("Fretless Bass");
    trackPatch->addItem("Slap Bass 1");
    trackPatch->addItem("Slap Bass 2");
    trackPatch->addItem("Synth Bass 1");
    trackPatch->addItem("Synth Bass 2");
    trackPatch->addItem("Violin");
    trackPatch->addItem("Viola");
    trackPatch->addItem("Cello");
    trackPatch->addItem("Contrabass");
    trackPatch->addItem("Tremolo Strings");
    trackPatch->addItem("Pizzicato Strings");
    trackPatch->addItem("Orchestral Harp");
    trackPatch->addItem("Timpani");
    trackPatch->addItem("String Ensemble 1");
    trackPatch->addItem("String Ensemble 2");
    trackPatch->addItem("Synth Strings 1");
    trackPatch->addItem("Synth Strings 2");
    trackPatch->addItem("Choir Aahs");
    trackPatch->addItem("Voice Oohs");
    trackPatch->addItem("Synth Voice");
    trackPatch->addItem("Orchestra Hit");
    trackPatch->addItem("Trumpet");
    trackPatch->addItem("Trombone");
    trackPatch->addItem("Tuba");
    trackPatch->addItem("Muted Trumpet");
    trackPatch->addItem("French Horn");
    trackPatch->addItem("Brass Section");
    trackPatch->addItem("Synth Brass 1");
    trackPatch->addItem("Synth Brass 2");
    trackPatch->addItem("Soprano Sax");
    trackPatch->addItem("Alto Sax");
    trackPatch->addItem("Tenor Sax");
    trackPatch->addItem("Baritone Sax");
    trackPatch->addItem("Oboe");
    trackPatch->addItem("English Horn");
    trackPatch->addItem("Bassoon");
    trackPatch->addItem("Clarinet");
    trackPatch->addItem("Piccolo");
    trackPatch->addItem("Flute");
    trackPatch->addItem("Recorder");
    trackPatch->addItem("Pan Flute");
    trackPatch->addItem("Blown Bottle");
    trackPatch->addItem("Shakuhachi");
    trackPatch->addItem("Whistle");
    trackPatch->addItem("Ocarina");
    trackPatch->addItem("Lead 1 (square)");
    trackPatch->addItem("Lead 2 (sawtooth)");
    trackPatch->addItem("Lead 3 (calliope)");
    trackPatch->addItem("Lead 4 (chiff)");
    trackPatch->addItem("Lead 5 (charang)");
    trackPatch->addItem("Lead 6 (voice)");
    trackPatch->addItem("Lead 7 (fifths)");
    trackPatch->addItem("Lead 8 (bass + lead)");
    trackPatch->addItem("Pad 1 (new age)");
    trackPatch->addItem("Pad 2 (warm)");
    trackPatch->addItem("Pad 3 (polysynth)");
    trackPatch->addItem("Pad 4 (choir)");
    trackPatch->addItem("Pad 5 (bowed)");
    trackPatch->addItem("Pad 6 (metallic)");
    trackPatch->addItem("Pad 7 (halo)");
    trackPatch->addItem("Pad 8 (sweep)");
    trackPatch->addItem("FX 1 (rain)");
    trackPatch->addItem("FX 2 (soundtrack)");
    trackPatch->addItem("FX 3 (crystal)");
    trackPatch->addItem("FX 4 (atmosphere)");
    trackPatch->addItem("FX 5 (brightness)");
    trackPatch->addItem("FX 6 (goblins)");
    trackPatch->addItem("FX 7 (echoes)");
    trackPatch->addItem("FX 8 (sci-fi)");
    trackPatch->addItem("Sitar");
    trackPatch->addItem("Banjo");
    trackPatch->addItem("Shamisen");
    trackPatch->addItem("Koto");
    trackPatch->addItem("Kalimba");
    trackPatch->addItem("Bag pipe");
    trackPatch->addItem("Fiddle");
    trackPatch->addItem("Shanai");
    trackPatch->addItem("Tinkle Bell");
    trackPatch->addItem("Agogo");
    trackPatch->addItem("Steel Drums");
    trackPatch->addItem("Woodblock");
    trackPatch->addItem("Taiko Drum");
    trackPatch->addItem("Melodic Tom");
    trackPatch->addItem("Synth Drum");
    trackPatch->addItem("Reverse Cymbal");
    trackPatch->addItem("Guitar Fret Noise");
    trackPatch->addItem("Breath Noise");
    trackPatch->addItem("Seashore");
    trackPatch->addItem("Bird Tweet");
    trackPatch->addItem("Telephone Ring");
    trackPatch->addItem("Helicopter");
    trackPatch->addItem("Applause");
    trackPatch->addItem("Gunshot");
    connect(trackPatch, SIGNAL(activated(int)), this, SLOT(changePatch(int)));
    layout->addWidget(trackPatch);

    tuningLabel = new ClickableLabel;
    tuningLabel->setToolTip(tr("Click to adjust tuning."));
    layout->addWidget(tuningLabel);
    connect(tuningLabel, SIGNAL(clicked()), this, SLOT(editTuning()));
    
    update(); // initialize the widgets

    layout->setSizeConstraint(QLayout::SetFixedSize);

    setLayout(layout);
}

void MixerInstrument::changePan(int value)
{
    guitar->SetPan(value);
}

void MixerInstrument::changeVolume(int value)
{
    guitar->SetInitialVolume(value);
}

void MixerInstrument::changePatch(int value)
{
    guitar->SetPreset(value);
}

void MixerInstrument::changeInstrumentName(QString name)
{
    guitar->SetDescription(name.toStdString());
    instrumentName->setText(name);
}

void MixerInstrument::editTuning()
{
    TuningDialog dialog(this, guitar->GetTuning(), tuningDictionary);

    if (dialog.exec() == QDialog::Accepted)
    {
        PowerTabEditor::undoManager->push(new EditTuning(guitar, dialog.getNewTuning()));
    }
}

/// Update the widget's data
void MixerInstrument::update()
{
    instrumentName->setText(QString().fromStdString(guitar->GetDescription()));
    instrumentNameEditor->setText(instrumentName->text());
    trackVolume->setValue(guitar->GetInitialVolume());
    trackPan->setValue(guitar->GetPan());
    trackPatch->setCurrentIndex(guitar->GetPreset());
    tuningLabel->setText(QString().fromStdString(guitar->GetTuningSpelling()));
}

void MixerInstrument::toggleVisible(bool visible)
{
    emit visibilityToggled(guitar->GetNumber(), visible);
}
