#include "mixer.h"

#include <QVBoxLayout>

#include <skinmanager.h>
#include "mixerinstrument.h"

Mixer::Mixer(QFrame *parent, SkinManager *skinManager) :
    QFrame(parent)
{
    setFrameStyle(QFrame::StyledPanel);

    setStyleSheet(skinManager->getMixerStyle());

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

Guitar* Mixer::getInstrument(int index)
{
    return channelList.at(index)->getInstrument();
}
