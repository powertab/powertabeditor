#ifndef TEMPOMARKERDIALOG_H
#define TEMPOMARKERDIALOG_H

#include <QDialog>

class QButtonGroup;

namespace Ui {
class TempoMarkerDialog;
}

class TempoMarkerDialog : public QDialog
{
    Q_OBJECT

public:
    explicit TempoMarkerDialog(QWidget* parent);
    ~TempoMarkerDialog();

private:
    Ui::TempoMarkerDialog *ui;
    QButtonGroup* beatTypes;
};

#endif // TEMPOMARKERDIALOG_H
