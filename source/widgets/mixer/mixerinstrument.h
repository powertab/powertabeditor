#ifndef MIXERINSTRUMENT_H
#define MIXERINSTRUMENT_H

#include <QWidget>
#include <memory>

class Guitar;
class QHBoxLayout;
class QLabel;
class QRadioButton;
class QSlider;
class QDial;
class QComboBox;
class QLineEdit;

#include <widgets/common.h>

class MixerInstrument : public QWidget
{
    Q_OBJECT
public:
    MixerInstrument(std::shared_ptr<Guitar> instrument, QWidget *parent = 0);

    std::shared_ptr<Guitar> getInstrument() { return guitar; }

protected:
    QHBoxLayout* layout;

    QLabel* instrumentIndex;
    ClickableLabel* instrumentName;
    QLineEdit* instrumentNameEditor;

    QRadioButton* soloPlayback;
    QRadioButton* mutePlayback;

    QSlider* trackVolume;
    QDial* trackPan;

    QComboBox* trackPatch;

    std::shared_ptr<Guitar> guitar;

public slots:
    void changePan(int value);
    void changeVolume(int value);
    void changePatch(int value);
    void changeInstrumentName(QString name);
};

#endif // MIXERINSTRUMENT_H
