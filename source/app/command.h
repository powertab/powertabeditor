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
