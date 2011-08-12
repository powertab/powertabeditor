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
    id_(id),
    defaultShortcut_(defaultShortcut)
{
    loadShortcut();
}

QString Command::id() const
{
    return id_;
}

QKeySequence Command::defaultShortcut() const
{
    return defaultShortcut_;
}

/// Set the shortcut for the command, and save it externally so that the setting will persist
void Command::setShortcut(const QKeySequence& shortcut)
{
    QAction::setShortcut(shortcut);

    QSettings settings;
    settings.setValue(KEY_PREFIX + id_, shortcut.toString());
}

/// Loads the shortcut for the command - if there is not a customized shortuct in the settings file,
/// then the default shortcut is used
void Command::loadShortcut()
{
    QSettings settings;

    const QString keySequence = settings.value(KEY_PREFIX + id_,
                                               defaultShortcut_.toString()).toString();

    QAction::setShortcut(keySequence);
}
