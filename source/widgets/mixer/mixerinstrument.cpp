#include "mixerinstrument.h"

#include <QHBoxLayout>
#include <QLabel>
#include <QRadioButton>
#include <QSlider>
#include <QDial>
#include <QComboBox>

#include <powertabdocument/guitar.h>
#include <powertabdocument/generalmidi.h>

using std::shared_ptr;

MixerInstrument::MixerInstrument(shared_ptr<Guitar> instrument, QWidget *parent) :
    QWidget(parent),
    guitar(instrument)
{
    layout = new QHBoxLayout;

    instrumentIndex = new QLabel(QString().number(guitar->GetNumber() + 1) + ".");
    layout->addWidget(instrumentIndex);

    instrumentName = new QLabel(QString().fromStdString(guitar->GetDescription()));
    instrumentName->setMinimumWidth(150);
    instrumentName->setMaximumWidth(150);
    layout->addWidget(instrumentName);

    soloPlayback = new QRadioButton;
    layout->addWidget(soloPlayback);

    mutePlayback = new QRadioButton;
    layout->addWidget(mutePlayback);

    trackVolume = new QSlider(Qt::Horizontal);
    trackVolume->setMaximumWidth(75);
    trackVolume->setRange(Guitar::MIN_INITIAL_VOLUME, Guitar::MAX_INITIAL_VOLUME);
    trackVolume->setTickInterval(Guitar::MAX_INITIAL_VOLUME / 8 );
    trackVolume->setValue(guitar->GetInitialVolume());
    connect(trackVolume, SIGNAL(valueChanged(int)), this, SLOT(changeVolume(int)));
    layout->addWidget(trackVolume);

    trackPan = new QDial;
    trackPan->setMaximumSize(25, 25);
    trackPan->setRange(Guitar::MIN_PAN, Guitar::MAX_PAN);
    trackPan->setSingleStep(1);
    trackPan->setPageStep(Guitar::MAX_PAN / 8);
    trackPan->setValue(guitar->GetPan());
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
	trackPatch->setCurrentIndex(guitar->GetPreset());
	connect(trackPatch, SIGNAL(activated(int)), this, SLOT(changePatch(int)));
	layout->addWidget(trackPatch);

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

