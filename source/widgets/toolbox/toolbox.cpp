#include "toolbox.h"

#include <skinmanager.h>
#include "notepage.h"
#include "scorepage.h"

ScorePage *Toolbox::scorePage = NULL;
NotePage *Toolbox::notePage = NULL;

Toolbox::Toolbox(QWidget *parent, SkinManager *skinManager) :
    QTabWidget(parent)
{
    setMaximumWidth(350);
    setIconSize(QSize(48,48));
    setTabPosition(QTabWidget::West);

    setStyleSheet(skinManager->getToolboxTabStyle());

    scorePage = new ScorePage(0,skinManager);
    addTab(scorePage, QIcon(":/icons/toolbox_score.png"),"");

    notePage = new NotePage(0,skinManager);
    addTab(notePage, QIcon(":/icons/toolbox_note.png"),"");
}
