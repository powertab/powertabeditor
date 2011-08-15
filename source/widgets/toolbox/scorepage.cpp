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
  
#include "scorepage.h"

#include <QPushButton>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QVBoxLayout>

#include <app/skinmanager.h>
#include <widgets/common.h>
#include <app/powertabeditor.h>
#include <app/command.h>

void ScorePage::createSongButtons()
{
    songGroup = new QGroupBox(tr("Song"));
    songLayout = new QHBoxLayout;

    addInstrumentButton = new QPushButton(QIcon(":images/addinstrument.gif"),"");
    addInstrumentButton->setToolTip(tr("Add Instrument"));
    addInstrumentButton->setFlat(true);

    addPercussionButton = new QPushButton(QIcon(":images/addpercussiontrack.gif"),"");
    addPercussionButton->setToolTip(tr("Add Percussion Track"));
    addPercussionButton->setFlat(true);

    increaseHeightButton = new QPushButton(QIcon(":images/increasetablineheight.gif"),"");
    increaseHeightButton->setToolTip(tr("Increase Tablature Line Height"));
    increaseHeightButton->setFlat(true);

    decreaseHeightButton = new QPushButton(QIcon(":images/decreasetablineheight.gif"),"");
    decreaseHeightButton->setToolTip(tr("Decrease Tablature Line Height"));
    decreaseHeightButton->setFlat(true);

    songLayout->addWidget(addInstrumentButton);
    songLayout->addWidget(addPercussionButton);
    songLayout->addWidget(increaseHeightButton);
    songLayout->addWidget(decreaseHeightButton);

    songLayout->addStretch(1);

    songGroup->setLayout(songLayout);
    layout->addWidget(songGroup);
}

void ScorePage::createSectionButtons()
{
    sectionGroup = new QGroupBox(tr("Section"));
    sectionLayout = new QHBoxLayout;

    insertSectionBeforeButton = new QPushButton(QIcon(":images/dotted_note"),"");
    insertSectionBeforeButton->setToolTip(tr("Insert Section Before"));
    insertSectionBeforeButton->setFlat(true);

    insertSectionAfterButton = new QPushButton(QIcon(":images/dotted_note"),"");
    insertSectionAfterButton->setToolTip(tr("Insert Section After"));
    insertSectionAfterButton->setFlat(true);

    removeSectionButton = new QPushButton(QIcon(":images/dotted_note"),"");
    removeSectionButton->setToolTip(tr("Remove Section"));
    removeSectionButton->setFlat(true);

    addBarButton = new QPushButton(QIcon(":images/dotted_note"),"");
    addBarButton->setToolTip(tr("Add Bar"));
    addBarButton->setFlat(true);

    increaseWidthButton = new QPushButton(QIcon(":images/dotted_note"),"");
    increaseWidthButton->setToolTip(tr("Increase Width"));
    increaseWidthButton->setFlat(true);
    connectButtonToAction(increaseWidthButton, mainWindow->increasePositionSpacingAct);

    decreaseWidthButton = new QPushButton(QIcon(":images/dotted_note"),"");
    decreaseWidthButton->setToolTip(tr("Decrease Width"));
    decreaseWidthButton->setFlat(true);
    connectButtonToAction(decreaseWidthButton, mainWindow->decreasePositionSpacingAct);

    justifyButton = new QPushButton(QIcon(":images/dotted_note"),"");
    justifyButton->setToolTip(tr("Justify"));
    justifyButton->setFlat(true);

    sectionLayout->addWidget(insertSectionBeforeButton);
    sectionLayout->addWidget(insertSectionAfterButton);
    sectionLayout->addWidget(removeSectionButton);
    sectionLayout->addWidget(addBarButton);
    sectionLayout->addWidget(increaseWidthButton);
    sectionLayout->addWidget(decreaseWidthButton);
    sectionLayout->addWidget(justifyButton);

    sectionLayout->addStretch(1);

    sectionGroup->setLayout(sectionLayout);
    layout->addWidget(sectionGroup);
}

void ScorePage::createFlowDynamicsButtons()
{
    flowDynamicsGroup = new QGroupBox(tr("Flow && Dynamics"));
    flowDynamicsLayout = new QHBoxLayout;

    addRehearsalSignButton = new QPushButton(QIcon(":images/dotted_note"),"");
    addRehearsalSignButton->setToolTip(tr("Add Rehearsal Sign"));
    addRehearsalSignButton->setFlat(true);

    addDirectionButton = new QPushButton(QIcon(":images/dotted_note"),"");
    addDirectionButton->setToolTip(tr("Add Musical Direction"));
    addDirectionButton->setFlat(true);

    addTempoMarkerButton = new QPushButton(QIcon(":images/dotted_note"),"");
    addTempoMarkerButton->setToolTip(tr("Add Tempo Marker"));
    addTempoMarkerButton->setFlat(true);

    addVolumeMarkerButton = new QPushButton(QIcon(":images/dotted_note"),"");
    addVolumeMarkerButton->setToolTip(tr("Add Volume Marker"));
    addVolumeMarkerButton->setFlat(true);

    flowDynamicsLayout->addWidget(addRehearsalSignButton);
    flowDynamicsLayout->addWidget(addDirectionButton);
    flowDynamicsLayout->addWidget(addTempoMarkerButton);
    flowDynamicsLayout->addWidget(addVolumeMarkerButton);

    flowDynamicsLayout->addStretch(1);

    flowDynamicsGroup->setLayout(flowDynamicsLayout);
    layout->addWidget(flowDynamicsGroup);
}

ScorePage::ScorePage(PowerTabEditor* mainWindow, boost::shared_ptr<SkinManager> skinManager, QFrame *parent) :
    QFrame(parent),
    mainWindow(mainWindow)
{
    setFrameStyle(QFrame::StyledPanel);

    setStyleSheet(skinManager->getToolboxPageStyle());

    layout = new QVBoxLayout;

    createSongButtons();
    createSectionButtons();
    //createStaffButtons();
    createFlowDynamicsButtons();

    layout->addStretch(1);

    setLayout(layout);
}
