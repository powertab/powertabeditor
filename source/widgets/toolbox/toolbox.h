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

#ifndef TOOLBOX_H
#define TOOLBOX_H

#include <QWidget>

class Command;
class QToolButton;

namespace Ui {
class ToolBox;
}

class ToolBox : public QWidget
{
    Q_OBJECT

public:
    explicit ToolBox(Command *keySignatureCommand,
                     Command *timeSignatureCommand,
                     Command *standardBarlineCommand,
                     Command *repeatEndingCommand,
                     Command *octave8vaCommand,
                     Command *octave15maCommand,
                     Command *octave8vbCommand,
                     Command *octave15mbCommand,
                     Command *wholeNoteCommand,
                     Command *halfNoteCommand,
                     Command *quarterNoteCommand,
                     Command *eighthNoteCommand,
                     Command *sixteenthNoteCommand,
                     Command *thirtySecondNoteCommand,
                     Command *sixtyFourthNoteCommand,
                     Command *addRestCommand,
                     Command *dottedCommand,
                     Command *doubleDottedCommand,
                     Command *tripletCommand,
                     Command *irregularGroupingCommand,
                     Command *tieCommand,
                     Command *fermataCommand,
                     Command *tempoMarkerCommand,
                     Command *multibarRestCommand,
                     Command *dynamicPPPCommand,
                     Command *dynamicPPCommand,
                     Command *dynamicPCommand,
                     Command *dynamicMPCommand,
                     Command *dynamicMFCommand,
                     Command *dynamicFCommand,
                     Command *dynamicFFCommand,
                     Command *dynamicFFFCommand,
                     Command *mutedCommand,
                     Command *ghostNoteCommand,
                     Command *marcatoCommand,
                     Command *sforzandoCommand,
                     Command *staccatoCommand,
                     Command *letRingCommand,
                     Command *palmMuteCommand,
                     Command *tapCommand,
                     Command *naturalHarmonicCommand,
                     Command *artificialHarmonicCommand,
                     Command *tappedHarmonicCommand,
                     Command *bendCommand,
                     Command *tremoloBarCommand,
                     Command *vibratoCommand,
                     Command *wideVibratoCommand,
                     Command *graceNoteCommand,
                     Command *leftHandFingeringCommand,
                     Command *hammerPullCommand,
                     Command *trillCommand,
                     Command *legatoSlideCommand,
                     Command *shiftSlideCommand,
                     Command *slideIntoFromAboveCommand,
                     Command *slideIntoFromBelowCommand,
                     Command *slideOutOfDownwardsCommand,
                     Command *slideOutOfUpwardsCommand,
                     Command *pickStrokeUpCommand,
                     Command *pickStrokeDownCommand,
                     Command *arpeggioUpCommand,
                     Command *arpeggioDownCommand,
                     Command *textCommand,
                     Command *directionCommand,
                     QWidget *parent = nullptr);
    ~ToolBox();

private:
    void updateButtonStatusFromAction(QToolButton *button, Command *command);
    void initButton(QToolButton *button, Command *command);

    Ui::ToolBox *ui;
};

#endif // TOOLBOX_H
