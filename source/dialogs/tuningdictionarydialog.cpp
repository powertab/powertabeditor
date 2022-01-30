/*
  * Copyright (C) 2012 Cameron White
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

#include "tuningdictionarydialog.h"
#include "ui_tuningdictionarydialog.h"

#include <app/tuningdictionary.h>
#include <dialogs/tuningdialog.h>
#include <util/tostring.h>

Q_DECLARE_METATYPE(TuningDictionary::Entry *)

TuningDictionaryDialog::TuningDictionaryDialog(QWidget *parent,
                                               TuningDictionary &dictionary)
    : QDialog(parent),
      ui(new Ui::TuningDictionaryDialog),
      myDictionary(dictionary)
{
    ui->setupUi(this);

    connect(ui->buttonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(ui->buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);

    ui->tuningsList->setColumnCount(2);
    ui->tuningsList->setHeaderLabels(
        { tr("Name"), tr("Tuning (Low to High)") });
    ui->tuningsList->header()->sectionResizeMode(QHeaderView::ResizeToContents);

    for (int i = Tuning::MIN_STRING_COUNT; i <= Tuning::MAX_STRING_COUNT; ++i)
        ui->stringsComboBox->addItem(QString::number(i), i);

    connect(ui->stringsComboBox,
            qOverload<int>(&QComboBox::currentIndexChanged), this,
            &TuningDictionaryDialog::onNumStringsChanged);
    connect(ui->tuningsList, &QTreeWidget::currentItemChanged, this,
            &TuningDictionaryDialog::onCurrentTuningChanged);
    connect(ui->newTuningButton, &QPushButton::clicked, this,
            &TuningDictionaryDialog::onNewTuning);
    connect(ui->editTuningButton, &QPushButton::clicked, this,
            &TuningDictionaryDialog::onEditTuning);
    connect(ui->deleteTuningButton, &QPushButton::clicked, this,
            &TuningDictionaryDialog::onDeleteTuning);

    // Select the "6 Strings" option by default.
    ui->stringsComboBox->setCurrentIndex(3);
}

TuningDictionaryDialog::~TuningDictionaryDialog()
{
    delete ui;
}

void TuningDictionaryDialog::onNumStringsChanged(int index)
{
    const int numStrings = ui->stringsComboBox->itemData(index).toInt();

    ui->tuningsList->clear();

    std::vector<TuningDictionary::Entry *> entries;
    myDictionary.findTunings(numStrings, entries);

    for (TuningDictionary::Entry *entry : entries)
    {
        const Tuning &tuning = entry->myTuning;
        auto item = new QTreeWidgetItem(
            QStringList({ QString::fromStdString(tuning.getName()),
                          QString::fromStdString(Util::toString(tuning)) }));

        item->setData(0, Qt::UserRole, QVariant::fromValue(entry));
        ui->tuningsList->addTopLevelItem(item);
    }
}

void TuningDictionaryDialog::onNewTuning()
{
    Tuning tuning;
    TuningDialog dialog(this, tuning, myDictionary);

    if (dialog.exec() == QDialog::Accepted)
    {
        myDictionary.addTuning(dialog.getTuning());
        onTuningModified();
    }
}

void TuningDictionaryDialog::onDeleteTuning()
{
    myDictionary.removeTuning(selectedTuning()->myTuning);
    onTuningModified();
}

void TuningDictionaryDialog::onCurrentTuningChanged(QTreeWidgetItem *current,
                                                    QTreeWidgetItem *)
{
    // Enable if there is a selected tuning and it's writeable.
    bool enable = false;
    if (current != nullptr)
    {
        auto entry =
            current->data(0, Qt::UserRole).value<TuningDictionary::Entry *>();
        enable = entry->myWriteable;
    }

    ui->deleteTuningButton->setEnabled(enable);
    ui->editTuningButton->setEnabled(enable);
}

void TuningDictionaryDialog::onEditTuning()
{
    Tuning &tuning = selectedTuning()->myTuning;
    TuningDialog dialog(this, tuning, myDictionary);

    if (dialog.exec() == QDialog::Accepted)
    {
        tuning = dialog.getTuning();
        onTuningModified();
    }
}

void TuningDictionaryDialog::onTuningModified()
{
    onNumStringsChanged(ui->stringsComboBox->currentIndex());
}

TuningDictionary::Entry *TuningDictionaryDialog::selectedTuning() const
{
    return ui->tuningsList->currentItem()->data(
                0, Qt::UserRole).value<TuningDictionary::Entry *>();
}
