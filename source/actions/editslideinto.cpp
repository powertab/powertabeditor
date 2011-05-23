#include "editslideinto.h"

#include <powertabdocument/note.h>

EditSlideInto::EditSlideInto(Note* note, uint8_t newSlideIntoType) :
    note(note),
    newSlideIntoType(newSlideIntoType)
{
    note->GetSlideInto(oldSlideIntoType);

    switch(newSlideIntoType)
    {
    case Note::slideIntoNone:
        setText(QObject::tr("Remove Slide Into"));
        break;

    case Note::slideIntoFromBelow:
        setText(QObject::tr("Slide Into From Below"));
        break;

    case Note::slideIntoFromAbove:
        setText(QObject::tr("Slide Into From Above"));
        break;

    default:
        Q_ASSERT(false); // invalid/unsupported slide type
        break;
    }
}

void EditSlideInto::redo()
{
    note->SetSlideInto(newSlideIntoType);
}

void EditSlideInto::undo()
{
    note->SetSlideInto(oldSlideIntoType);
}
