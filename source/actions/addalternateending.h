#ifndef ADDALTERNATEENDING_H
#define ADDALTERNATEENDING_H

#include <QUndoCommand>
#include <memory>

class Score;
class AlternateEnding;

class AddAlternateEnding : public QUndoCommand
{
public:
    AddAlternateEnding(Score* score, std::shared_ptr<AlternateEnding> altEnding);

    void redo();
    void undo();

private:
    Score* score;
    std::shared_ptr<AlternateEnding> altEnding;
};

#endif // ADDALTERNATEENDING_H
