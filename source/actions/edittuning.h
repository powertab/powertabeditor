#ifndef EDITTUNING_H
#define EDITTUNING_H

#include <QUndoCommand>

#include <powertabdocument/tuning.h>
#include <memory>

class Guitar;

class EditTuning : public QUndoCommand
{
public:
    EditTuning(std::shared_ptr<Guitar> guitar, const Tuning& newTuning);
    void redo();
    void undo();
    
private:
    std::shared_ptr<Guitar> guitar;
    const Tuning newTuning;
    const Tuning oldTuning;
};

#endif // EDITTUNING_H
