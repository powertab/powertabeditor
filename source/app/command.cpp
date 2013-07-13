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

#include <QSettings>

const QString Command::KEY_PREFIX = "shortcuts/";

Command::Command(const QString& text, const QString& id,
                 const QKeySequence& defaultShortcut, QObject* parent) :
    QAction(text, parent),
    myId(id),
    myDefaultShortcut(defaultShortcut)
{
    loadShortcut();
}

QString Command::id() const
{
    return myId;
}

QKeySequence Command::defaultShortcut() const
{
    return myDefaultShortcut;
}

void Command::setShortcut(const QKeySequence& shortcut)
{
    QAction::setShortcut(shortcut);

    QSettings settings;
    // Only save shortcuts that are different from the default.
    if (shortcut == defaultShortcut())
    {
        settings.remove(settingsKey());
    }
    else
    {
        settings.setValue(settingsKey(), shortcut.toString());
    }
}

void Command::loadShortcut()
{
    QSettings settings;

    const QString keySequence = settings.value(settingsKey(),
                                               myDefaultShortcut.toString()).toString();

    QAction::setShortcut(keySequence);
}

QString Command::settingsKey() const
{
    return KEY_PREFIX + myId;
}
