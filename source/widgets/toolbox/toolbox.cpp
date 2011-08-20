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
  
#include "toolbox.h"

#include <app/skinmanager.h>
#include "notepage.h"
#include "scorepage.h"

Toolbox::Toolbox(PowerTabEditor* mainWindow, boost::shared_ptr<SkinManager> skinManager) :
    scorePage(new ScorePage(mainWindow, skinManager)),
    notePage(new NotePage(mainWindow, skinManager))
{
    setMaximumWidth(350);
    setIconSize(QSize(48,48));
    setTabPosition(QTabWidget::West);

    setStyleSheet(skinManager->getToolboxTabStyle());

    addTab(scorePage, QIcon(":/icons/toolbox_score.png"),"");
    addTab(notePage, QIcon(":/icons/toolbox_note.png"),"");
}
