#include "mixer.h"

#include <QVBoxLayout>

#include <skinmanager.h>
#include "mixerinstrument.h"

using std::shared_ptr;

Mixer::Mixer(shared_ptr<SkinManager> skinManager, QFrame *parent) :
    QFrame(parent)
{
    setFrameStyle(QFrame::StyledPanel);

    setStyleSheet(skinManager->getMixerStyle());

    layout = new QVBoxLayout;
    layout->setSpacing(0);
    setLayout(layout);
}

void Mixer::AddInstrument(shared_ptr<Guitar> guitar)
{
    MixerInstrument* channel = new MixerInstrument(guitar);
    layout->addWidget(channel);

    channelList.push_back(channel);
}

shared_ptr<Guitar> Mixer::getInstrument(size_t index)
{
    return channelList.at(index)->getInstrument();
}
