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
class ClickableLabel;

class MixerInstrument : public QWidget
{
    Q_OBJECT
public:
    MixerInstrument(std::shared_ptr<Guitar> instrument, QWidget *parent = 0);

    std::shared_ptr<Guitar> getInstrument() const
    {
        return guitar;
    }
    
    void update();

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

    ClickableLabel* tuningLabel;

    std::shared_ptr<Guitar> guitar;

public slots:
    void changePan(int value);
    void changeVolume(int value);
    void changePatch(int value);
    void changeInstrumentName(QString name);
    void editTuning();
};

#endif // MIXERINSTRUMENT_H
