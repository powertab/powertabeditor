#ifndef GOTOREHEARSALSIGNDIALOG_H
#define GOTOREHEARSALSIGNDIALOG_H

#include <QDialog>

namespace Ui {
class GoToRehearsalSignDialog;
}

class Score;
class SystemLocation;

class GoToRehearsalSignDialog : public QDialog
{
    Q_OBJECT
public:
    explicit GoToRehearsalSignDialog(QWidget *parent, const Score* score);
    ~GoToRehearsalSignDialog();

    SystemLocation getLocation() const;

public slots:
    void accept();

private:
    Ui::GoToRehearsalSignDialog *ui;

    const Score* score;
};

#endif // GOTOREHEARSALSIGNDIALOG_H
