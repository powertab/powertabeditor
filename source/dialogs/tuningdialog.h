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
  
#ifndef DIALOGS_TUNINGDIALOG_H
#define DIALOGS_TUNINGDIALOG_H

#include <QDialog>

class QComboBox;
class Tuning;
class TuningDictionary;

namespace Ui {
    class TuningDialog;
}

class TuningDialog : public QDialog
{
    Q_OBJECT

public:
    TuningDialog(QWidget *parent, const Tuning &currentTuning,
                 const TuningDictionary &dictionary);
    ~TuningDialog();

    Tuning getTuning() const;

private slots:
    /// Switches the note names available for each string.
    void toggleSharps(bool usesSharps);

    /// Updates which string selection boxes are enabled, based on the number
    /// of strings selected.
    void updateEnabledStrings(int numStrings);

    /// Loads all tuning presets for the specified number of strings.
    void updateTuningDictionary(int numStrings);

    /// Loads the currently-selected tuning preset.
    void loadPreset();

    /// Once a change is made to the tuning, update the preset combobox.
    void updateCurrentPreset();

private:
    Ui::TuningDialog *ui;
    const TuningDictionary &myDictionary;

    QStringList myNoteNames;
    std::vector<QComboBox *> myStringSelectors;

    void initStringSelectors(const Tuning &currentTuning);

    /// Generates a list of note names, to be used for selecting the pitch
    /// of a string.
    void generateNoteNames(bool usesSharps);
};

#endif
