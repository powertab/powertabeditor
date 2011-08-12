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
  
#include "skinmanager.h"

#include <QCoreApplication>
#include <QFile>
#include <QTextStream>

SkinManager::SkinManager(const QString& filename)
{
    documentTabStyle = readSegment(filename,"document_tab.txt");

    toolboxTabStyle = readSegment(filename,"toolbox_tab.txt");
    toolboxPageStyle = readSegment(filename,"toolbox_page.txt");

    mixerStyle = readSegment(filename,"mixer.txt");
}

QString SkinManager::getDocumentTabStyle() const
{
    return documentTabStyle;
}

QString SkinManager::getToolboxTabStyle() const
{
    return toolboxTabStyle;
}

QString SkinManager::getToolboxPageStyle() const
{
    return toolboxPageStyle;
}

QString SkinManager::getMixerStyle() const
{
    return mixerStyle;
}

QString SkinManager::readSegment(const QString& skinname, const QString& filename)
{
    QString out;
    QFile data;

    data.setFileName(QCoreApplication::applicationDirPath()+"/skins/"+skinname+"/"+filename);

    if(data.open(QFile::ReadOnly))
    {
        QTextStream styleIn(&data);
        out = styleIn.readAll();
        data.close();
    }

    return out;
}
