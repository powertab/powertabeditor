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

#include "lefthandfingeringdialog.h"
#include "ui_lefthandfingeringdialog.h"

#include <score/note.h>

LeftHandFingeringDialog::LeftHandFingeringDialog(QWidget *parent)
    : QDialog(parent), ui(new Ui::LeftHandFingeringDialog)
{
    ui->setupUi(this);

    connect(ui->buttonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(ui->buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);

    ui->fingerComboBox->addItems({
        tr("None (string empty)"), tr("Index finger"), tr("Middle finger"),
        tr("Ring finger"), tr("Little finger"), tr("Thumb")
    });

    ui->positionComboBox->addItems({
        tr("Left"), tr("Above and left"), tr("Above, centered"),
        tr("Above and right"), tr("Right"), tr("Below and right"),
        tr("Below, centered"), tr("Below and left")
    });
}

LeftHandFingeringDialog::~LeftHandFingeringDialog()
{
}

LeftHandFingering LeftHandFingeringDialog::getLeftHandFingering() const
{
    return LeftHandFingering(
        static_cast<LeftHandFingering::Finger>(ui->fingerComboBox->currentIndex()),
        static_cast<LeftHandFingering::DisplayPosition>(ui->positionComboBox->currentIndex())
    );
}
