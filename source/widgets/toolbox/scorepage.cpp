#include "scorepage.h"

#include <QPushButton>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QVBoxLayout>

#include <skinmanager.h>

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

    insertSectionBeforeButton = new QPushButton(QIcon(":images/dotted_note.gif"),"");
    insertSectionBeforeButton->setToolTip(tr("Insert Section Before"));
    insertSectionBeforeButton->setFlat(true);

    insertSectionAfterButton = new QPushButton(QIcon(":images/dotted_note.gif"),"");
    insertSectionAfterButton->setToolTip(tr("Insert Section After"));
    insertSectionAfterButton->setFlat(true);

    removeSectionButton = new QPushButton(QIcon(":images/dotted_note.gif"),"");
    removeSectionButton->setToolTip(tr("Remove Section"));
    removeSectionButton->setFlat(true);

    addBarButton = new QPushButton(QIcon(":images/dotted_note.gif"),"");
    addBarButton->setToolTip(tr("Add Bar"));
    addBarButton->setFlat(true);

    increaseWidthButton = new QPushButton(QIcon(":images/dotted_note.gif"),"");
    increaseWidthButton->setToolTip(tr("Increase Width"));
    increaseWidthButton->setFlat(true);

    decreaseWidthButton = new QPushButton(QIcon(":images/dotted_note.gif"),"");
    decreaseWidthButton->setToolTip(tr("Decrease Width"));
    decreaseWidthButton->setFlat(true);

    justifyButton = new QPushButton(QIcon(":images/dotted_note.gif"),"");
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

    addRehearsalSignButton = new QPushButton(QIcon(":images/dotted_note.gif"),"");
    addRehearsalSignButton->setToolTip(tr("Add Rehearsal Sign"));
    addRehearsalSignButton->setFlat(true);

    addDirectionButton = new QPushButton(QIcon(":images/dotted_note.gif"),"");
    addDirectionButton->setToolTip(tr("Add Musical Direction"));
    addDirectionButton->setFlat(true);

    addTempoMarkerButton = new QPushButton(QIcon(":images/dotted_note.gif"),"");
    addTempoMarkerButton->setToolTip(tr("Add Tempo Marker"));
    addTempoMarkerButton->setFlat(true);

    addVolumeMarkerButton = new QPushButton(QIcon(":images/dotted_note.gif"),"");
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

ScorePage::ScorePage(std::shared_ptr<SkinManager> skinManager, QFrame *parent) : QFrame(parent)
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
