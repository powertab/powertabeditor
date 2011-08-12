#ifndef REHEARSALSIGNDIALOG_H
#define REHEARSALSIGNDIALOG_H

#include <QDialog>

class Score;

namespace Ui {
    class RehearsalSignDialog;
}

class RehearsalSignDialog : public QDialog
{
    Q_OBJECT
public:
    RehearsalSignDialog(Score* score, QWidget *parent = 0);
    ~RehearsalSignDialog();

    uint8_t getSelectedLetter() const;
    std::string getEnteredDescription() const;

public slots:
    void accept();

private:
    Ui::RehearsalSignDialog* ui;

    void populateLetterChoices();
    void populateDescriptionChoices();

    uint8_t selectedLetter;
    std::string enteredDescription;
    Score* score;
};

#endif // REHEARSALSIGNDIALOG_H
