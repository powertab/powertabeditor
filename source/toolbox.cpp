#include "toolbox.h"

#include <QHBoxLayout>
#include <QLabel>

Toolbox::Toolbox(QWidget *parent) :
	QTabWidget(parent)
{
	setMaximumWidth(350);
	setIconSize(QSize(48,48));
	setTabPosition(QTabWidget::West);

	addTab(new NoteTab, QIcon(":/icons/toolbox_note.png"),"");
	addTab(new OrnamentTab, QIcon(":/icons/toolbox_embellishment.png"),"");
	addTab(new ScoreTab, QIcon(":/icons/toolbox_score.png"),"");
}

void NoteTab::ResetNoteAndRestButtons()
{
	note1_button->setChecked(false);
	note2_button->setChecked(false);
	note4_button->setChecked(false);
	note8_button->setChecked(false);
	note16_button->setChecked(false);
	note32_button->setChecked(false);

	rest1_button->setChecked(false);
	rest2_button->setChecked(false);
	rest4_button->setChecked(false);
	rest8_button->setChecked(false);
	rest16_button->setChecked(false);
	rest32_button->setChecked(false);
}

NoteTab::NoteTab(QWidget *parent) : QWidget(parent)
{
	QHBoxLayout *noteRow = new QHBoxLayout;
	note1_button = new QPushButton(QIcon(":/images/whole_note.gif"),"");
	note1_button->setCheckable(true);
	note1_button->setChecked(true);
	noteRow->addWidget(note1_button);
	note2_button = new QPushButton(QIcon(":/images/half_note.gif"),"");
	note2_button->setCheckable(true);
	noteRow->addWidget(note2_button);
	note4_button = new QPushButton(QIcon(":/images/quarter_note.gif"),"");
	note4_button->setCheckable(true);
	noteRow->addWidget(note4_button);
	note8_button = new QPushButton(QIcon(":/images/8th_note.gif"),"");
	note8_button->setCheckable(true);
	noteRow->addWidget(note8_button);
	note16_button = new QPushButton(QIcon(":/images/16th_note.gif"),"");
	note16_button->setCheckable(true);
	noteRow->addWidget(note16_button);
	note32_button =new QPushButton(QIcon(":/images/32th_note.gif"),"");
	note32_button->setCheckable(true);
	noteRow->addWidget(note32_button);

	QHBoxLayout *restRow = new QHBoxLayout;
	rest1_button = new QPushButton;
	rest1_button->setText("1");
	rest1_button->setCheckable(true);
	restRow->addWidget(rest1_button);
	rest2_button = new QPushButton;
	rest2_button->setText("2");
	rest2_button->setCheckable(true);
	restRow->addWidget(rest2_button);
	rest4_button = new QPushButton;
	rest4_button->setText("4");
	rest4_button->setCheckable(true);
	restRow->addWidget(rest4_button);
	rest8_button = new QPushButton;
	rest8_button->setText("8");
	rest8_button->setCheckable(true);
	restRow->addWidget(rest8_button);
	rest16_button = new QPushButton;
	rest16_button->setText("16");
	rest16_button->setCheckable(true);
	restRow->addWidget(rest16_button);
	rest32_button = new QPushButton;
	rest32_button->setText("32");
	rest32_button->setCheckable(true);
	restRow->addWidget(rest32_button);

	connect(note1_button,SIGNAL(pressed()),this,SLOT(ResetNoteAndRestButtons()));
	connect(note2_button,SIGNAL(pressed()),this,SLOT(ResetNoteAndRestButtons()));
	connect(note4_button,SIGNAL(pressed()),this,SLOT(ResetNoteAndRestButtons()));
	connect(note8_button,SIGNAL(pressed()),this,SLOT(ResetNoteAndRestButtons()));
	connect(note16_button,SIGNAL(pressed()),this,SLOT(ResetNoteAndRestButtons()));
	connect(note32_button,SIGNAL(pressed()),this,SLOT(ResetNoteAndRestButtons()));

	connect(rest1_button,SIGNAL(pressed()),this,SLOT(ResetNoteAndRestButtons()));
	connect(rest2_button,SIGNAL(pressed()),this,SLOT(ResetNoteAndRestButtons()));
	connect(rest4_button,SIGNAL(pressed()),this,SLOT(ResetNoteAndRestButtons()));
	connect(rest8_button,SIGNAL(pressed()),this,SLOT(ResetNoteAndRestButtons()));
	connect(rest16_button,SIGNAL(pressed()),this,SLOT(ResetNoteAndRestButtons()));
	connect(rest32_button,SIGNAL(pressed()),this,SLOT(ResetNoteAndRestButtons()));

	QVBoxLayout *mainLayout = new QVBoxLayout;
	mainLayout->addSpacing(5);
	mainLayout->addLayout(noteRow);
	mainLayout->addLayout(restRow);
	mainLayout->addStretch(1);
	setLayout(mainLayout);
}

OrnamentTab::OrnamentTab(QWidget *parent) : QWidget(parent)
{

}

ScoreTab::ScoreTab(QWidget *parent) : QWidget(parent)
{

}

/*
void Toolbox::CreateNoteTab()
{
	QToolBox *note_tab = new QToolBox(this);

	QVBoxLayout *note_layout = new QVBoxLayout(this->note_tab);

	QHBoxLayout *first_row = new QHBoxLayout;

	QPushButton *button1 = new QPushButton;
	button1->setText("1");
	first_row->addWidget(button1);
	QPushButton *button2 = new QPushButton;
	button2->setText("2");
	first_row->addWidget(button2);
	QPushButton *button3 = new QPushButton;
	button3->setText("4");
	first_row->addWidget(button3);
	QPushButton *button4 = new QPushButton;
	button4->setText("8");
	first_row->addWidget(button4);
	QPushButton *button5 = new QPushButton;
	button5->setText("16");
	first_row->addWidget(button5);
	QPushButton *button6 = new QPushButton;
	button6->setText("32");
	first_row->addWidget(button6);

	note_layout->addLayout(first_row);

	addTab(note_tab,QIcon(":/icons/toolbox_note.png"),"");
}

void Toolbox::CreateOrnamentTab()
{
	QToolBox *ornament_tab = new QToolBox;
	addTab(ornament_tab,QIcon(":/icons/toolbox_embellishment.png"),"");
}

void Toolbox::CreateScoreTab()
{
	QToolBox *score_tab = new QToolBox;
	addTab(score_tab,QIcon(":/icons/toolbox_score.png"),"");
}

void Toolbox::CreateTabs()
{
	CreateNoteTab();
	CreateOrnamentTab();
	CreateScoreTab();
}
*/
