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

    void AddInstrument(Guitar* guitar);
    Guitar* getInstrument(int index);

protected:
    QList<MixerInstrument *> channelList;
    QVBoxLayout* layout;

signals:

public slots:

};

#endif // MIXER_H
