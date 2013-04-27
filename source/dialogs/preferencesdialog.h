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
  
#ifndef PREFERENCESDIALOG_H
#define PREFERENCESDIALOG_H

#include <QDialog>
#include <boost/shared_ptr.hpp>
#include <powertabdocument/tuning.h>

namespace Ui {
    class PreferencesDialog;
}

class SettingsPubSub;
class TuningDictionary;

/// Dialog to allow the user to modify general editor-wide settings
class PreferencesDialog : public QDialog
{
    Q_OBJECT
public:
    explicit PreferencesDialog(QWidget* parent,
                               boost::shared_ptr<SettingsPubSub> pubsub,
                               boost::shared_ptr<TuningDictionary> tuningDictionary);
    ~PreferencesDialog();

private:
    Ui::PreferencesDialog* ui;
    boost::shared_ptr<SettingsPubSub> pubsub;
    boost::shared_ptr<TuningDictionary> tuningDictionary;
    Tuning defaultInstrumentTuning;

    void loadCurrentSettings();

private slots:
    void accept();
    void editTuning();
};

#endif // PREFERENCESDIALOG_H
