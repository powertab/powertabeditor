#ifndef ADDDYNAMIC_H
#define ADDDYNAMIC_H

#include <QUndoCommand>
#include <memory>

class Score;
class Dynamic;

class AddDynamic : public QUndoCommand
{
public:
    AddDynamic(Score* score, std::shared_ptr<Dynamic> dynamic);

    void redo();
    void undo();

private:
    Score* score;
    std::shared_ptr<Dynamic> dynamic;
};

#endif // ADDDYNAMIC_H
