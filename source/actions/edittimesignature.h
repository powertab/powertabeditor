#ifndef EDITTIMESIGNATURE_H
#define EDITTIMESIGNATURE_H

#include <QUndoCommand>

class Score;
#include <powertabdocument/systemlocation.h>
#include <powertabdocument/timesignature.h>

class EditTimeSignature : public QObject, public QUndoCommand
{
    Q_OBJECT

public:
    EditTimeSignature(Score* score, const SystemLocation location,
                      const TimeSignature& newTimeSig);

    void redo();
    void undo();

signals:
    // need to trigger a full redraw of the whole score when editing a time signature
    void triggered();

private:
    Score* score;
    const SystemLocation location;
    TimeSignature newTimeSig;
    TimeSignature oldTimeSig;

    void switchTimeSignatures(const TimeSignature& oldTime,
                                                 const TimeSignature& newTime);
};

#endif // EDITTIMESIGNATURE_H
