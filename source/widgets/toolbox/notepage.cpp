#include "notepage.h"

void NotePage::createNoteButtons()
{
	noteGroup = new QGroupBox(tr("Notes"));
	noteLayout = new QHBoxLayout;

	for (int i=0; i<7; i++)
	{
		QString icon, tip;

		switch(i)
		{
			case 0:	icon=":/images/whole_note.gif";		tip="Whole Note"; break;
			case 1: icon=":/images/half_note.gif";		tip="Half Note"; break;
			case 2: icon=":/images/quarter_note.gif";	tip="Quarter Note"; break;
			case 3: icon=":/images/8th_note.gif";		tip="8th Note"; break;
			case 4: icon=":/images/16th_note.gif";		tip="16th Note"; break;
			case 5: icon=":/images/32th_note.gif";		tip="32nd Note"; break;
			case 6: icon=":/images/64th_note.gif";		tip="64th Note"; break;
		}

		noteButton[i] = new QPushButton(QIcon(icon),"");
		noteButton[i]->setToolTip(tip);
		noteButton[i]->setCheckable(true);

		connect(noteButton[i],SIGNAL(pressed()),this,SLOT(resetNoteAndRestButtons()));

		noteLayout->addWidget(noteButton[i]);
	}

	noteButton[0]->setChecked(true);

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
			case 0:	icon=":/images/whole_rest.gif";		tip="Whole Rest"; break;
			case 1: icon=":/images/half_rest.gif";		tip="Half Rest"; break;
			case 2: icon=":/images/quarter_rest.gif";	tip="Quarter Rest"; break;
			case 3: icon=":/images/8th_rest.gif";		tip="8th Rest"; break;
			case 4: icon=":/images/16th_rest.gif";		tip="16th Rest"; break;
			case 5: icon=":/images/32nd_rest.gif";		tip="32nd Rest"; break;
			case 6: icon=":/images/64th_rest.gif";		tip="64th Rest"; break;
		}

		restButton[i] = new QPushButton(QIcon(icon),"");
		restButton[i]->setToolTip(tip);
		restButton[i]->setCheckable(true);

		connect(restButton[i],SIGNAL(pressed()),this,SLOT(resetNoteAndRestButtons()));

		restLayout->addWidget(restButton[i]);
	}

	restGroup->setLayout(restLayout);
	layout->addWidget(restGroup);
}

void NotePage::createOtherButtons()
{
	rhythmGroup = new QGroupBox(tr("Rhythmic Devices"));
	rhythmLayout = new QHBoxLayout;

	dottedButton = new QPushButton(QIcon(":images/dotted_note.gif"),"");
	dottedButton->setToolTip(tr("Dotted Note"));
	dottedButton->setCheckable(true);

	doubledottedButton = new QPushButton(QIcon(":images/doubledotted_note.gif"),"");
	doubledottedButton->setToolTip(tr("Double Dotted Note"));
	doubledottedButton->setCheckable(true);

	tieButton = new QPushButton(QIcon(":images/tie_note.gif"),"");
	tieButton->setToolTip(tr("Tie Note"));
	tieButton->setCheckable(true);

	connect(dottedButton,SIGNAL(pressed()),this,SLOT(resetDottedButtons()));
	connect(doubledottedButton,SIGNAL(pressed()),this,SLOT(resetDottedButtons()));

	rhythmLayout->addWidget(dottedButton);
	rhythmLayout->addWidget(doubledottedButton);
	rhythmLayout->addWidget(tieButton);

	rhythmGroup->setLayout(rhythmLayout);
	layout->addWidget(rhythmGroup);
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
	doubledottedButton->setChecked(false);
}

NotePage::NotePage(QWidget *parent) : QWidget(parent)
{
	layout = new QVBoxLayout;

	createNoteButtons();
	createRestButtons();
	createOtherButtons();

	layout->addStretch(1);

	setLayout(layout);
}
