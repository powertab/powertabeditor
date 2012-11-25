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

#ifndef TRILLDIALOG_H
#define TRILLDIALOG_H

#include <QDialog>
#include <boost/cstdint.hpp>

namespace Ui {
class TrillDialog;
}

class Note;

class TrillDialog : public QDialog
{
    Q_OBJECT

public:
    explicit TrillDialog(QWidget* parent, const Note* note);
    ~TrillDialog();

    uint8_t getTrill() const;

public slots:
    void accept();

private:
    Ui::TrillDialog *ui;
    const Note* note;
};

#endif // TRILLDIALOG_H
