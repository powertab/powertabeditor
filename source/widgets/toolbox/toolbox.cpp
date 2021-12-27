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

#include "toolbox.h"
#include "ui_toolbox.h"

#include <app/command.h>

#include <QAction>
#include <QToolButton>

ToolBox::ToolBox(Command *keySignatureCommand,
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
                 QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ToolBox)
{
    ui->setupUi(this);

    ui->keySignatureButton->setDefaultAction(keySignatureCommand);
    ui->timeSignatureButton->setDefaultAction(timeSignatureCommand);
    ui->standardBarlineButton->setDefaultAction(standardBarlineCommand);
    ui->repeatEndingButton->setDefaultAction(repeatEndingCommand);
    ui->octave8vaButton->setDefaultAction(octave8vaCommand);
    ui->octave15maButton->setDefaultAction(octave15maCommand);
    ui->octave8vbButton->setDefaultAction(octave8vbCommand);
    ui->octave15mbButton->setDefaultAction(octave15mbCommand);

    ui->wholeNoteButton->setDefaultAction(wholeNoteCommand);
    ui->halfNoteButton->setDefaultAction(halfNoteCommand);
    ui->quarterNoteButton->setDefaultAction(quarterNoteCommand);
    ui->eighthNoteButton->setDefaultAction(eighthNoteCommand);
    ui->sixteenthNoteButton->setDefaultAction(sixteenthNoteCommand);
    ui->thirtySecondNoteButton->setDefaultAction(thirtySecondNoteCommand);
    ui->sixtyFourthNoteButton->setDefaultAction(sixtyFourthNoteCommand);
    ui->addRestButton->setDefaultAction(addRestCommand);

    ui->dottedButton->setDefaultAction(dottedCommand);
    ui->doubleDottedButton->setDefaultAction(doubleDottedCommand);
    ui->tripletButton->setDefaultAction(tripletCommand);
    ui->irregularGroupingButton->setDefaultAction(irregularGroupingCommand);
    ui->tieButton->setDefaultAction(tieCommand);
    ui->fermataButton->setDefaultAction(fermataCommand);
    ui->tempoMarkerButton->setDefaultAction(tempoMarkerCommand);
    ui->multibarRestButton->setDefaultAction(multibarRestCommand);

    ui->dynamicPPPButton->setDefaultAction(dynamicPPPCommand);
    ui->dynamicPPButton->setDefaultAction(dynamicPPCommand);
    ui->dynamicPButton->setDefaultAction(dynamicPCommand);
    ui->dynamicMPButton->setDefaultAction(dynamicMPCommand);
    ui->dynamicMFButton->setDefaultAction(dynamicMFCommand);
    ui->dynamicFButton->setDefaultAction(dynamicFCommand);
    ui->dynamicFFButton->setDefaultAction(dynamicFFCommand);
    ui->dynamicFFFButton->setDefaultAction(dynamicFFFCommand);

    ui->mutedButton->setDefaultAction(mutedCommand);
    ui->ghostNoteButton->setDefaultAction(ghostNoteCommand);
    ui->marcatoButton->setDefaultAction(marcatoCommand);
    ui->sforzandoButton->setDefaultAction(sforzandoCommand);
    ui->staccatoButton->setDefaultAction(staccatoCommand);
    ui->letRingButton->setDefaultAction(letRingCommand);
    ui->palmMuteButton->setDefaultAction(palmMuteCommand);
    ui->tapButton->setDefaultAction(tapCommand);

    ui->naturalHarmonicButton->setDefaultAction(naturalHarmonicCommand);
    ui->artificialHarmonicButton->setDefaultAction(artificialHarmonicCommand);
    ui->tappedHarmonicButton->setDefaultAction(tappedHarmonicCommand);
    ui->bendButton->setDefaultAction(bendCommand);
    ui->tremoloBarButton->setDefaultAction(tremoloBarCommand);
    ui->vibratoButton->setDefaultAction(vibratoCommand);
    ui->wideVibratoButton->setDefaultAction(wideVibratoCommand);
    ui->graceNoteButton->setDefaultAction(graceNoteCommand);
    ui->leftHandFingeringButton->setDefaultAction(leftHandFingeringCommand);

    ui->hammerPullButton->setDefaultAction(hammerPullCommand);
    ui->trillButton->setDefaultAction(trillCommand);
    ui->legatoSlideButton->setDefaultAction(legatoSlideCommand);
    ui->shiftSlideButton->setDefaultAction(shiftSlideCommand);
    ui->slideIntoFromAboveButton->setDefaultAction(slideIntoFromAboveCommand);
    ui->slideIntoFromBelowButton->setDefaultAction(slideIntoFromBelowCommand);
    ui->slideOutOfDownwardsButton->setDefaultAction(slideOutOfDownwardsCommand);
    ui->slideOutOfUpwardsButton->setDefaultAction(slideOutOfUpwardsCommand);

    ui->pickStrokeUpButton->setDefaultAction(pickStrokeUpCommand);
    ui->pickStrokeDownButton->setDefaultAction(pickStrokeDownCommand);
    ui->arpeggioUpButton->setDefaultAction(arpeggioUpCommand);
    ui->arpeggioDownButton->setDefaultAction(arpeggioDownCommand);
    ui->textButton->setDefaultAction(textCommand);
    ui->directionButton->setDefaultAction(directionCommand);
}

ToolBox::~ToolBox()
{
    delete ui;
}
