#ifndef ADDGUITAR_H
#define ADDGUITAR_H

#include <QUndoCommand>
#include <memory>

class Guitar;
class Score;
class Mixer;

class AddGuitar : public QObject, public QUndoCommand
{
    Q_OBJECT
public:
    AddGuitar(Score* score, Mixer* mixer);
    void redo();
    void undo();

signals:
    void triggered();

private:
    Score* score;
    Mixer* mixer;
    std::shared_ptr<Guitar> newGuitar;
};

#endif // ADDGUITAR_H
