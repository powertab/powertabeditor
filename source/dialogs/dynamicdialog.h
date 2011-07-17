#ifndef DYNAMICDIALOG_H
#define DYNAMICDIALOG_H

#include <QDialog>

class QButtonGroup;

namespace Ui {
    class DynamicDialog;
}

class DynamicDialog : public QDialog
{
    Q_OBJECT

public:
    explicit DynamicDialog(QWidget *parent = 0);
    ~DynamicDialog();

    uint8_t selectedVolumeLevel() const;

private slots:
    void updateSelectedVolumeLevel(int level);

private:
    Ui::DynamicDialog *ui;

    QButtonGroup* volumeLevels;
    uint8_t volumeLevel;
};

#endif // DYNAMICDIALOG_H
