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

#ifndef BARLINEDIALOG_H
#define BARLINEDIALOG_H

#include <QDialog>
#include <boost/cstdint.hpp>

namespace Ui {
class BarlineDialog;
}

class BarlineDialog : public QDialog
{
    Q_OBJECT

public:
    explicit BarlineDialog(QWidget* parent, uint8_t barType, uint8_t repeats, bool startBar, bool endBar);
    ~BarlineDialog();

    uint8_t barlineType() const;
    uint8_t repeatCount() const;

private slots:
    void onBarlineTypeChanged(int);

private:
    Ui::BarlineDialog *ui;
};

#endif // BARLINEDIALOG_H
