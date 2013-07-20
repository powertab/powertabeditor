#ifndef DIALOGS_GOTOREHEARSALSIGNDIALOG_H
#define DIALOGS_GOTOREHEARSALSIGNDIALOG_H

#include <QDialog>

namespace Ui {
class GoToRehearsalSignDialog;
}

class Score;
class ScoreLocation;

class GoToRehearsalSignDialog : public QDialog
{
    Q_OBJECT

public:
    explicit GoToRehearsalSignDialog(QWidget *parent, const Score &score);
    ~GoToRehearsalSignDialog();

    /// Returns the location of the selected rehearsal sign.
    ScoreLocation getLocation() const;

public slots:
    void accept();

private:
    Ui::GoToRehearsalSignDialog *ui;
    const Score &myScore;
};

#endif
