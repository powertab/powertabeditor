#ifndef EDITREHEARSALSIGN_H
#define EDITREHEARSALSIGN_H

#include <QUndoCommand>

class Barline;
class RehearsalSign;

class EditRehearsalSign : public QUndoCommand
{
public:
    EditRehearsalSign(RehearsalSign* rehearsalSign, bool isShown, quint8 letter = 0, std::string description = "");

    virtual void undo();
    virtual void redo();

protected:
    RehearsalSign* rehearsalSign;
    bool isShown;
    quint8 letter;
    std::string description;

    void showHide(bool show);
};

#endif // EDITREHEARSALSIGN_H
