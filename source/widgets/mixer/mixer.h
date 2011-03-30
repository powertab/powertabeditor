#ifndef MIXER_H
#define MIXER_H

#include <QFrame>

#include <memory>

class SkinManager;
class MixerInstrument;
class Guitar;
class QVBoxLayout;

class Mixer : public QFrame
{
    Q_OBJECT
public:
    Mixer(std::shared_ptr<SkinManager> skinManager, QFrame *parent = 0);

    void AddInstrument(std::shared_ptr<Guitar> guitar);

    std::shared_ptr<Guitar> getInstrument(size_t index);

protected:
    std::vector<MixerInstrument*> channelList;
    QVBoxLayout* layout;

signals:

public slots:

};

#endif // MIXER_H
