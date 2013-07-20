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
#include <QMessageBox>

#include <boost/foreach.hpp>

#include <actions/edittuning.h>
#include <app/powertabeditor.h>
#include <dialogs/tuningdialog.h>
#include <powertabdocument/generalmidi.h>
#include <powertabdocument/guitar.h>
#include <widgets/clickablelabel.h>

using boost::shared_ptr;

MixerInstrument::MixerInstrument(Score* score, shared_ptr<Guitar> instrument,
                                 shared_ptr<TuningDictionary> tuningDictionary,
                                 QWidget *parent) :
    QWidget(parent),
    score(score),
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
    connect(isVisible, SIGNAL(clicked(bool)), this, SLOT(toggleVisible(bool)));
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
    std::vector<std::string> presetNames;
    midi::GetMidiPresetNames(presetNames);
    BOOST_FOREACH(const std::string& name, presetNames)
    {
        trackPatch->addItem(QString::fromStdString(name));
    }
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
    TuningDialog dialog(this, guitar, guitar->GetTuning(), tuningDictionary);

    if (dialog.exec() == QDialog::Accepted)
    {
        Tuning newTuning = dialog.getNewTuning();
        if (EditTuning::canChangeTuning(score, guitar, newTuning))
        {
            PowerTabEditor::undoManager->push(new EditTuning(this, score,
                                                             guitar, newTuning,
                                                             dialog.getNewCapo()),
                                              UndoManager::AFFECTS_ALL_SYSTEMS);
        }
        else
        {
            // TODO - offer an option to e.g. automatically delete or shift
            // notes, etc.
            QMessageBox msg;
            msg.setText(QObject::tr("Cannot safely change the tuning of this guitar."));
            msg.exec();
            return;
        }
    }
}

/// Update the widget's data
void MixerInstrument::update()
{
    instrumentName->setText(QString().fromStdString(guitar->GetDescription()));
    instrumentNameEditor->setText(instrumentName->text());
    isVisible->setChecked(guitar->IsShown());
    trackVolume->setValue(guitar->GetInitialVolume());
    trackPan->setValue(guitar->GetPan());
    trackPatch->setCurrentIndex(guitar->GetPreset());
    tuningLabel->setText(QString().fromStdString(guitar->GetTuningSpelling()));
}

void MixerInstrument::toggleVisible(bool visible)
{
    emit visibilityToggled(guitar->GetNumber(), visible);
}
