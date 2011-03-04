#include "removetrill.h"

#include <powertabdocument/note.h>

RemoveTrill::RemoveTrill(Note* note) :
    note(note)
{
    note->GetTrill(originalTrill);

    setText(QObject::tr("Remove Trill"));
}

void RemoveTrill::redo()
{
    note->ClearTrill();
}

void RemoveTrill::undo()
{
    note->SetTrill(originalTrill);
}
