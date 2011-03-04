#include "trilldialog.h"

#include <powertabdocument/note.h>

#include <QFormLayout>
#include <QSpinBox>
#include <QDialogButtonBox>
#include <QMessageBox>

TrillDialog::TrillDialog(Note* note, quint8& trillFret) :
    note(note),
    trillFret(trillFret)
{
    setWindowTitle(tr("Trill"));
    setModal(true);

    QFormLayout* formLayout = new QFormLayout;

    QSpinBox* mainNote = new QSpinBox;
    mainNote->setValue(note->GetFretNumber());
    mainNote->setDisabled(true);

    trillFretSelector = new QSpinBox;
    trillFretSelector->setValue(0);
    trillFretSelector->setMinimum(Note::MIN_FRET_NUMBER);
    trillFretSelector->setMaximum(Note::MAX_FRET_NUMBER);

    formLayout->addRow(tr("Current Fret:"), mainNote);
    formLayout->addRow(tr("Trill With Fret:"), trillFretSelector);

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

void TrillDialog::accept()
{
    const int selectedFret = trillFretSelector->value();

    if (!note->IsValidTrill(selectedFret))
    {
        QMessageBox msgBox;
        msgBox.setIcon(QMessageBox::Warning);
        msgBox.setWindowTitle(tr("Trill"));
        msgBox.setText(tr("The trilled fret number cannot be the same as the original fret number."));
        msgBox.exec();
        trillFretSelector->setFocus();
    }
    else
    {
        trillFret = selectedFret;
        done(Accepted);
    }
}

void TrillDialog::reject()
{
    done(Rejected);
}
