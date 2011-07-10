#include "keyboardsettingsdialog.h"
#include "ui_keyboardsettingsdialog.h"

#include <app/command.h>

#include <QKeyEvent>

Q_DECLARE_METATYPE(Command*)

KeyboardSettingsDialog::KeyboardSettingsDialog(const QList<Command*>& commands,
                                               QWidget* parent) :
    QDialog(parent),
    ui(new Ui::KeyboardSettingsDialog),
    commands(commands)
{
    ui->setupUi(this);

    initializeCommandTable();

    ui->shortcutEdit->installEventFilter(this);

    connect(ui->resetButton, SIGNAL(clicked()), this, SLOT(resetShortcut()));

    connect(ui->commandsList, SIGNAL(currentItemChanged(QTreeWidgetItem*,QTreeWidgetItem*)),
            this, SLOT(activeCommandChanged(QTreeWidgetItem*,QTreeWidgetItem*)));

    connect(ui->defaultButton, SIGNAL(clicked()), this, SLOT(resetToDefaultShortcut()));
}

KeyboardSettingsDialog::~KeyboardSettingsDialog()
{
    delete ui;
}

namespace
{
bool compareCommands(const Command* cmd1, const Command* cmd2)
{
    return cmd1->id() < cmd2->id();
}
}

/// Setup: load the commands into the table widget, etc
void KeyboardSettingsDialog::initializeCommandTable()
{
    qSort(commands.begin(), commands.end(), compareCommands);

    ui->commandsList->setColumnCount(3);

    ui->commandsList->setHeaderLabels(QStringList() << tr("Command") << tr("Label") << tr("Shortcut"));

    // populate list of commands
    foreach(Command* command, commands)
    {
        QTreeWidgetItem* item = new QTreeWidgetItem(QStringList() << command->id() <<
                                                    command->text() << command->shortcut().toString());
        item->setData(0, Qt::UserRole, qVariantFromValue(command));
        ui->commandsList->addTopLevelItem(item);
    }

    ui->commandsList->header()->setResizeMode(QHeaderView::ResizeToContents);

    // resize dialog to avoid horizontal scrollbars
    int totalWidth = 0;
    for (int i = 0; i < ui->commandsList->columnCount(); i++)
    {
        totalWidth += ui->commandsList->columnWidth(i);
    }

    resize(totalWidth + 50, height());

    ui->commandsList->setCurrentItem(ui->commandsList->itemAt(0, 0));
}

/// Capture the key presses as they are typed, in order to get the key sequence
bool KeyboardSettingsDialog::eventFilter(QObject*, QEvent *e)
{
    if (e->type() == QEvent::KeyPress)
    {
        QKeyEvent* k = static_cast<QKeyEvent*>(e);
        processKeyPress(k);
        return true;
    }

    // ignore these events
    if (e->type() == QEvent::KeyRelease)
    {
        return true;
    }

    return false;
}

/// Process the key press and update the key sequence
void KeyboardSettingsDialog::processKeyPress(QKeyEvent *e)
{
    int key = e->key();

    // ignore a modifer key by itself (i.e. just the Ctrl key)
    if (key == Qt::Key_Control || key == Qt::Key_Shift ||
            key == Qt::Key_Meta || key == Qt::Key_Alt)
    {
        return;
    }

    // allow the use of backspace to clear the shortcut
    if (key == Qt::Key_Backspace)
    {
        setShortcut("");
    }
    else
    {
        key |= e->modifiers();

        QKeySequence newKeySequence(key);
        setShortcut(newKeySequence.toString());
    }

    e->accept();
}

/// Reset the shortcut for the selected command to the value it had before editing began
void KeyboardSettingsDialog::resetShortcut()
{
    setShortcut(activeCommand()->shortcut());
}

/// Reset the active command to its default shortcut
void KeyboardSettingsDialog::resetToDefaultShortcut()
{
    setShortcut(activeCommand()->defaultShortcut());
}

/// Updates the shortcut in the commands list and the shortcut editor
void KeyboardSettingsDialog::setShortcut(const QString& shortcut)
{
    ui->commandsList->currentItem()->setText(CommandShortcut, shortcut);
    ui->shortcutEdit->setText(shortcut);
}

/// Switch the text in the shortcut editor when the selected command changes
void KeyboardSettingsDialog::activeCommandChanged(QTreeWidgetItem* current,
                                                  QTreeWidgetItem* /*previous*/)
{
    ui->shortcutEdit->setText(current->text(CommandShortcut));
}

/// Save all of the shortcuts
void KeyboardSettingsDialog::saveShortcuts()
{
    for (int i = 0; i < ui->commandsList->topLevelItemCount(); i++)
    {
        QTreeWidgetItem* currentItem = ui->commandsList->topLevelItem(i);
        Command* command = qVariantValue<Command*>(currentItem->data(0, Qt::UserRole));

        command->setShortcut(currentItem->text(CommandShortcut));
    }
}

/// Save the shortcuts and close the dialog
void KeyboardSettingsDialog::accept()
{
    saveShortcuts();

    done(Accepted);
}

/// Get the Command corresponding to the currently selected row
Command * KeyboardSettingsDialog::activeCommand() const
{
    return qVariantValue<Command*>(ui->commandsList->currentItem()->data(0, Qt::UserRole));
}
