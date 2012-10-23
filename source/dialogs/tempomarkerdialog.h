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

private slots:
    void onListessoChanged(bool enabled);
    void onShowMetronomeMarkerChanged(bool enabled);

private:
    Ui::TempoMarkerDialog *ui;
    QButtonGroup* beatTypes;
    QButtonGroup* listessoBeatTypes;
    QButtonGroup* tripletFeelTypes;
};

#endif // TEMPOMARKERDIALOG_H
