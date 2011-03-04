#include "addtrill.h"

#include <powertabdocument/note.h>

AddTrill::AddTrill(Note* note, quint8 trillFret) :
    note(note),
    trillFret(trillFret)
{
    Q_ASSERT(!note->HasTrill()); // should be editing a note without any current trill

    setText(QObject::tr("Add Trill"));
}

void AddTrill::redo()
{
    note->SetTrill(trillFret);
}

void AddTrill::undo()
{
    note->ClearTrill();
}
