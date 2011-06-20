#ifndef TIMESIGNATUREDIALOG_H
#define TIMESIGNATUREDIALOG_H

#include <QDialog>

#include <powertabdocument/timesignature.h>
#include <boost/array.hpp>

class QCheckBox;
class QComboBox;
class QSpinBox;
class QLineEdit;

class TimeSignatureDialog : public QDialog
{
    Q_OBJECT
public:
    TimeSignatureDialog(const TimeSignature& originalTimeSignature);

    TimeSignature getNewTimeSignature() const;

private:
    void init();
    void updatePossiblePulseValues();

    TimeSignature newTimeSignature;

    QCheckBox* visibilityToggle;
    QCheckBox* commonTimeToggle;
    QCheckBox* cutTimeToggle;
    QSpinBox* beatsPerMeasure;
    QComboBox* pulsesPerMeasure;
    QComboBox* beatValue;

    boost::array<QLineEdit*, 4> beamingPatterns;
};

#endif // TIMESIGNATUREDIALOG_H
