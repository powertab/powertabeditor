#include "mixerinstrument.h"

#include <QHBoxLayout>
#include <QLabel>
#include <QRadioButton>
#include <QSlider>
#include <QDial>

#include "../../powertabdocument/guitar.h"

MixerInstrument::MixerInstrument(Guitar* guitar, QWidget *parent) :
    QWidget(parent)
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
    layout->addWidget(trackVolume);

    trackPan = new QDial;
    trackPan->setMaximumSize(25, 25);
    layout->addWidget(trackPan);

    layout->addStretch();

    setLayout(layout);
}
