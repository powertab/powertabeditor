#ifndef REMOVESYSTEM_H
#define REMOVESYSTEM_H

#include <QUndoCommand>

class Score;

class RemoveSystem : public QObject, public QUndoCommand
{
    Q_OBJECT
public:
    RemoveSystem(Score* score, quint32 index);
    void redo();
    void undo();

signals:
    void triggered();

protected:
    Score* score;
    const quint32 index;
};

#endif // REMOVESYSTEM_H
