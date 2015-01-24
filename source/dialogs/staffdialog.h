/*
  * Copyright (C) 2013 Cameron White
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

#ifndef DIALOGS_STAFFDIALOG_H
#define DIALOGS_STAFFDIALOG_H

#include <QDialog>
#include <score/staff.h>

namespace Ui {
class StaffDialog;
}

class StaffDialog : public QDialog
{
public:
    explicit StaffDialog(QWidget *parent);
    ~StaffDialog();

    int getStringCount() const;
    void setStringCount(int strings);

    Staff::ClefType getClefType() const;
    void setClefType(Staff::ClefType type);

private:
    Ui::StaffDialog *ui;
};

#endif
