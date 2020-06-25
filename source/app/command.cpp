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
  
#include "command.h"

#include <app/settings.h>
#include <util/settingstree.h>

const std::string theKeyPrefix("shortcuts/");

Command::Command(const QString &text, const QString &id,
                 const QKeySequence &default_shortcut, QObject *parent,
                 const QString &iconFileName)
    : QAction(text, parent), myId(id), myDefaultShortcut(default_shortcut)
{
    setShortcut(myDefaultShortcut);

    setIconVisibleInMenu(false);

    if (!iconFileName.isNull())
        setIcon(QIcon(iconFileName));
}

QString Command::id() const
{
    return myId;
}

QKeySequence Command::defaultShortcut() const
{
    return myDefaultShortcut;
}

void Command::save(SettingsTree &settings) const
{
    // Only save shortcuts that are different from the default.
    if (shortcut() == myDefaultShortcut)
        settings.remove(getSettingsKey());
    else
        settings.set(getSettingsKey(), shortcut());
}

void Command::load(const SettingsTree &settings)
{
    setShortcut(settings.get(getSettingsKey(), myDefaultShortcut));
}

std::string Command::getSettingsKey() const
{
    return theKeyPrefix + myId.toStdString();
}
