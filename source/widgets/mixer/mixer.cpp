#include "mixer.h"

#include <QVBoxLayout>
#include <QLabel> // temp

#include "mixerinstrument.h"

Mixer::Mixer(QWidget *parent) :
    QWidget(parent)
{
    layout = new QVBoxLayout;
    layout->setSpacing(0);
    setLayout(layout);
}

void Mixer::AddInstrument(Guitar *guitar)
{
    MixerInstrument* channel = new MixerInstrument(guitar);
    layout->addWidget(channel);

    channelList.push_back(channel);
}
