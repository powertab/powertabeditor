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

#include <powertabdocument/rehearsalsign.h>
#include <powertabdocument/score.h>
#include <powertabdocument/system.h>
#include <powertabdocument/barline.h>

#include <boost/foreach.hpp>
#include <set>

#include <QCompleter>
#include <QMessageBox>

RehearsalSignDialog::RehearsalSignDialog(QWidget *parent, Score *score) :
    QDialog(parent),
    ui(new Ui::RehearsalSignDialog),
    score(score)
{
    ui->setupUi(this);

    populateLetterChoices();
    populateDescriptionChoices();
    ui->descriptionComboBox->clearEditText();
}

RehearsalSignDialog::~RehearsalSignDialog()
{
    delete ui;
}

// populate the list of available letters
void RehearsalSignDialog::populateLetterChoices()
{
    std::set<uint8_t> lettersInUse;

    // find the letters that are already in use, by checking every rehearsal sign in the score
    for(size_t i = 0; i < score->GetSystemCount(); i++)
    {
        boost::shared_ptr<System> system = score->GetSystem(i);

        std::vector<System::BarlineConstPtr> barlines;
        system->GetBarlines(barlines);

        BOOST_FOREACH(System::BarlineConstPtr barline, barlines)
        {
            const RehearsalSign& currentSign = barline->GetRehearsalSign();
            if (currentSign.IsSet())
            {
                lettersInUse.insert(currentSign.GetLetter());
            }
        }
    }

    // add all letters that are not in use to the drop-down list
    for (quint8 currentLetter = 'A'; currentLetter <= 'Z'; currentLetter++)
    {
        if (lettersInUse.find(currentLetter) == lettersInUse.end())
        {
            ui->letterComboBox->addItem(QChar(currentLetter));
        }
    }
}

void RehearsalSignDialog::populateDescriptionChoices()
{
    QStringList descriptions;

    descriptions << "Intro" << "Pre-Verse" << "Verse" << "Pre-Chorus" << "Chorus";
    descriptions << "Interlude" << "Breakdown" << "Bridge" << "Guitar Break" << "Guitar Solo";
    descriptions << "Out-Chorus" << "Outro";

    ui->descriptionComboBox->addItems(descriptions);

    // Autocompletion for description choices
    QCompleter *completer = new QCompleter(descriptions);
    completer->setCaseSensitivity(Qt::CaseInsensitive);
    ui->descriptionComboBox->setCompleter(completer);
}

void RehearsalSignDialog::accept()
{
    selectedLetter = ui->letterComboBox->currentText().at(0).toAscii(); // should only be one letter
    enteredDescription = ui->descriptionComboBox->currentText().toStdString();
    
    if (enteredDescription.empty())
    {
        QMessageBox(QMessageBox::Warning, tr("Rehearsal Sign"),
                    tr("The Rehearsal Sign description cannot be empty.")).exec();
    }
    else
    {
        done(QDialog::Accepted);
    }
}

/// Returns the letter that was selected by the user
uint8_t RehearsalSignDialog::getSelectedLetter() const
{
    return selectedLetter;
}

/// Returns the description of the rehearsal sign that was entered by the user
std::string RehearsalSignDialog::getEnteredDescription() const
{
    return enteredDescription;
}
