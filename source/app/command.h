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
  
#ifndef COMMAND_H
#define COMMAND_H

#include <QAction>

/// An extension of QAction, supporting customizable shortcuts, default key sequences, etc
class Command : public QAction
{
    Q_OBJECT

public:
    Command(const QString& text, const QString& id,
            const QKeySequence& defaultShortcut, QObject* parent);

    QString id() const;
    QKeySequence defaultShortcut() const;

    void setShortcut(const QKeySequence& shortcut);

private:
    void loadShortcut();

    const QString id_; /// unique identifer (used when storing a customized key sequence)
    const QKeySequence defaultShortcut_;

    static const QString KEY_PREFIX; ///< prefix for all keys used with QSetting
};

#endif // COMMAND_H
