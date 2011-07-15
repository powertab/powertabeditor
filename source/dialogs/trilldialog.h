#ifndef TRILLDIALOG_H
#define TRILLDIALOG_H

#include <QDialog>

class Note;
class QSpinBox;

class TrillDialog : public QDialog
{
    Q_OBJECT
public:
    TrillDialog(Note* note, quint8& trillFret);

public slots:
    void accept();

protected:
    Note* note;
    quint8& trillFret;

    QSpinBox* trillFretSelector;
};

#endif // TRILLDIALOG_H
