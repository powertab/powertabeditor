#ifndef EDITTIMESIGNATURE_H
#define EDITTIMESIGNATURE_H

#include <QUndoCommand>

class Score;
#include <powertabdocument/systemlocation.h>
#include <powertabdocument/timesignature.h>

class EditTimeSignature : public QUndoCommand
{
public:
    EditTimeSignature(Score* score, const SystemLocation location,
                      const TimeSignature& newTimeSig);

    void redo();
    void undo();

private:
    Score* score;
    const SystemLocation location;
    TimeSignature newTimeSig;
    TimeSignature oldTimeSig;

    void switchTimeSignatures(const TimeSignature& oldTime,
                                                 const TimeSignature& newTime);
};

#endif // EDITTIMESIGNATURE_H
