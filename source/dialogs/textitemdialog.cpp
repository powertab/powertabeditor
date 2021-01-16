/*
  * Copyright (C) 2014 Cameron White
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

#include "textitemdialog.h"
#include "ui_textitemdialog.h"

#include <QMessageBox>
#include <score/textitem.h>

TextItemDialog::TextItemDialog(QWidget *parent, const TextItem *current_item)
    : QDialog(parent), ui(new Ui::TextItemDialog)
{
    ui->setupUi(this);

    connect(ui->buttonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(ui->buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);

    if (current_item)
    {
        ui->contentsTextEdit->setPlainText(
            QString::fromStdString(current_item->getContents()));
    }
}

TextItemDialog::~TextItemDialog()
{
    delete ui;
}

std::string TextItemDialog::getContents() const
{
    return ui->contentsTextEdit->toPlainText().toStdString();
}

void TextItemDialog::accept()
{
    if (ui->contentsTextEdit->toPlainText().isEmpty())
    {
        QMessageBox msgBox(this);
        msgBox.setIcon(QMessageBox::Warning);
        msgBox.setWindowTitle(tr("Text"));
        msgBox.setText(tr("The text cannot be empty."));
        msgBox.exec();
    }
    else
        done(Accepted);
}
