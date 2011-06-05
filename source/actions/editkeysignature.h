#ifndef EDITKEYSIGNATURE_H
#define EDITKEYSIGNATURE_H

#include <QUndoCommand>
#include <powertabdocument/systemlocation.h>
#include <powertabdocument/keysignature.h>

class Score;

class EditKeySignature : public QUndoCommand
{
public:
    EditKeySignature(Score* score, const SystemLocation& location,
                     uint8_t newKeyType, uint8_t newKeyAccidentals);

    void redo();
    void undo();

private:
    void switchKeySignatures(const KeySignature& oldKey, const KeySignature& newKey);

    Score* score;
    const SystemLocation location;
    const KeySignature newKeySig;
    KeySignature oldKeySig;
};

#endif // EDITKEYSIGNATURE_H
