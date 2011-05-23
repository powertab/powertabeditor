#ifndef EDITSLIDEINTO_H
#define EDITSLIDEINTO_H

#include <QUndoCommand>

class Note;

class EditSlideInto : public QUndoCommand
{
public:
    EditSlideInto(Note* note, uint8_t newSlideIntoType);
    void redo();
    void undo();

private:
    Note* note;
    uint8_t newSlideIntoType;
    uint8_t oldSlideIntoType;
};

#endif // EDITSLIDEINTO_H
