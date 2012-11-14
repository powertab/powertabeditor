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

#ifndef GOTOBARLINEDIALOG_H
#define GOTOBARLINEDIALOG_H

#include <QDialog>

#include <vector>
#include <powertabdocument/systemlocation.h>

namespace Ui {
class GoToBarlineDialog;
}

class Score;
class SystemLocation;

class GoToBarlineDialog : public QDialog
{
    Q_OBJECT
public:
    explicit GoToBarlineDialog(QWidget *parent, const Score* score);
    ~GoToBarlineDialog();

    SystemLocation getLocation() const;

private:
    Ui::GoToBarlineDialog *ui;

    const Score* score;
    std::vector<SystemLocation> barlineLocations;
};

#endif // GOTOBARLINEDIALOG_H
