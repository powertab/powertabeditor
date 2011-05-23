#include "notepage.h"

#include <QPushButton>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QAction>

#include <app/skinmanager.h>
#include <powertabeditor.h>

#include <widgets/common.h>

void NotePage::createNoteButtons()
{
    noteGroup = new QGroupBox(tr("Notes"));
    noteLayout = new QHBoxLayout;

    for (int i=0; i<7; i++)
    {
        QString icon, tip;
        QAction* action = NULL;

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
        noteButton[i]->setIcon(QIcon(icon));
        noteButton[i]->setIconSize(QSize(24,24));
        noteButton[i]->setToolTip(tip);
        noteButton[i]->setCheckable(true);
        noteButton[i]->setFlat(true);		

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
        restButton[i]->setIconSize(QSize(24,24));
        restButton[i]->setToolTip(tip);
        restButton[i]->setCheckable(true);
        restButton[i]->setFlat(true);

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
    dottedButton->setIconSize(QSize(24,24));
    dottedButton->setToolTip(tr("Dotted Note"));
    dottedButton->setCheckable(true);
    dottedButton->setFlat(true);
    connectButtonToAction(dottedButton, mainWindow->dottedNoteAct);

    doubleDottedButton = new QPushButton(QIcon(":images/doubledotted_note"),"");
    doubleDottedButton->setIconSize(QSize(24,24));
    doubleDottedButton->setToolTip(tr("Double Dotted Note"));
    doubleDottedButton->setCheckable(true);
    doubleDottedButton->setFlat(true);
    connectButtonToAction(doubleDottedButton, mainWindow->doubleDottedNoteAct);

    tieButton = new QPushButton(QIcon(":images/tie_note"),"");
    tieButton->setIconSize(QSize(24,24));
    tieButton->setToolTip(tr("Tie Note"));
    tieButton->setCheckable(true);
    tieButton->setFlat(true);
    connectButtonToAction(tieButton, mainWindow->tiedNoteAct);

    groupingButton = new QPushButton(QIcon(":images/group_note"),"");
    groupingButton->setIconSize(QSize(24,24));
    groupingButton->setToolTip(tr("Group Note"));
    groupingButton->setCheckable(true);
    groupingButton->setFlat(true);

    fermataButton = new QPushButton(QIcon(":images/fermata"),"");
    fermataButton->setIconSize(QSize(24,24));
    fermataButton->setToolTip(tr("Fermata"));
    fermataButton->setCheckable(true);
    fermataButton->setFlat(true);
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
    slideInBelowButton->setCheckable(true);
    slideInBelowButton->setFlat(true);

    slideInAboveButton = new QPushButton(QIcon(":images/slideinabove.gif"),"");
    slideInAboveButton->setToolTip(tr("Slide In From Above"));
    slideInAboveButton->setCheckable(true);
    slideInAboveButton->setFlat(true);

    slideOutBelowButton = new QPushButton(QIcon(":images/slideoutdown.gif"),"");
    slideOutBelowButton->setToolTip(tr("Slide Out Downwards"));
    slideOutBelowButton->setCheckable(true);
    slideOutBelowButton->setFlat(true);
    connectButtonToAction(slideOutBelowButton, mainWindow->slideOutOfDownwardsAct);

    slideOutAboveButton = new QPushButton(QIcon(":images/slideoutup.gif"),"");
    slideOutAboveButton->setToolTip(tr("Slide Out Upwards"));
    slideOutAboveButton->setCheckable(true);
    slideOutAboveButton->setFlat(true);
    connectButtonToAction(slideOutAboveButton, mainWindow->slideOutOfUpwardsAct);

    shiftSlideButton = new QPushButton(QIcon(":images/shiftslide.gif"),"");
    shiftSlideButton->setToolTip(tr("Shift Slide"));
    shiftSlideButton->setCheckable(true);
    shiftSlideButton->setFlat(true);
    connectButtonToAction(shiftSlideButton, mainWindow->shiftSlideAct);

    legatoSlideButton = new QPushButton(QIcon(":images/legatoslide.gif"),"");
    legatoSlideButton->setToolTip(tr("Legato Slide"));
    legatoSlideButton->setCheckable(true);
    legatoSlideButton->setFlat(true);
    connectButtonToAction(legatoSlideButton, mainWindow->legatoSlideAct);

    legatoButton = new QPushButton(QIcon(":images/legato.gif"),"");
    legatoButton->setToolTip(tr("Legato"));
    legatoButton->setCheckable(true);
    legatoButton->setFlat(true);
    connectButtonToAction(legatoButton, mainWindow->hammerPullAct);

    /*
    connect(legatoButton,SIGNAL(pressed()),this,SLOT(resetArticulationButtons()));
    connect(slideButton,SIGNAL(pressed()),this,SLOT(resetArticulationButtons()));
    connect(legatoSlideButton,SIGNAL(pressed()),this,SLOT(resetArticulationButtons()));
    */

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

    slightVibratoButton = new QPushButton(QIcon(":images/slightvibrato.gif"),"");
    slightVibratoButton->setToolTip(tr("Slight Vibrato"));
    slightVibratoButton->setCheckable(true);
    slightVibratoButton->setFlat(true);
    connectButtonToAction(slightVibratoButton, mainWindow->vibratoAct);

    wideVibratoButton = new QPushButton(QIcon(":images/widevibrato.gif"),"");
    wideVibratoButton->setToolTip(tr("Wide Vibrato"));
    wideVibratoButton->setCheckable(true);
    wideVibratoButton->setFlat(true);
    connectButtonToAction(wideVibratoButton, mainWindow->wideVibratoAct);

    bendButton = new QPushButton(QIcon(":images/bend.gif"),"");
    bendButton->setToolTip(tr("Bend"));
    bendButton->setCheckable(true);
    bendButton->setFlat(true);

    trillButton = new QPushButton(QIcon(":images/trill.gif"),"");
    trillButton->setToolTip(tr("Trill"));
    trillButton->setCheckable(true);
    trillButton->setFlat(true);
    connectButtonToAction(trillButton, mainWindow->trillAction);

    tremoloButton = new QPushButton(QIcon(":images/tremolo.gif"),"");
    tremoloButton->setToolTip(tr("Tremolo"));
    tremoloButton->setCheckable(true);
    tremoloButton->setFlat(true);
    connectButtonToAction(tremoloButton, mainWindow->tremoloPickingAct);

    /*
    connect(dottedButton,SIGNAL(pressed()),this,SLOT(resetDottedButtons()));
    connect(doubleDottedButton,SIGNAL(pressed()),this,SLOT(resetDottedButtons()));
    */

    vibratoTremoloLayout->addWidget(slightVibratoButton);
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

void NotePage::resetSlideLegatoButtons()
{
    legatoButton->setChecked(false);
    legatoSlideButton->setChecked(false);
    shiftSlideButton->setChecked(false);
}

NotePage::NotePage(PowerTabEditor* mainWindow, std::shared_ptr<SkinManager> skinManager, QFrame *parent) :
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
