#ifndef MIXER_H
#define MIXER_H

#include <QFrame>

#include <vector>
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

    void addInstrument(std::shared_ptr<Guitar> guitar);
    void removeInstrument(size_t index);

    std::shared_ptr<Guitar> getInstrument(size_t index) const;

protected:
    std::vector<std::shared_ptr<MixerInstrument> > channelList;
    QVBoxLayout* layout;

public slots:
    void update();

};

#endif // MIXER_H
