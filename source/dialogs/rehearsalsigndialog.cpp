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

RehearsalSignDialog::RehearsalSignDialog(Score* score, QWidget *parent) :
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
        std::shared_ptr<System> system = score->GetSystem(i);

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
