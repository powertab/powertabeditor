#include "scorepage.h"

ScorePage::ScorePage(QFrame *parent, SkinManager *skinManager) : QFrame(parent)
{
	setFrameStyle(QFrame::StyledPanel);

	setStyleSheet(skinManager->getToolboxPageStyle());
}
