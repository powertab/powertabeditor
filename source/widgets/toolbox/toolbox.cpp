#include "toolbox.h"

#include <QHBoxLayout>
#include <QLabel>

ScorePage *Toolbox::scorePage = NULL;
NotePage *Toolbox::notePage = NULL;

Toolbox::Toolbox(QWidget *parent) :
	QTabWidget(parent)
{
	setMaximumWidth(350);
	setIconSize(QSize(48,48));
	setTabPosition(QTabWidget::West);

	scorePage = new ScorePage;
	addTab(scorePage, QIcon(":/icons/toolbox_score.png"),"");

	notePage = new NotePage;
	addTab(notePage, QIcon(":/icons/toolbox_note.png"),"");
}
