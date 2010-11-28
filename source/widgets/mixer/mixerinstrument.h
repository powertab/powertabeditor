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
    MixerInstrument(Guitar* instrument, QWidget *parent = 0);
    Guitar* getInstrument() { return guitar; }

protected:
    QHBoxLayout* layout;

    QLabel* instrumentIndex;
    QLabel* instrumentName;

    QRadioButton* soloPlayback;
    QRadioButton* mutePlayback;

    QSlider* trackVolume;
    QDial* trackPan;

    Guitar* guitar;

signals:

public slots:

    void changePan(int value);
    void changeVolume(int value);

};

#endif // MIXERINSTRUMENT_H
