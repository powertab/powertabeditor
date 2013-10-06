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

#ifndef DIALOGS_ARTIFICIALHARMONICDIALOG_H
#define DIALOGS_ARTIFICIALHARMONICDIALOG_H

#include <QDialog>
#include <score/note.h>

namespace Ui
{
class ArtificialHarmonicDialog;
}

class QAbstractButton;
class QButtonGroup;

class ArtificialHarmonicDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ArtificialHarmonicDialog(QWidget *parent);
    ~ArtificialHarmonicDialog();

    ArtificialHarmonic getHarmonic() const;

private slots:
    void onAccidentalButtonClicked(QAbstractButton *);

private:
    Ui::ArtificialHarmonicDialog *ui;

    QButtonGroup *myKeyGroup;
    QButtonGroup *myAccidentalGroup;
    QButtonGroup *myOctaveGroup;
};

#endif
