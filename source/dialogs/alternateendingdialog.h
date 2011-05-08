#ifndef ALTERNATEENDINGDIALOG_H
#define ALTERNATEENDINGDIALOG_H

#include <QDialog>
#include <memory>
#include <vector>

class AlternateEnding;
class QCheckBox;

class AlternateEndingDialog : public QDialog
{
    Q_OBJECT
public:
    AlternateEndingDialog(std::shared_ptr<AlternateEnding> altEnding);

public slots:
    void accept();
    void reject();

private:
    std::shared_ptr<AlternateEnding> altEnding;
    std::vector<QCheckBox*> checkBoxes;
};

#endif // ALTERNATEENDINGDIALOG_H
