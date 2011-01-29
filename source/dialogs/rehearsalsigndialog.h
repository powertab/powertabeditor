#ifndef REHEARSALSIGNDIALOG_H
#define REHEARSALSIGNDIALOG_H

#include <QDialog>

class RehearsalSign;
class Score;
class QComboBox;
class System;

class RehearsalSignDialog : public QDialog
{
    Q_OBJECT
public:
    RehearsalSignDialog(Score* score, System* system, RehearsalSign* rehearsalSign, QWidget *parent = 0);

public slots:
    void accept();
    void reject();

protected:
    void populateLetterChoices();
    void populateDescriptionChoices();

    RehearsalSign* rehearsalSign;
    Score* score;
    System* system;
    QComboBox* letterChoice;
    QComboBox* descriptionChoice;
};

#endif // REHEARSALSIGNDIALOG_H
