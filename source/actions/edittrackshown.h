#ifndef EDITTRACKSHOWN_H
#define EDITTRACKSHOWN_H

#include <QUndoCommand>
#include <boost/cstdint.hpp>

class Score;

class EditTrackShown : public QObject, public QUndoCommand
{
    Q_OBJECT
public:
    EditTrackShown(Score* score, uint32_t trackNumber, bool trackShown);

    void redo();
    void undo();

signals:
    void triggered();

private:
    Score* score;
    const uint32_t trackNumber;
    const bool trackShown;

    void toggleShown(bool show);
};

#endif // EDITTRACKSHOWN_H
