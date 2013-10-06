/*
  * Copyright (C) 2011 Cameron White
  *
  * This program is free software: you can redistribute it and/or modify
  * it under the terms of the GNU General Public License as published by
  * the Free Software Foundation, either version 3 of the License, or
  * (at your option) any later version.
  *
  * This program is distributed in the hope that it will be useful,
  * but WITHOUT ANY WARRANTY; without even the implied warranty of
  * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  * GNU General Public License for more details.
  *
  * You should have received a copy of the GNU General Public License
  * along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
  
#ifndef DIALOGS_TIMESIGNATUREDIALOG_H
#define DIALOGS_TIMESIGNATUREDIALOG_H

#include <QDialog>
#include <score/timesignature.h>

class QLineEdit;

namespace Ui {
    class TimeSignatureDialog;
}

class TimeSignatureDialog : public QDialog
{
    Q_OBJECT

public:
    explicit TimeSignatureDialog(QWidget *parent,
                                 const TimeSignature &currentTimeSignature);
    ~TimeSignatureDialog();

    /// Returns a new time signature using the values selected in the dialog.
    TimeSignature getTimeSignature() const;

private slots:
    virtual void accept() override;
    void editTimeSignatureVisible(bool isVisible);
    void editMetronomePulses(int selectedIndex);
    void editCutTime(bool enabled);
    void editCommonTime(bool enabled);
    void editBeatValue(int selectedIndex);
    void editBeatsPerMeasure(int beats);

private:
    /// Update the options in the metronome pulses combo box (the available
    /// options for metronome pulses depend on the number of beats).
    void updatePossiblePulseValues();

    Ui::TimeSignatureDialog *ui;
    std::array<QLineEdit *, 4> myBeamingPatterns;
    TimeSignature myTimeSignature;
};

#endif
