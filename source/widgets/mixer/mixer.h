#ifndef MIXER_H
#define MIXER_H

#include <QFrame>

#include "../../skinmanager.h"

class MixerInstrument;
class Guitar;
class QVBoxLayout;

class Mixer : public QFrame
{
    Q_OBJECT
public:
	Mixer(QFrame *parent = 0, SkinManager *skinManager = 0);

    void AddInstrument(Guitar* guitar);
    Guitar* getInstrument(int index);

protected:
    QList<MixerInstrument *> channelList;
    QVBoxLayout* layout;

signals:

public slots:

};

#endif // MIXER_H
