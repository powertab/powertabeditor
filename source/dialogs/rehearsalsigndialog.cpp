/*
  * Copyright (C) 2011 Cameron White
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
  
#include "rehearsalsigndialog.h"
#include "ui_rehearsalsigndialog.h"

#include <QCompleter>
#include <QMessageBox>

RehearsalSignDialog::RehearsalSignDialog(QWidget *parent)
    : QDialog(parent),
      ui(new Ui::RehearsalSignDialog)
{
    ui->setupUi(this);

    populateDescriptionChoices();
    ui->descriptionComboBox->clearEditText();
}

RehearsalSignDialog::~RehearsalSignDialog()
{
    delete ui;
}

void RehearsalSignDialog::populateDescriptionChoices()
{
    QStringList descriptions;
    descriptions << "Intro" << "Pre-Verse" << "Verse" << "Pre-Chorus"
                 << "Chorus" << "Interlude" << "Breakdown" << "Bridge"
                 << "Guitar Break" << "Guitar Solo" << "Out-Chorus" << "Outro";

    ui->descriptionComboBox->addItems(descriptions);

    // Autocompletion for description choices.
    QCompleter *completer = new QCompleter(descriptions);
    completer->setCaseSensitivity(Qt::CaseInsensitive);
    ui->descriptionComboBox->setCompleter(completer);
}

void RehearsalSignDialog::accept()
{
    std::string description = ui->descriptionComboBox->currentText().toStdString();
    
    if (description.empty())
    {
        QMessageBox(QMessageBox::Warning, tr("Rehearsal Sign"),
                    tr("The Rehearsal Sign description cannot be empty.")).exec();
    }
    else
    {
        done(QDialog::Accepted);
    }
}

std::string RehearsalSignDialog::getDescription() const
{
    return ui->descriptionComboBox->currentText().toStdString();
}
