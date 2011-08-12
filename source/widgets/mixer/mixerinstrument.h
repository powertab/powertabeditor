/*
  * Copyright (C) 2011 Cameron White
  *
  * This program is free software: you can redistribute it and/or modify
  * it under the terms of the GNU General Public License as published by
  * the Free Software Foundation, either version 3 of the License, or
  * (at your option) any later version.
  *
  * This program is distributed in the hope that it will be useful,
  * but WITHOUT ANY WARRANTY; without even the implied warranty of
  * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  * GNU General Public License for more details.
  *
  * You should have received a copy of the GNU General Public License
  * along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
  
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
