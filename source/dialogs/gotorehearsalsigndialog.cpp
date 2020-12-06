#include "gotorehearsalsigndialog.h"
#include "ui_gotorehearsalsigndialog.h"

#include <score/score.h>
#include <score/scorelocation.h>

// The ScoreLocation class cannot be used with QVariant since it doesn't
// have a default constructor. So, we use a pair of ints to store the system
// and position.
typedef std::pair<int, int> SystemAndPosition;
Q_DECLARE_METATYPE(SystemAndPosition)

GoToRehearsalSignDialog::GoToRehearsalSignDialog(QWidget *parent,
                                                 const Score &score)
    : QDialog(parent),
      ui(new Ui::GoToRehearsalSignDialog),
      myScore(score)
{
    ui->setupUi(this);

    connect(ui->buttonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(ui->buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);

    // Add all of the rehearsal signs in the score to the list.
    int systemIndex = 0;
    for (const System &system : score.getSystems())
    {
        for (const Barline &barline : system.getBarlines())
        {
            if (barline.hasRehearsalSign())
            {
                SystemAndPosition location = std::make_pair(
                            systemIndex, barline.getPosition());
                const RehearsalSign &sign = barline.getRehearsalSign();

                ui->rehearsalSignComboBox->addItem(
                    QStringLiteral("%1 -- %2")
                        .arg(QString::fromStdString(sign.getLetters()),
                             QString::fromStdString(sign.getDescription())),
                    QVariant::fromValue(location));
            }
        }

        ++systemIndex;
    }
}

GoToRehearsalSignDialog::~GoToRehearsalSignDialog()
{
    delete ui;
}

ConstScoreLocation GoToRehearsalSignDialog::getLocation() const
{
    const int index = ui->rehearsalSignComboBox->currentIndex();
    Q_ASSERT(index >= 0);

    SystemAndPosition location = ui->rehearsalSignComboBox->itemData(
                index).value<SystemAndPosition>();

    return ConstScoreLocation(myScore, location.first, 0, location.second);
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
