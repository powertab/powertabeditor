#ifndef UNDOMANAGER_H
#define UNDOMANAGER_H

#include <QUndoGroup>
#include <QUndoStack>

#include <boost/ptr_container/ptr_vector.hpp>

class QUndoCommand;

class UndoManager : public QUndoGroup
{
    Q_OBJECT
public:
    explicit UndoManager(QObject *parent = 0);

    void addNewUndoStack();
    void setActiveStackIndex(int index);
    void removeStack(int index);
    void push(QUndoCommand* cmd);

private:
    boost::ptr_vector<QUndoStack> undoStacks;
};

#endif // UNDOMANAGER_H
