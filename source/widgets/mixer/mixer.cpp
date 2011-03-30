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
    layout->setSizeConstraint(QLayout::SetFixedSize);
    setLayout(layout);
}

void Mixer::addInstrument(shared_ptr<Guitar> guitar)
{
    std::shared_ptr<MixerInstrument> channel(new MixerInstrument(guitar));
    layout->addWidget(channel.get());

    channelList.push_back(channel);
}

void Mixer::removeInstrument(size_t index)
{
    Q_ASSERT(index < channelList.size());

    layout->removeWidget(channelList.at(index).get());
    channelList.erase(channelList.begin() + index);
}

shared_ptr<Guitar> Mixer::getInstrument(size_t index) const
{
    return channelList.at(index)->getInstrument();
}
