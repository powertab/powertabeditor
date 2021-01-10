/*
  * Copyright (C) 2012 Cameron White
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

#ifndef DIALOGS_TEMPOMARKERDIALOG_H
#define DIALOGS_TEMPOMARKERDIALOG_H

#include <QDialog>
#include <score/tempomarker.h>

class QButtonGroup;

namespace Ui {
class TempoMarkerDialog;
}

class TempoMarkerDialog : public QDialog
{
    Q_OBJECT

public:
    explicit TempoMarkerDialog(QWidget *parent, const TempoMarker *marker);
    ~TempoMarkerDialog();

    TempoMarker getTempoMarker() const;

private slots:
    /// Disables the BPM spinner if listesso is enabled.
    void onListessoChanged(bool enabled);
    /// Disable the beat types, BPM spinner, and listesso beat types if the
    /// metronome marker will be hidden.
    void onShowMetronomeMarkerChanged(bool enabled);

private:
    Ui::TempoMarkerDialog *ui;
    QButtonGroup *myBeatTypes;
    QButtonGroup *myListessoBeatTypes;
    QButtonGroup *myTripletFeelTypes;
};

#endif
