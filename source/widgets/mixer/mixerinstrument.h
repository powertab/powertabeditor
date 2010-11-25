#ifndef MIXERINSTRUMENT_H
#define MIXERINSTRUMENT_H

#include <QWidget>

class Guitar;
class QHBoxLayout;
class QLabel;
class QRadioButton;
class QSlider;
class QDial;

class MixerInstrument : public QWidget
{
    Q_OBJECT
public:
    MixerInstrument(Guitar* guitar, QWidget *parent = 0);

protected:
    QHBoxLayout* layout;

    QLabel* instrumentIndex;
    QLabel* instrumentName;

    QRadioButton* soloPlayback;
    QRadioButton* mutePlayback;

    QSlider* trackVolume;
    QDial* trackPan;



signals:

public slots:

};

#endif // MIXERINSTRUMENT_H
