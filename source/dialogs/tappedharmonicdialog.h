#ifndef TAPPEDHARMONICDIALOG_H
#define TAPPEDHARMONICDIALOG_H

#include <QDialog>

class Note;
class QComboBox;

class TappedHarmonicDialog : public QDialog
{
    Q_OBJECT
public:
    TappedHarmonicDialog(const Note* note, uint8_t& tappedFret);

public slots:
    void accept();

private:
    void initTappedFrets();

    const Note* note;
    uint8_t& tappedFret;

    QComboBox* tappedFretSelector;
};

#endif // TAPPEDHARMONICDIALOG_H
