#ifndef ADDSYSTEM_H
#define ADDSYSTEM_H

#include <QUndoCommand>

class Score;

class AddSystem : public QObject, public QUndoCommand
{
    Q_OBJECT
public:
    AddSystem(Score* score, quint32 index);
    void redo();
    void undo();

signals:
    void triggered();

protected:
    Score* score;
    const quint32 index;
};

#endif // ADDSYSTEM_H
