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
  
#ifndef DIALOGS_KEYBOARDSETTINGSDIALOG_H
#define DIALOGS_KEYBOARDSETTINGSDIALOG_H

#include <QDialog>
#include <string>
#include <vector>
#include <unordered_map>

namespace Ui {
    class KeyboardSettingsDialog;
}

class Command;
class QTreeWidgetItem;

class KeyboardSettingsDialog : public QDialog
{
    Q_OBJECT

public:
    KeyboardSettingsDialog(QWidget *parent,
                           const std::vector<Command *> &myCommands);
    ~KeyboardSettingsDialog();

private slots:
    /// Reset the shortcut for the selected command to the value it had before
    /// editing began.
    void resetShortcut();

    /// Reset the active command to its default shortcut.
    void resetToDefaultShortcut();

    /// Switch the text in the shortcut editor when the selected command changes.
    void activeCommandChanged(QTreeWidgetItem *current,
                              QTreeWidgetItem *previous);

    /// Save the shortcuts and close the dialog.
    virtual void accept() override;

private:
    /// Performs setup such as loading the commands into the table widget, etc.
    void initializeCommandTable();

    /// Capture the key presses as they are typed, in order to get the
    /// key sequence.
    virtual bool eventFilter(QObject *o, QEvent *e) override;

    /// Process the key press and update the key sequence.
    void processKeyPress(QKeyEvent *e);

    /// Save all of the shortcuts.
    void saveShortcuts();

    /// Updates the shortcut in the commands list and the shortcut editor.
    /// If 'item' is not specified, the currently selected item will be used.
    void setShortcut(const QKeySequence &shortcut, QTreeWidgetItem *item = 0);

    /// Get the Command corresponding to the currently selected row.
    Command *activeCommand() const;

    Ui::KeyboardSettingsDialog *ui;
    std::vector<Command*> myCommands;
    /// The known shortcuts (used when checking for duplicates).
    std::unordered_map<std::string, int> myKnownShortcuts;

    /// Corresponds to the columns used in the QTreeWidget for dealing
    /// with the data.
    enum Columns
    {
        CommandId,
        CommandLabel,
        CommandShortcut
    };
};

#endif
