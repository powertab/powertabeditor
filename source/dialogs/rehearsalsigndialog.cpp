#include "rehearsalsigndialog.h"

#include <powertabdocument/rehearsalsign.h>
#include <powertabdocument/score.h>

#include <powertabeditor.h>

#include <actions/editrehearsalsign.h>

#include <QFormLayout>
#include <QComboBox>
#include <QCompleter>
#include <QDialogButtonBox>

RehearsalSignDialog::RehearsalSignDialog(Score* score, System* system, RehearsalSign* rehearsalSign, QWidget *parent) :
    QDialog(parent),
    rehearsalSign(rehearsalSign),
    score(score),
    system(system)
{
    setWindowTitle(tr("Rehearsal Sign"));
    setModal(true);

    letterChoice = new QComboBox;
    populateLetterChoices();

    descriptionChoice = new QComboBox;
    descriptionChoice->setEditable(true);
    populateDescriptionChoices();
    descriptionChoice->clearEditText();

    QFormLayout* formLayout = new QFormLayout;
    formLayout->addRow(tr("Letter:"), letterChoice);
    formLayout->addRow(tr("Description:"), descriptionChoice);

    QDialogButtonBox* buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);

    QVBoxLayout *buttonsLayout = new QVBoxLayout;
    buttonsLayout->addStretch(1);
    buttonsLayout->addLayout(formLayout);
    buttonsLayout->addWidget(buttonBox);

    buttonsLayout->setSizeConstraint(QLayout::SetFixedSize);
    setLayout(buttonsLayout);
    connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
    connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
}

// populate the list of available letters
void RehearsalSignDialog::populateLetterChoices()
{
    QMap<quint8, quint8> lettersInUse;

    // find the letters that are already in use, by checking every rehearsal sign in the score
    for(size_t i = 0; i < score->GetSystemCount(); i++)
    {
        System* system = score->GetSystem(i);

        std::vector<Barline*> barlines;
        system->GetBarlines(barlines);

        foreach(Barline* barline, barlines)
        {
            const RehearsalSign& currentSign = barline->GetRehearsalSignConstRef();
            if (currentSign.IsSet())
            {
                lettersInUse.insert(currentSign.GetLetter(), 0); // insert the rehearsal sign's letter as a key in the map
            }
        }
    }

    // add all letters that are not in use to the drop-down list
    for (quint8 currentLetter = 'A'; currentLetter <= 'Z'; currentLetter++)
    {
        if (!lettersInUse.contains(currentLetter))
        {
            letterChoice->addItem(QChar(currentLetter));
        }
    }
}

void RehearsalSignDialog::populateDescriptionChoices()
{
    QStringList descriptions;

    descriptions << "Intro" << "Pre-Verse" << "Verse" << "Pre-Chorus" << "Chorus";
    descriptions << "Interlude" << "Breakdown" << "Bridge" << "Guitar Break" << "Guitar Solo";
    descriptions << "Out-Chorus" << "Outro";

    descriptionChoice->addItems(descriptions);

    // Autocompletion for description choices
    QCompleter *completer = new QCompleter(descriptions);
    completer->setCaseSensitivity(Qt::CaseInsensitive);
    descriptionChoice->setCompleter(completer);
}

void RehearsalSignDialog::accept()
{
    QChar letter = letterChoice->currentText().at(0); // should only be one letter
    std::string description = descriptionChoice->currentText().toStdString();

    PowerTabEditor::undoManager->push(new EditRehearsalSign(score, system, rehearsalSign, true, letter.toAscii(), description));
    done(QDialog::Accepted);
}

void RehearsalSignDialog::reject()
{
    done(QDialog::Rejected);
}
