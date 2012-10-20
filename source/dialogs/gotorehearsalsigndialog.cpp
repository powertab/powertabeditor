#include "gotorehearsalsigndialog.h"
#include "ui_gotorehearsalsigndialog.h"

#include <powertabdocument/score.h>
#include <powertabdocument/system.h>
#include <powertabdocument/barline.h>
#include <powertabdocument/systemlocation.h>

Q_DECLARE_METATYPE(SystemLocation)

GoToRehearsalSignDialog::GoToRehearsalSignDialog(QWidget *parent,
                                                 const Score* score) :
    QDialog(parent),
    ui(new Ui::GoToRehearsalSignDialog),
    score(score)
{
    ui->setupUi(this);

    // Add all of the rehearsal signs in the score to the list.
    for (size_t i = 0; i < score->GetSystemCount(); ++i)
    {
        Score::SystemConstPtr system(score->GetSystem(i));
        std::vector<System::BarlineConstPtr> barlines;
        system->GetBarlines(barlines);

        for (size_t j = 0; j < barlines.size(); ++j)
        {
            const RehearsalSign& sign = barlines[j]->GetRehearsalSign();
            if (sign.IsSet())
            {
                const uint32_t position = barlines[j]->GetPosition();
                ui->rehearsalSignComboBox->addItem(
                            QString::fromStdString(sign.GetFormattedText()),
                            QVariant::fromValue(SystemLocation(i, position)));
            }
        }
    }
}

GoToRehearsalSignDialog::~GoToRehearsalSignDialog()
{
    delete ui;
}

/// Returns the location of the selected rehearsal sign.
SystemLocation GoToRehearsalSignDialog::getLocation() const
{
    const int index = ui->rehearsalSignComboBox->currentIndex();
    Q_ASSERT(index >= 0);

    return ui->rehearsalSignComboBox->itemData(index).value<SystemLocation>();
}

void GoToRehearsalSignDialog::accept()
{
    const int index = ui->rehearsalSignComboBox->currentIndex();
    if (index >= 0)
    {
        done(QDialog::Accepted);
    }
    else
    {
        done(QDialog::Rejected);
    }
}
