#ifndef REHEARSALSIGNDIALOG_H
#define REHEARSALSIGNDIALOG_H

#include <QDialog>

class RehearsalSign;
class Score;
class QComboBox;

class RehearsalSignDialog : public QDialog
{
    Q_OBJECT
public:
    RehearsalSignDialog(Score* score, QWidget *parent = 0);

    uint8_t getSelectedLetter() const;
    std::string getEnteredDescription() const;

public slots:
    void accept();

protected:
    void populateLetterChoices();
    void populateDescriptionChoices();

    uint8_t selectedLetter;
    std::string enteredDescription;
    Score* score;

    QComboBox* letterChoice;
    QComboBox* descriptionChoice;
};

#endif // REHEARSALSIGNDIALOG_H
