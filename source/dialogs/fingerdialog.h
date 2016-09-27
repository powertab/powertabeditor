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

#ifndef DIALOGS_FINGERDIALOG_H
#define DIALOGS_FINGERDIALOG_H

#include <QDialog>

namespace Ui {
    class FingerDialog;
}

class FingerDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit FingerDialog(QWidget *parent, int finger);
    ~FingerDialog();
    
    int getFinger() const;
    
    public slots:
    virtual void accept() override;
    
private:
    Ui::FingerDialog *ui;
    const int myFinger;
};

#endif
