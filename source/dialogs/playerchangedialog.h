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

#ifndef DIALOGS_PLAYERCHANGEDIALOG_H
#define DIALOGS_PLAYERCHANGEDIALOG_H

#include <QDialog>

namespace Ui {
class PlayerChangeDialog;
}

class PlayerChange;
class QComboBox;
class Score;
class System;

class PlayerChangeDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit PlayerChangeDialog(QWidget *parent, const Score &score,
                                const System &system,
                                const PlayerChange *currentPlayers);
    ~PlayerChangeDialog();

    PlayerChange getPlayerChange() const;
    
private:
    QComboBox *getStaffComboBox(int numStrings, const System &system);
    QComboBox *getInstrumentComboBox(const Score &score);

    Ui::PlayerChangeDialog *ui;
    std::vector<QComboBox *> myStaffComboBoxes;
    std::vector<QComboBox *> myInstrumentComboBoxes;
};

#endif
