/*
  * Copyright (C) 2021 Cameron White
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

#ifndef DIALOGS_TREMOLOBARDIALOG_H
#define DIALOGS_TREMOLOBARDIALOG_H

#include <QDialog>
#include <score/position.h>

class QComboBox;

namespace Ui
{
class TremoloBarDialog;
}

class TremoloBarDialog final : public QDialog
{
    Q_OBJECT

public:
    TremoloBarDialog(QWidget *parent, const TremoloBar *current_trem);
    ~TremoloBarDialog();

    TremoloBar getTremoloBar() const;

private:
    void initPitches();
    void handleTypeChanged();

    Ui::TremoloBarDialog *ui;
};

#endif
