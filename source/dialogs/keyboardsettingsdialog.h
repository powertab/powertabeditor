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
