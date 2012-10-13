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

namespace Ui {
    class PreferencesDialog;
}

class SettingsPubSub;

/// Dialog to allow the user to modify general editor-wide settings
class PreferencesDialog : public QDialog
{
    Q_OBJECT
public:
    explicit PreferencesDialog(boost::shared_ptr<SettingsPubSub> pubsub,
                               QWidget *parent = 0);
    ~PreferencesDialog();

private:
    Ui::PreferencesDialog* ui;
    boost::shared_ptr<SettingsPubSub> pubsub;

    void loadCurrentSettings();

private slots:
    void accept();
};

#endif // PREFERENCESDIALOG_H
