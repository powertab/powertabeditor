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
#include <boost/shared_ptr.hpp>
#include <boost/cstdint.hpp>

class Guitar;
class QHBoxLayout;
class QLabel;
class QCheckBox;
class QSlider;
class QDial;
class QComboBox;
class QLineEdit;
class ClickableLabel;
class TuningDictionary;
class Score;

class MixerInstrument : public QWidget
{
    Q_OBJECT
public:
    MixerInstrument(Score* score, boost::shared_ptr<Guitar> instrument,
                    boost::shared_ptr<TuningDictionary> tuningDictionary,
                    QWidget *parent);

    boost::shared_ptr<Guitar> getInstrument() const
    {
        return guitar;
    }
    
    void update();

signals:
    void visibilityToggled(uint32_t trackNumber, bool isVisible);

private:
    QHBoxLayout* layout;

    QLabel* instrumentIndex;
    ClickableLabel* instrumentName;
    QLineEdit* instrumentNameEditor;

    QCheckBox* isVisible;

    QSlider* trackVolume;
    QDial* trackPan;

    QComboBox* trackPatch;

    ClickableLabel* tuningLabel;

    Score* score;
    boost::shared_ptr<Guitar> guitar;
    boost::shared_ptr<TuningDictionary> tuningDictionary;

private slots:
    void changePan(int value);
    void changeVolume(int value);
    void changePatch(int value);
    void changeInstrumentName(QString name);
    void editTuning();
    void toggleVisible(bool visible);
};

#endif // MIXERINSTRUMENT_H
