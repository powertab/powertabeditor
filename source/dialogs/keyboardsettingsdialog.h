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
  
#ifndef KEYBOARDSETTINGSDIALOG_H
#define KEYBOARDSETTINGSDIALOG_H

#include <QDialog>

namespace Ui {
    class KeyboardSettingsDialog;
}

class Command;
class QTreeWidgetItem;

class KeyboardSettingsDialog : public QDialog
{
    Q_OBJECT

public:
    KeyboardSettingsDialog(const QList<Command*>& commands, QWidget* parent = 0);
    ~KeyboardSettingsDialog();

private slots:
    void resetShortcut();
    void resetToDefaultShortcut();
    void activeCommandChanged(QTreeWidgetItem* current, QTreeWidgetItem* previous);
    void accept();

private:
    void initializeCommandTable();
    bool eventFilter(QObject* o, QEvent* e);
    void processKeyPress(QKeyEvent* e);
    void saveShortcuts();
    void setShortcut(const QString& shortcut);
    Command* activeCommand() const;

    Ui::KeyboardSettingsDialog *ui;
    QList<Command*> commands;

    /// Corresponds to the columns used in the QTreeWidget for dealing with the data
    enum Columns
    {
        CommandId,
        CommandLabel,
        CommandShortcut
    };
};

#endif // KEYBOARDSETTINGSDIALOG_H
