#include "toolbox.h"

#include <skinmanager.h>
#include "notepage.h"
#include "scorepage.h"

ScorePage *Toolbox::scorePage = NULL;
NotePage *Toolbox::notePage = NULL;

Toolbox::Toolbox(PowerTabEditor* mainWindow, std::shared_ptr<SkinManager> skinManager)
{
    setMaximumWidth(350);
    setIconSize(QSize(48,48));
    setTabPosition(QTabWidget::West);

    setStyleSheet(skinManager->getToolboxTabStyle());

    scorePage = new ScorePage(mainWindow, skinManager, 0);
    addTab(scorePage, QIcon(":/icons/toolbox_score.png"),"");

    notePage = new NotePage(mainWindow, skinManager, 0);
    addTab(notePage, QIcon(":/icons/toolbox_note.png"),"");
}
