#ifndef UNDOMANAGER_H
#define UNDOMANAGER_H

#include <QUndoGroup>
#include <QList>
#include <QUndoStack>

class QUndoCommand;

class UndoManager : public QUndoGroup
{
    Q_OBJECT
public:
    explicit UndoManager(QObject *parent = 0);
    ~UndoManager();

    void addNewUndoStack();
    void setActiveStackIndex(int index);
    void removeStack(int index);
    void push(QUndoCommand* cmd);

private:
    QList<QUndoStack*> undoStacks;

signals:

public slots:

};

#endif // UNDOMANAGER_H
