#include "timesignaturedialog.h"

#include <QFormLayout>
#include <QDialogButtonBox>
#include <QCheckBox>
#include <QSpinBox>
#include <QComboBox>
#include <QLineEdit>

#include <boost/array.hpp>
#include <boost/foreach.hpp>

TimeSignatureDialog::TimeSignatureDialog(const TimeSignature& originalTimeSignature) :
    newTimeSignature(originalTimeSignature)
{
    setWindowTitle(tr("Time Signature"));
    setModal(true);

    QVBoxLayout* mainLayout = new QVBoxLayout;
    QFormLayout* formLayout = new QFormLayout;

    visibilityToggle = new QCheckBox;
    formLayout->addRow(tr("Show Time Signature:"), visibilityToggle);

    commonTimeToggle = new QCheckBox;
    formLayout->addRow(tr("Common Time:"), commonTimeToggle);

    cutTimeToggle = new QCheckBox;
    formLayout->addRow(tr("Cut Time:"), cutTimeToggle);

    beatsPerMeasure = new QSpinBox;
    formLayout->addRow(tr("Beats Per Measure:"), beatsPerMeasure);

    beatValue = new QComboBox;
    formLayout->addRow(tr("Beat Value:"), beatValue);

    pulsesPerMeasure = new QComboBox;
    formLayout->addRow(tr("Metronome Pulses per Measure:"), pulsesPerMeasure);

    QHBoxLayout* beamingPatternLayout = new QHBoxLayout;
    BOOST_FOREACH(QLineEdit*& pattern, beamingPatterns)
    {
        pattern = new QLineEdit;
        beamingPatternLayout->addWidget(pattern);
    }

    formLayout->addRow(tr("Beaming Pattern:"), beamingPatternLayout);

    mainLayout->addLayout(formLayout);

    QDialogButtonBox* buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
    connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
    mainLayout->addWidget(buttonBox);

    setLayout(mainLayout);

    init();
}

/// Initialize with default values, set up signal/slot connections, etc
void TimeSignatureDialog::init()
{
    beatsPerMeasure->setMinimum(TimeSignature::MIN_BEATSPERMEASURE);
    beatsPerMeasure->setMaximum(TimeSignature::MAX_BEATSPERMEASURE);

    boost::array<int, 5> beatValues = {{2, 4, 8, 16, 32}};
    BOOST_FOREACH(int beat, beatValues)
    {
        beatValue->addItem(QString::number(beat), beat);
    }

    BOOST_FOREACH(QLineEdit* pattern, beamingPatterns)
    {
        pattern->setValidator(new QIntValidator(0, TimeSignature::MAX_BEATAMOUNT, 0));
     }

    visibilityToggle->setChecked(newTimeSignature.IsShown());
    commonTimeToggle->setChecked(newTimeSignature.IsCommonTime());
    cutTimeToggle->setChecked(newTimeSignature.IsCutTime());

    beatsPerMeasure->setValue(newTimeSignature.GetBeatsPerMeasure());
    beatValue->setCurrentIndex(beatValue->findData(newTimeSignature.GetBeatAmount()));

    updatePossiblePulseValues();
    pulsesPerMeasure->setCurrentIndex(pulsesPerMeasure->findData(newTimeSignature.GetPulses()));

    boost::array<uint8_t, 4> beamingPatternValues = {{0, 0, 0, 0}};
    newTimeSignature.GetBeamingPattern(beamingPatternValues[0], beamingPatternValues[1],
                                       beamingPatternValues[2], beamingPatternValues[3]);
    for (size_t i = 0; i < beamingPatterns.size(); i++)
    {
        beamingPatterns[i]->setText(QString::number(beamingPatternValues[i]));
    }
}

/// The available options for metronome pulses depend on the number of beats in the measure
void TimeSignatureDialog::updatePossiblePulseValues()
{
    for (uint8_t i = TimeSignature::MIN_PULSES; i < TimeSignature::MAX_PULSES; i++)
    {
        if (newTimeSignature.IsValidPulses(i))
        {
            pulsesPerMeasure->addItem(QString::number(i), i);
        }
    }
}

TimeSignature TimeSignatureDialog::getNewTimeSignature() const
{
    return newTimeSignature;
}
