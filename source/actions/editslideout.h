#ifndef EDITSLIDEOUT_H
#define EDITSLIDEOUT_H

#include <QUndoCommand>

class Note;

class EditSlideOut : public QUndoCommand
{
public:
    EditSlideOut(Note* note, quint8 slideType, qint8 steps);
    void redo();
    void undo();

protected:
    Note* note;
    const quint8 newSlideType;
    quint8 oldSlideType;
    const qint8 newSteps;
    qint8 oldSteps;
};

#endif // EDITSLIDEOUT_H
