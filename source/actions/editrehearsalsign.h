#ifndef EDITREHEARSALSIGN_H
#define EDITREHEARSALSIGN_H

#include <QUndoCommand>

class Barline;
class RehearsalSign;

class EditRehearsalSign : public QUndoCommand
{
public:
    EditRehearsalSign(RehearsalSign& rehearsalSign, bool isShown,
                      uint8_t letter = 0, const std::string& description = "");

    virtual void undo();
    virtual void redo();

protected:
    RehearsalSign& rehearsalSign;
    bool isShown;
    quint8 letter;
    std::string description;

    void showHide(bool show);
};

#endif // EDITREHEARSALSIGN_H
