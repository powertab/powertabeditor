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
  
#ifndef DIALOGS_ALTERNATEENDINGDIALOG_H
#define DIALOGS_ALTERNATEENDINGDIALOG_H

#include <QDialog>
#include <score/alternateending.h>
#include <vector>

class QCheckBox;

class AlternateEndingDialog : public QDialog
{
    Q_OBJECT

public:
    AlternateEndingDialog(QWidget *parent,
                          const AlternateEnding *current_ending);

    AlternateEnding getAlternateEnding() const;

private slots:
    virtual void accept() override;

private:
    std::vector<QCheckBox *> myCheckBoxes;
    QCheckBox *myDaCapoCheckbox;
    QCheckBox *myDalSegnoCheckbox;
    QCheckBox *myDalSegnoSegnoCheckbox;
};

#endif
