#include "removedynamic.h"

#include <powertabdocument/score.h>

RemoveDynamic::RemoveDynamic(Score* score, std::shared_ptr<Dynamic> dynamic) :
    score(score), dynamic(dynamic)
{
    setText(QObject::tr("Remove Dynamic"));
}

void RemoveDynamic::redo()
{
    score->RemoveDynamic(dynamic);
}

void RemoveDynamic::undo()
{
    score->InsertDynamic(dynamic);
}
