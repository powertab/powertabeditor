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

#ifndef DIALOGS_GOTOBARLINEDIALOG_H
#define DIALOGS_GOTOBARLINEDIALOG_H

#include <QDialog>
#include <score/scorelocation.h>
#include <vector>

namespace Ui {
class GoToBarlineDialog;
}

class Score;

class GoToBarlineDialog : public QDialog
{
public:
    explicit GoToBarlineDialog(QWidget *parent, const Score &score);
    ~GoToBarlineDialog();

    /// Returns the location of the selected barline.
    ConstScoreLocation getLocation() const;

private:
    Ui::GoToBarlineDialog *ui;
    std::vector<ConstScoreLocation> myLocations;
};

#endif
