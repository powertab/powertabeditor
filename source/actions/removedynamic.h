#ifndef REMOVEDYNAMIC_H
#define REMOVEDYNAMIC_H

#include <QUndoCommand>
#include <memory>

class Score;
class Dynamic;

class RemoveDynamic : public QUndoCommand
{
public:
    RemoveDynamic(Score* score, std::shared_ptr<Dynamic> dynamic);

    void redo();
    void undo();

private:
    Score* score;
    std::shared_ptr<Dynamic> dynamic;
};

#endif // REMOVEDYNAMIC_H
