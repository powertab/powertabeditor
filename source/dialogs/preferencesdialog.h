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
  
#ifndef DIALOGS_PREFERENCESDIALOG_H
#define DIALOGS_PREFERENCESDIALOG_H

#include <QDialog>
#include <score/tuning.h>

namespace Ui {
    class PreferencesDialog;
}

class SettingsManager;
class TuningDictionary;

/// Dialog that allows the user to modify general editor-wide settings.
class PreferencesDialog : public QDialog
{
    Q_OBJECT

public:
    explicit PreferencesDialog(QWidget *parent,
                               SettingsManager &settings_manager,
                               const TuningDictionary &dictionary);
    ~PreferencesDialog();

private slots:
    virtual void accept() override;
    void editTuning();

private:
    /// Load the current preferences and initialize the widgets with those
    /// values.
    void loadCurrentSettings();

    Ui::PreferencesDialog *ui;
    SettingsManager &mySettingsManager;
    const TuningDictionary &myDictionary;
    Tuning myDefaultTuning;
};

#endif
