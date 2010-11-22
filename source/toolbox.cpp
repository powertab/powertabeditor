#include "toolbox.h"

#include <QHBoxLayout>
#include <QPushButton>
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

NoteTab::NoteTab(QWidget *parent) : QWidget(parent)
{
	QHBoxLayout *noteRow = new QHBoxLayout;
	QPushButton *button1 = new QPushButton;
	button1->setText("1");
	button1->setCheckable(true);
	button1->setChecked(true);
	noteRow->addWidget(button1);
	QPushButton *button2 = new QPushButton;
	button2->setText("2");
	button2->setCheckable(true);
	noteRow->addWidget(button2);
	QPushButton *button3 = new QPushButton;
	button3->setText("4");
	button3->setCheckable(true);
	noteRow->addWidget(button3);
	QPushButton *button4 = new QPushButton;
	button4->setText("8");
	button4->setCheckable(true);
	noteRow->addWidget(button4);
	QPushButton *button5 = new QPushButton;
	button5->setText("16");
	button5->setCheckable(true);
	noteRow->addWidget(button5);
	QPushButton *button6 = new QPushButton;
	button6->setText("32");
	button6->setCheckable(true);
	noteRow->addWidget(button6);

	QHBoxLayout *restRow = new QHBoxLayout;
	QPushButton *rbutton1 = new QPushButton;
	rbutton1->setText("R1");
	restRow->addWidget(rbutton1);
	QPushButton *rbutton2 = new QPushButton;
	rbutton2->setText("R2");
	restRow->addWidget(rbutton2);
	QPushButton *rbutton3 = new QPushButton;
	rbutton3->setText("R4");
	restRow->addWidget(rbutton3);
	QPushButton *rbutton4 = new QPushButton;
	rbutton4->setText("R8");
	restRow->addWidget(rbutton4);
	QPushButton *rbutton5 = new QPushButton;
	rbutton5->setText("R16");
	restRow->addWidget(rbutton5);
	QPushButton *rbutton6 = new QPushButton;
	rbutton6->setText("R32");
	restRow->addWidget(rbutton6);

	QVBoxLayout *mainLayout = new QVBoxLayout;
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
