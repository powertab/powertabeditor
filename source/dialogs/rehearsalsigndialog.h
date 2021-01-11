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
  
#ifndef DIALOGS_REHEARSALSIGNDIALOG_H
#define DIALOGS_REHEARSALSIGNDIALOG_H

#include <QDialog>

namespace Ui {
    class RehearsalSignDialog;
}

class RehearsalSignDialog : public QDialog
{
    Q_OBJECT

public:
    RehearsalSignDialog(QWidget *parent, const std::string &description);
    ~RehearsalSignDialog();

    /// Returns the description of the rehearsal sign that was entered by
    /// the user.
    std::string getDescription() const;

public slots:
    virtual void accept() override;

private:
    void populateDescriptionChoices();

    Ui::RehearsalSignDialog* ui;
};

#endif
