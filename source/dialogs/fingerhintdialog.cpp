/*
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

#include "fingerhintdialog.h"
#include "ui_fingerhintdialog.h"

#include <score/note.h>

FingerHintDialog::FingerHintDialog(QWidget *parent)
    : QDialog(parent), ui(new Ui::FingerHintDialog)
{
    ui->setupUi(this);

    ui->fingerComboBox->addItems({
        tr("None (string empty)"), tr("Index finger"), tr("Middle finger"),
        tr("Ring finger"), tr("Little finger")
    });

    ui->positionComboBox->addItems({
        tr("Above and left"), tr("Above, centered"), tr("Above and right"),
        tr("Right"), tr("Below and right"), tr("Below, centered"),
        tr("Below and left"), tr("Left")
    });
}

FingerHintDialog::~FingerHintDialog() { delete ui; }

FingerHint FingerHintDialog::getFingerHint() const
{
    return FingerHint(
        static_cast<FingerHint::Finger>(ui->fingerComboBox->currentIndex()),
        static_cast<FingerHint::DisplayPosition>(ui->positionComboBox->currentIndex())
    );
}
