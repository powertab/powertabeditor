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
		noteButton[i]->setFlat(true);

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
		restButton[i]->setFlat(true);

		connect(restButton[i],SIGNAL(pressed()),this,SLOT(resetNoteAndRestButtons()));

		restLayout->addWidget(restButton[i]);
	}

	restGroup->setLayout(restLayout);
	layout->addWidget(restGroup);
}

void NotePage::createRhythmButtons()
{
	rhythmGroup = new QGroupBox(tr("Rhythmic Devices"));
	rhythmLayout = new QHBoxLayout;

	dottedButton = new QPushButton(QIcon(":images/dotted_note.gif"),"");
	dottedButton->setToolTip(tr("Dotted Note"));
	dottedButton->setCheckable(true);
	dottedButton->setFlat(true);

	doubleDottedButton = new QPushButton(QIcon(":images/doubledotted_note.gif"),"");
	doubleDottedButton->setToolTip(tr("Double Dotted Note"));
	doubleDottedButton->setCheckable(true);
	doubleDottedButton->setFlat(true);

	tieButton = new QPushButton(QIcon(":images/tie_note.gif"),"");
	tieButton->setToolTip(tr("Tie Note"));
	tieButton->setCheckable(true);
	tieButton->setFlat(true);

	groupingButton = new QPushButton(QIcon(":images/grouping.gif"),"");
	groupingButton->setToolTip(tr("Group Note"));
	groupingButton->setCheckable(true);
	groupingButton->setFlat(true);

	fermataButton = new QPushButton(QIcon(":images/fermata.gif"),"");
	fermataButton->setToolTip(tr("Fermata"));
	fermataButton->setCheckable(true);
	fermataButton->setFlat(true);

	connect(dottedButton,SIGNAL(pressed()),this,SLOT(resetDottedButtons()));
	connect(doubleDottedButton,SIGNAL(pressed()),this,SLOT(resetDottedButtons()));

	rhythmLayout->addWidget(dottedButton);
	rhythmLayout->addWidget(doubleDottedButton);
	rhythmLayout->addWidget(tieButton);
	rhythmLayout->addWidget(groupingButton);
	rhythmLayout->addWidget(fermataButton);

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

	slideOutAboveButton = new QPushButton(QIcon(":images/slideoutup.gif"),"");
	slideOutAboveButton->setToolTip(tr("Slide Out Upwards"));
	slideOutAboveButton->setCheckable(true);
	slideOutAboveButton->setFlat(true);

	shiftSlideButton = new QPushButton(QIcon(":images/shiftslide.gif"),"");
	shiftSlideButton->setToolTip(tr("Shift Slide"));
	shiftSlideButton->setCheckable(true);
	shiftSlideButton->setFlat(true);

	legatoSlideButton = new QPushButton(QIcon(":images/legatoslide.gif"),"");
	legatoSlideButton->setToolTip(tr("Legato Slide"));
	legatoSlideButton->setCheckable(true);
	legatoSlideButton->setFlat(true);

	legatoButton = new QPushButton(QIcon(":images/legato.gif"),"");
	legatoButton->setToolTip(tr("Legato"));
	legatoButton->setCheckable(true);
	legatoButton->setFlat(true);

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

	wideVibratoButton = new QPushButton(QIcon(":images/widevibrato.gif"),"");
	wideVibratoButton->setToolTip(tr("Wide Vibrato"));
	wideVibratoButton->setCheckable(true);
	wideVibratoButton->setFlat(true);

	bendButton = new QPushButton(QIcon(":images/bend.gif"),"");
	bendButton->setToolTip(tr("Bend"));
	bendButton->setCheckable(true);
	bendButton->setFlat(true);

	trillButton = new QPushButton(QIcon(":images/trill.gif"),"");
	trillButton->setToolTip(tr("Trill"));
	trillButton->setCheckable(true);
	trillButton->setFlat(true);

	tremoloButton = new QPushButton(QIcon(":images/tremolo.gif"),"");
	tremoloButton->setToolTip(tr("Tremolo"));
	tremoloButton->setCheckable(true);
	tremoloButton->setFlat(true);

	/*
	connect(dottedButton,SIGNAL(pressed()),this,SLOT(resetDottedButtons()));
	connect(doubleDottedButton,SIGNAL(pressed()),this,SLOT(resetDottedButtons()));
	*/

	vibratoTremoloLayout->addWidget(slightVibratoButton);
	vibratoTremoloLayout->addWidget(wideVibratoButton);
	vibratoTremoloLayout->addWidget(bendButton);
	vibratoTremoloLayout->addWidget(trillButton);
	vibratoTremoloLayout->addWidget(tremoloButton);

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

NotePage::NotePage(QWidget *parent) : QWidget(parent)
{
	layout = new QVBoxLayout;

	createNoteButtons();
	createRestButtons();
	createRhythmButtons();
	createSlideLegatoButtons();
	createVibratoTremoloButtons();

	layout->addStretch(1);

	setLayout(layout);
}
