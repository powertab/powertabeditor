#ifndef BARLINEDIALOG_H
#define BARLINEDIALOG_H

#include <QDialog>

class QSpinBox;
class QComboBox;

// Dialog to allow the user to modify a barline

class BarlineDialog : public QDialog
{
    Q_OBJECT
public:
    BarlineDialog(quint8& barType, quint8& repeats);

protected:
    quint8& barType;
    quint8& repeats;

    QSpinBox* repeatCount;
    QComboBox* barLineType;

    void init();

private slots:
    void disableRepeatCount(int newBarlineType);
    void accept();
    void reject();
};

#endif // BARLINEDIALOG_H
