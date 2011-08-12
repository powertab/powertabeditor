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
