#ifndef EDITKEYSIGNATURE_H
#define EDITKEYSIGNATURE_H

#include <QUndoCommand>
#include <powertabdocument/systemlocation.h>
#include <powertabdocument/keysignature.h>

class Score;

class EditKeySignature : public QObject, public QUndoCommand
{
    Q_OBJECT

public:
    EditKeySignature(Score* score, const SystemLocation& location,
                     uint8_t newKeyType, uint8_t newKeyAccidentals, bool isShown);

    void redo();
    void undo();

signals:
    // need to trigger a full redraw of the whole score when editing a key signature
    void triggered();

private:
    void switchKeySignatures(const KeySignature& oldKey, const KeySignature& newKey);

    Score* score;
    const SystemLocation location;
    KeySignature newKeySig;
    KeySignature oldKeySig;
};

#endif // EDITKEYSIGNATURE_H
