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
  
#ifndef APP_COMMAND_H
#define APP_COMMAND_H

#include <QAction>

class SettingsTree;

/// An extension of QAction, supporting customizable shortcuts, default key
/// sequences, etc.
class Command : public QAction
{
    Q_OBJECT

public:
    Command(const QString &text, const QString &id,
            const QKeySequence &defaultShortcut, QObject *parent,
            const QString &iconFileName = QString());

    QString id() const;
    QKeySequence defaultShortcut() const;

    /// Loads the shortcut for the command - if there is not a customized
    /// shortcut in the settings file, then the default shortcut is used.
    void load(const SettingsTree &settings);

    /// Saves the command's shortcut.
    void save(SettingsTree &settings) const;

private:
    /// Returns the name of the key used for storing the customized shortcut.
    std::string getSettingsKey() const;

    /// Unique identifer (used when storing a customized key sequence).
    const QString myId;
    const QKeySequence myDefaultShortcut;
};

#endif
