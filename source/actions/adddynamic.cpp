#include "adddynamic.h"

#include <powertabdocument/score.h>

AddDynamic::AddDynamic(Score* score, std::shared_ptr<Dynamic> dynamic) :
    score(score),
    dynamic(dynamic)
{
    setText(QObject::tr("Add Dynamic"));
}

void AddDynamic::redo()
{
    score->InsertDynamic(dynamic);
}

void AddDynamic::undo()
{
    score->RemoveDynamic(dynamic);
}
