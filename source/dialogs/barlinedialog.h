#ifndef BARLINEDIALOG_H
#define BARLINEDIALOG_H

#include <QDialog>

class QSpinBox;
class QComboBox;
class Barline;

// Dialog to allow the user to modify a barline

class BarlineDialog : public QDialog
{
    Q_OBJECT
public:
    explicit BarlineDialog(Barline* bar, QWidget *parent = 0);

private:
    Barline* barLine;
    QSpinBox* repeatCount;
    QComboBox* barLineType;

    void init();

private slots:
    void disableRepeatCount(int newBarlineType);
    void accept();
    void reject();
};

#endif // BARLINEDIALOG_H
