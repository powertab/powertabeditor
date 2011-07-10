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
