/*
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

#ifndef DIALOGS_LEFTHANDFINGERINGDIALOG_H
#define DIALOGS_LEFTHANDFINGERINGDIALOG_H

#include <QDialog>
#include <memory>
#include <score/note.h>

class QComboBox;

namespace Ui
{
class LeftHandFingeringDialog;
}

class LeftHandFingeringDialog : public QDialog
{
    Q_OBJECT

public:
    LeftHandFingeringDialog(QWidget *parent);
    ~LeftHandFingeringDialog();

    LeftHandFingering getLeftHandFingering() const;

private:
    std::unique_ptr<Ui::LeftHandFingeringDialog> ui;
};

#endif
