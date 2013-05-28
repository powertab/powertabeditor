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

#include <boost/make_shared.hpp>

#include <app/tuningdictionary.h>
#include <dialogs/tuningdialog.h>
#include <powertabdocument/tuning.h>

Q_DECLARE_METATYPE(boost::shared_ptr<Tuning>);

TuningDictionaryDialog::TuningDictionaryDialog(
        boost::shared_ptr<TuningDictionary> tuningDictionary, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::TuningDictionaryDialog),
    tuningDictionary(tuningDictionary)
{
    ui->setupUi(this);

    ui->tuningsList->setColumnCount(2);
    ui->tuningsList->setHeaderLabels(QStringList() <<
                                     tr("Name") << tr("Tuning (Low to High)"));

#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
    ui->tuningsList->header()->sectionResizeMode(QHeaderView::ResizeToContents);
#else
    ui->tuningsList->header()->setResizeMode(QHeaderView::ResizeToContents);
#endif

    for (uint8_t i = Tuning::MIN_STRING_COUNT; i <= Tuning::MAX_STRING_COUNT; ++i)
    {
        ui->stringsComboBox->addItem(QString::number(i), i);
    }

    connect(ui->stringsComboBox, SIGNAL(currentIndexChanged(int)),
            this, SLOT(onNumStringsChanged(int)));
    connect(ui->tuningsList,
            SIGNAL(currentItemChanged(QTreeWidgetItem*,QTreeWidgetItem*)), this,
            SLOT(onCurrentTuningChanged(QTreeWidgetItem*, QTreeWidgetItem*)));
    connect(ui->newTuningButton, SIGNAL(clicked()), this, SLOT(onNewTuning()));
    connect(ui->editTuningButton, SIGNAL(clicked()), this,
            SLOT(onEditTuning()));
    connect(ui->deleteTuningButton, SIGNAL(clicked()), this,
            SLOT(onDeleteTuning()));

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

    std::vector<boost::shared_ptr<Tuning> > tunings;
    tuningDictionary->findTunings(tunings, numStrings);
    for (size_t i = 0; i < tunings.size(); ++i)
    {
        QTreeWidgetItem *item = new QTreeWidgetItem(QStringList() <<
            QString::fromStdString(tunings[i]->GetName()) <<
            QString::fromStdString(tunings[i]->GetSpelling()));
        item->setData(0, Qt::UserRole, QVariant::fromValue(tunings[i]));
        ui->tuningsList->addTopLevelItem(item);
    }
}

void TuningDictionaryDialog::onNewTuning()
{
    Tuning tuning;
    tuning.SetToStandard();
    TuningDialog dialog(this, boost::shared_ptr<const Guitar>(), tuning,
                        tuningDictionary);

    if (dialog.exec() == QDialog::Accepted)
    {
        tuningDictionary->addTuning(boost::make_shared<Tuning>(dialog.getNewTuning()));
        onTuningModified();
    }
}

void TuningDictionaryDialog::onDeleteTuning()
{
    tuningDictionary->removeTuning(selectedTuning());
    onTuningModified();
}

void TuningDictionaryDialog::onCurrentTuningChanged(QTreeWidgetItem *current,
                                                  QTreeWidgetItem */*prev*/)
{
    ui->deleteTuningButton->setEnabled(current != NULL);
    ui->editTuningButton->setEnabled(current != NULL);
}

void TuningDictionaryDialog::onEditTuning()
{
    boost::shared_ptr<Tuning> tuning = selectedTuning();
    TuningDialog dialog(this, boost::shared_ptr<const Guitar>(), *tuning,
                        tuningDictionary);
    if (dialog.exec() == QDialog::Accepted)
    {
        *tuning = dialog.getNewTuning();
        onTuningModified();
    }
}

void TuningDictionaryDialog::onTuningModified()
{
    onNumStringsChanged(ui->stringsComboBox->currentIndex());
}

boost::shared_ptr<Tuning> TuningDictionaryDialog::selectedTuning() const
{
    return ui->tuningsList->currentItem()->data(0, Qt::UserRole).
            value<boost::shared_ptr<Tuning> >();
}
