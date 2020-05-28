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
  
#include "keyboardsettingsdialog.h"
#include "ui_keyboardsettingsdialog.h"

#include <app/command.h>
#include <QKeyEvent>
#include <QMessageBox>

Q_DECLARE_METATYPE(Command *)

KeyboardSettingsDialog::KeyboardSettingsDialog(
    QWidget *parent, const std::vector<Command *> &commands)
    : QDialog(parent), ui(new Ui::KeyboardSettingsDialog), myCommands(commands)
{
    ui->setupUi(this);

    connect(ui->buttonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(ui->buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);

    initializeCommandTable();

    ui->shortcutEdit->installEventFilter(this);

    connect(ui->resetButton, &QPushButton::clicked, this,
            &KeyboardSettingsDialog::resetShortcut);

    connect(ui->commandsList, &QTreeWidget::currentItemChanged, this,
            &KeyboardSettingsDialog::activeCommandChanged);

    connect(ui->defaultButton, &QPushButton::clicked, this,
            &KeyboardSettingsDialog::resetToDefaultShortcut);
    ui->shortcutEdit->setFocus();
}

KeyboardSettingsDialog::~KeyboardSettingsDialog()
{
    delete ui;
}

void KeyboardSettingsDialog::initializeCommandTable()
{
    std::sort(myCommands.begin(), myCommands.end(),
              [](const Command *a, const Command *b) {
        return a->id() < b->id();
    });

    ui->commandsList->setColumnCount(3);

    ui->commandsList->setHeaderLabels(
        { tr("Command"), tr("Label"), tr("Shortcut") });

    // Populate list of commands.
    for (Command *command : myCommands)
    {
        QString shortcut_text =
            command->shortcut().toString(QKeySequence::NativeText);

        // Build up a set of the known shortcuts.
        if (!command->shortcut().isEmpty())
        {
            auto result =
                myKnownShortcuts.emplace(shortcut_text.toStdString(),
                                         ui->commandsList->topLevelItemCount());
            // We shouldn't have duplicate shortcuts.
            Q_ASSERT_X(result.second, "Import Keyboard Settings",
                       "Duplicate Shortcut");
        }

        // NOTE: QAction::toolTip() is called to avoid getting ampersands from
        //       mnemonics (which would appear in QAction::text)
        auto item = new QTreeWidgetItem(
            QStringList({ command->id(), command->toolTip(), shortcut_text }));

        item->setData(0, Qt::UserRole, QVariant::fromValue(command));
        ui->commandsList->addTopLevelItem(item);
    }

    ui->commandsList->header()->sectionResizeMode(QHeaderView::ResizeToContents);

    // resize dialog to avoid horizontal scrollbars
    int totalWidth = 0;
    for (int i = 0; i < ui->commandsList->columnCount(); i++)
    {
        totalWidth += ui->commandsList->columnWidth(i);
    }

    resize(totalWidth + 50, height());

    ui->commandsList->setCurrentItem(ui->commandsList->itemAt(0, 0));
}

bool KeyboardSettingsDialog::eventFilter(QObject *, QEvent *e)
{
    if (e->type() == QEvent::KeyPress)
    {
        QKeyEvent *k = static_cast<QKeyEvent *>(e);
        processKeyPress(k);
        return true;
    }

    // Ignore these events.
    if (e->type() == QEvent::KeyRelease)
    {
        return true;
    }

    return false;
}

void KeyboardSettingsDialog::processKeyPress(QKeyEvent *e)
{
    int key = e->key();

    // Ignore a modifer key by itself (i.e. just the Ctrl key).
    if (key == Qt::Key_Control || key == Qt::Key_Shift ||
            key == Qt::Key_Meta || key == Qt::Key_Alt)
    {
        return;
    }

    QTreeWidgetItem *item = ui->commandsList->currentItem();
    QString text = item->text(CommandShortcut);
    
    if (key == Qt::Key_Backspace && !text.isEmpty())
    {
        // Remove if there is a shortcut already present (and backspace is pressed)
        setShortcut(QKeySequence());
    }
    else
    {
        // Add in any modifers like Shift or Ctrl, but remove the keypad modifer
        // since QKeySequence doesn't handle that well.
        key |= (e->modifiers() & ~Qt::KeypadModifier);
        
        setShortcut(key);
    }

    e->accept();
}

void KeyboardSettingsDialog::resetShortcut()
{
    setShortcut(activeCommand()->shortcut());
}

void KeyboardSettingsDialog::resetToDefaultShortcut()
{
    setShortcut(activeCommand()->defaultShortcut());
}

void KeyboardSettingsDialog::setShortcut(const QKeySequence &shortcut,
                                         QTreeWidgetItem *item)
{
    if (!item)
        item = ui->commandsList->currentItem();
    
    const QString shortcut_text = shortcut.toString(QKeySequence::NativeText);
    
    // Check whether this shortcut is already in use by a different command.
    auto it = myKnownShortcuts.find(shortcut_text.toStdString());
    if (it != myKnownShortcuts.end())
    {
        QTreeWidgetItem *conflict_item =
            ui->commandsList->topLevelItem(it->second);
        if (item == conflict_item)
            return;
        
        auto conflict = conflict_item->data(0, Qt::UserRole).value<Command *>();

        QMessageBox msg;
        msg.setIcon(QMessageBox::Question);
        msg.setText(
            tr("The shortcut %1 is already in use.").arg(shortcut_text));
        msg.setInformativeText(
            tr("Do you want to use this shortcut and remove the shortcut of "
               "the <b>%1</b> command?").arg(conflict->id()));
        msg.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
        msg.setDefaultButton(QMessageBox::Yes);

        if (msg.exec() == QMessageBox::Yes)
            setShortcut(QKeySequence(), conflict_item);
        else
            return;
    }

    // Update the hash table of shortcuts.
    myKnownShortcuts.erase(item->text(CommandShortcut).toStdString());
    if (!shortcut.isEmpty())
    {
        myKnownShortcuts.emplace(shortcut_text.toStdString(),
                                 ui->commandsList->indexOfTopLevelItem(item));
    }
    
    item->setText(CommandShortcut, shortcut_text);
    if (item == ui->commandsList->currentItem())
        ui->shortcutEdit->setText(shortcut_text);
}

void KeyboardSettingsDialog::activeCommandChanged(QTreeWidgetItem* current,
                                                  QTreeWidgetItem* /*previous*/)
{
    ui->shortcutEdit->setText(current->text(CommandShortcut));
    ui->shortcutEdit->setFocus();
}

void KeyboardSettingsDialog::saveShortcuts()
{
    for (int i = 0; i < ui->commandsList->topLevelItemCount(); i++)
    {
        QTreeWidgetItem *currentItem = ui->commandsList->topLevelItem(i);
        Command* command = currentItem->data(0, Qt::UserRole).value<Command*>();

        command->setShortcut(currentItem->text(CommandShortcut));
    }
}

void KeyboardSettingsDialog::accept()
{
    saveShortcuts();
    done(Accepted);
}

Command *KeyboardSettingsDialog::activeCommand() const
{
    return ui->commandsList->currentItem()->data(0, Qt::UserRole).value<Command*>();
}
