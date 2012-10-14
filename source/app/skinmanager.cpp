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
#include <QDir>
#include <QSettings>
#include <app/settings.h>

SkinManager::SkinManager()
{
    reload();
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

QString SkinManager::readSegment(const QString& skinname, const QString& filename)
{
    QString out;
    QFile data;

    data.setFileName(skinDir() + "/" + skinname + "/" + filename);

    if(data.open(QFile::ReadOnly))
    {
        QTextStream styleIn(&data);
        out = styleIn.readAll();
        data.close();
    }

    return out;
}

/// Returns a list of all skins that can be used
QStringList SkinManager::availableSkins()
{
    return QDir(skinDir()).entryList(QDir::AllDirs | QDir::NoDotAndDotDot);
}

/// Returns the directory containing all skins
QString SkinManager::skinDir()
{
    return QCoreApplication::applicationDirPath() + "/skins";
}

/// Loads the skin specified in the application settings
/// TODO - ensure that any skinned widgets have their stylesheets updated
void SkinManager::reload()
{
    QString skinName;
    QSettings settings;

    if (settings.value(Settings::APPEARANCE_USE_SKIN).toBool())
    {
        skinName = settings.value(Settings::APPEARANCE_SKIN_NAME).toString();
    }

    documentTabStyle = readSegment(skinName,"document_tab.txt");
    toolboxTabStyle = readSegment(skinName,"toolbox_tab.txt");
    toolboxPageStyle = readSegment(skinName,"toolbox_page.txt");
}
