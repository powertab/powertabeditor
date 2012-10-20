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
  
#include "notepage.h"

#include <QPushButton>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QAction>

#include <app/skinmanager.h>
#include <app/powertabeditor.h>
#include <app/command.h>

#include <widgets/common.h>

void NotePage::createNoteButtons()
{
    noteGroup = new QGroupBox(tr("Notes"));
    noteLayout = new QHBoxLayout;

    for (int i=0; i<7; i++)
    {
        QString icon, tip;
        Command* action = NULL;

        switch(i)
        {
        case 0:
            icon=":/images/whole_note";
            tip="Whole Note";
            action = mainWindow->wholeNoteAct;
            break;
        case 1:
            icon=":/images/half_note";
            tip="Half Note";
            action = mainWindow->halfNoteAct;
            break;
        case 2:
            icon=":/images/quarter_note";
            tip="Quarter Note";
            action = mainWindow->quarterNoteAct;
            break;
        case 3:
            icon=":/images/8th_note";
            tip="8th Note";
            action = mainWindow->eighthNoteAct;
            break;
        case 4:
            icon=":/images/16th_note";
            tip="16th Note";
            action = mainWindow->sixteenthNoteAct;
            break;
        case 5:
            icon=":/images/32nd_note";
            tip="32nd Note";
            action = mainWindow->thirtySecondNoteAct;
            break;
        case 6:
            icon=":/images/64th_note";
            tip="64th Note";
            action = mainWindow->sixtyFourthNoteAct;
            break;
        }

        noteButton[i] = new QPushButton(QIcon(icon),"");
        initButton(noteButton[i]);
        noteButton[i]->setToolTip(tip);

        connectButtonToAction(noteButton[i], action);

        noteLayout->addWidget(noteButton[i]);
    }

    noteLayout->addStretch(1);

    noteGroup->setLayout(noteLayout);
    layout->addWidget(noteGroup);
}

void NotePage::createRestButtons()
{
    restGroup = new QGroupBox(tr("Rests"));
    restLayout = new QHBoxLayout;

    for (int i=0; i<7; i++)
    {
        QString icon, tip;

        switch(i)
        {
        case 0:	icon=":/images/whole_rest";		tip="Whole Rest"; break;
        case 1: icon=":/images/half_rest";		tip="Half Rest"; break;
        case 2: icon=":/images/quarter_rest";	tip="Quarter Rest"; break;
        case 3: icon=":/images/8th_rest";		tip="8th Rest"; break;
        case 4: icon=":/images/16th_rest";		tip="16th Rest"; break;
        case 5: icon=":/images/32nd_rest";		tip="32nd Rest"; break;
        case 6: icon=":/images/64th_rest";		tip="64th Rest"; break;
        }

        restButton[i] = new QPushButton(QIcon(icon),"");
        initButton(restButton[i]);
        restButton[i]->setToolTip(tip);

        connect(restButton[i],SIGNAL(pressed()),this,SLOT(resetNoteAndRestButtons()));

        restLayout->addWidget(restButton[i]);
    }

    restLayout->addStretch(1);

    restGroup->setLayout(restLayout);
    layout->addWidget(restGroup);
}

void NotePage::createRhythmButtons()
{
    rhythmGroup = new QGroupBox(tr("Rhythmic Devices"));
    rhythmLayout = new QHBoxLayout;

    dottedButton = new QPushButton(QIcon(":images/dotted_note"),"");
    dottedButton->setToolTip(tr("Dotted Note"));
    initButton(dottedButton);
    connectButtonToAction(dottedButton, mainWindow->dottedNoteAct);

    doubleDottedButton = new QPushButton(QIcon(":images/doubledotted_note"),"");
    initButton(doubleDottedButton);
    doubleDottedButton->setToolTip(tr("Double Dotted Note"));
    connectButtonToAction(doubleDottedButton, mainWindow->doubleDottedNoteAct);

    tieButton = new QPushButton(QIcon(":images/tie_note"),"");
    initButton(tieButton);
    tieButton->setToolTip(tr("Tie Note"));
    connectButtonToAction(tieButton, mainWindow->tiedNoteAct);

    groupingButton = new QPushButton(QIcon(":images/group_note"),"");
    initButton(groupingButton);
    groupingButton->setToolTip(tr("Group Note"));
    connectButtonToAction(groupingButton, mainWindow->irregularGroupingAct);

    fermataButton = new QPushButton(QIcon(":images/fermata"),"");
    initButton(fermataButton);
    fermataButton->setToolTip(tr("Fermata"));
    connectButtonToAction(fermataButton, mainWindow->fermataAct);

    connect(dottedButton,SIGNAL(pressed()),this,SLOT(resetDottedButtons()));
    connect(doubleDottedButton,SIGNAL(pressed()),this,SLOT(resetDottedButtons()));

    rhythmLayout->addWidget(dottedButton);
    rhythmLayout->addWidget(doubleDottedButton);
    rhythmLayout->addWidget(tieButton);
    rhythmLayout->addWidget(groupingButton);
    rhythmLayout->addWidget(fermataButton);

    rhythmLayout->addStretch(1);

    rhythmGroup->setLayout(rhythmLayout);
    layout->addWidget(rhythmGroup);
}

void NotePage::createSlideLegatoButtons()
{
    slideLegatoGroup = new QGroupBox(tr("Slide && Legato"));
    slideLegatoLayout = new QHBoxLayout;

    slideInBelowButton = new QPushButton(QIcon(":images/slideinbelow.gif"),"");
    slideInBelowButton->setToolTip(tr("Slide In From Below"));
    initButton(slideInBelowButton);
    connectButtonToAction(slideInBelowButton, mainWindow->slideIntoFromBelowAct);

    slideInAboveButton = new QPushButton(QIcon(":images/slideinabove.gif"),"");
    slideInAboveButton->setToolTip(tr("Slide In From Above"));
    initButton(slideInAboveButton);
    connectButtonToAction(slideInAboveButton, mainWindow->slideIntoFromAboveAct);

    slideOutBelowButton = new QPushButton(QIcon(":images/slideoutdown.gif"),"");
    slideOutBelowButton->setToolTip(tr("Slide Out Downwards"));
    initButton(slideOutBelowButton);
    connectButtonToAction(slideOutBelowButton, mainWindow->slideOutOfDownwardsAct);

    slideOutAboveButton = new QPushButton(QIcon(":images/slideoutup.gif"),"");
    slideOutAboveButton->setToolTip(tr("Slide Out Upwards"));
    initButton(slideOutAboveButton);
    connectButtonToAction(slideOutAboveButton, mainWindow->slideOutOfUpwardsAct);

    shiftSlideButton = new QPushButton(QIcon(":images/shiftslide.gif"),"");
    shiftSlideButton->setToolTip(tr("Shift Slide"));
    initButton(shiftSlideButton);
    connectButtonToAction(shiftSlideButton, mainWindow->shiftSlideAct);

    legatoSlideButton = new QPushButton(QIcon(":images/legatoslide.gif"),"");
    legatoSlideButton->setToolTip(tr("Legato Slide"));
    initButton(legatoSlideButton);
    connectButtonToAction(legatoSlideButton, mainWindow->legatoSlideAct);

    legatoButton = new QPushButton(QIcon(":images/legato.gif"),"");
    legatoButton->setToolTip(tr("Legato"));
    initButton(legatoButton);
    connectButtonToAction(legatoButton, mainWindow->hammerPullAct);

    slideLegatoLayout->addWidget(legatoButton);
    slideLegatoLayout->addWidget(legatoSlideButton);
    slideLegatoLayout->addWidget(shiftSlideButton);
    slideLegatoLayout->addWidget(slideInBelowButton);
    slideLegatoLayout->addWidget(slideInAboveButton);
    slideLegatoLayout->addWidget(slideOutBelowButton);
    slideLegatoLayout->addWidget(slideOutAboveButton);

    slideLegatoLayout->addStretch(1);

    slideLegatoGroup->setLayout(slideLegatoLayout);
    layout->addWidget(slideLegatoGroup);
}

void NotePage::createVibratoTremoloButtons()
{
    vibratoTremoloGroup = new QGroupBox(tr("Vibrato && Tremolo"));
    vibratoTremoloLayout = new QHBoxLayout;

    vibratoButton = new QPushButton(QIcon(":images/vibrato.png"),"");
    vibratoButton->setToolTip(tr("Vibrato"));
    initButton(vibratoButton);
    connectButtonToAction(vibratoButton, mainWindow->vibratoAct);

    wideVibratoButton = new QPushButton(QIcon(":images/widevibrato.png"),"");
    wideVibratoButton->setToolTip(tr("Wide Vibrato"));
    initButton(wideVibratoButton);
    connectButtonToAction(wideVibratoButton, mainWindow->wideVibratoAct);

    bendButton = new QPushButton(QIcon(":images/bend.gif"),"");
    bendButton->setToolTip(tr("Bend"));
    initButton(bendButton);

    trillButton = new QPushButton(QIcon(":images/trill.png"),"");
    trillButton->setToolTip(tr("Trill"));
    initButton(trillButton);
    connectButtonToAction(trillButton, mainWindow->trillAction);

    tremoloButton = new QPushButton(QIcon(":images/tremolo.png"),"");
    tremoloButton->setToolTip(tr("Tremolo"));
    initButton(tremoloButton);
    connectButtonToAction(tremoloButton, mainWindow->tremoloPickingAct);

    vibratoTremoloLayout->addWidget(vibratoButton);
    vibratoTremoloLayout->addWidget(wideVibratoButton);
    vibratoTremoloLayout->addWidget(bendButton);
    vibratoTremoloLayout->addWidget(trillButton);
    vibratoTremoloLayout->addWidget(tremoloButton);

    vibratoTremoloLayout->addStretch(1);

    vibratoTremoloGroup->setLayout(vibratoTremoloLayout);
    layout->addWidget(vibratoTremoloGroup);
}

void NotePage::resetNoteAndRestButtons()
{
    for (int i=0; i<7; i++)
        noteButton[i]->setChecked(false);

    for (int i=0; i<7; i++)
        restButton[i]->setChecked(false);
}

void NotePage::resetDottedButtons()
{
    dottedButton->setChecked(false);
    doubleDottedButton->setChecked(false);
}

NotePage::NotePage(PowerTabEditor* mainWindow, boost::shared_ptr<SkinManager> skinManager, QFrame *parent) :
    QFrame(parent),
    mainWindow(mainWindow)
{
    setFrameStyle(QFrame::StyledPanel);

    setStyleSheet(skinManager->getToolboxPageStyle());

    layout = new QVBoxLayout;

    createNoteButtons();
    createRestButtons();
    createRhythmButtons();
    createSlideLegatoButtons();
    createVibratoTremoloButtons();

    layout->addStretch(1);

    setLayout(layout);
}

/// Common initialization code for all buttons in the widget
void NotePage::initButton(QPushButton *button)
{
    button->setIconSize(QSize(24,24));
    button->setCheckable(true);
    button->setFlat(true);
}
