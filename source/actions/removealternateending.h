#ifndef REMOVEALTERNATEENDING_H
#define REMOVEALTERNATEENDING_H

#include <QUndoCommand>
#include <memory>

class Score;
class AlternateEnding;

class RemoveAlternateEnding : public QUndoCommand
{
public:
    RemoveAlternateEnding(Score* score, std::shared_ptr<AlternateEnding> altEnding);

    void redo();
    void undo();

private:
    Score* score;
    std::shared_ptr<AlternateEnding> altEnding;
};

#endif // REMOVEALTERNATEENDING_H
