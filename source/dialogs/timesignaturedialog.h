#ifndef TIMESIGNATUREDIALOG_H
#define TIMESIGNATUREDIALOG_H

#include <QDialog>
#include <powertabdocument/timesignature.h>
#include <array>

class QLineEdit;

namespace Ui {
    class TimeSignatureDialog;
}

class TimeSignatureDialog : public QDialog
{
    Q_OBJECT

public:
    explicit TimeSignatureDialog(const TimeSignature& originalTimeSignature);
    ~TimeSignatureDialog();

    TimeSignature getNewTimeSignature() const;

private slots:
    void editTimeSignatureVisible(bool isVisible);
    void editMetronomePulses(int selectedIndex);
    void editCutTime(bool enabled);
    void editCommonTime(bool enabled);
    void editBeatValue(int selectedIndex);
    void editBeatsPerMeasure(int numBeats);

private:
    Ui::TimeSignatureDialog *ui;
    std::array<QLineEdit*, 4> beamingPatterns;

    TimeSignature newTimeSignature;

    void init();
    void updatePossiblePulseValues();
};

#endif // TIMESIGNATUREDIALOG_H
