#include "editslideout.h"

#include <powertabdocument/note.h>

EditSlideOut::EditSlideOut(Note* note, quint8 slideType, qint8 steps) :
    note(note),
    newSlideType(slideType),
    newSteps(steps)
{
    Q_ASSERT(Note::IsValidSlideOutOfType(slideType));

    QString undoText;

    if (slideType == Note::slideOutOfShiftSlide)
        undoText = QObject::tr("Shift Slide");
    else if (slideType == Note::slideOutOfLegatoSlide)
        undoText = QObject::tr("Legato Slide");
    else if (slideType == Note::slideOutOfDownwards)
        undoText = QObject::tr("Slide Out Of Downwards");
    else if (slideType == Note::slideOutOfUpwards)
        undoText = QObject::tr("Slide Out Of Upwards");
    else if (slideType == Note::slideOutOfNone)
        undoText = QObject::tr("Remove Slide");

    setText(undoText);

    note->GetSlideOutOf(oldSlideType, oldSteps);
}

void EditSlideOut::redo()
{
    note->SetSlideOutOf(newSlideType, newSteps);
}

void EditSlideOut::undo()
{
    note->SetSlideOutOf(oldSlideType, oldSteps);
}
