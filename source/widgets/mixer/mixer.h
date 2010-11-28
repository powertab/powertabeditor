#ifndef MIXER_H
#define MIXER_H

#include <QWidget>

class MixerInstrument;
class Guitar;
class QVBoxLayout;

class Mixer : public QWidget
{
    Q_OBJECT
public:
    Mixer(QWidget *parent = 0);

    void AddInstrument(Guitar* guitar);
    Guitar* getInstrument(int index);

protected:
    QList<MixerInstrument *> channelList;
    QVBoxLayout* layout;

signals:

public slots:

};

#endif // MIXER_H
