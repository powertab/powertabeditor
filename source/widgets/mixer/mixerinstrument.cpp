#include "mixerinstrument.h"

#include <QHBoxLayout>
#include <QLabel>
#include <QRadioButton>
#include <QSlider>
#include <QDial>

#include "../../powertabdocument/guitar.h"
#include "../../powertabdocument/generalmidi.h"

MixerInstrument::MixerInstrument(Guitar* instrument, QWidget *parent) :
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

    layout->addStretch();

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

