/*
  * Copyright (C) 2013 Cameron White
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

#include "directiondialog.h"
#include "ui_directiondialog.h"

static const int MAX_SYMBOLS = 3;

static QString theDirectionText[DirectionSymbol::NumSymbolTypes] =
{
    "Coda", "Double Coda", "Segno", "Segno Segno",
    "Fine", "Da Capo", "Dal Segno", "Dal Segno Segno", "To Coda",
    "To Double Coda", "Da Capo al Coda", "Da Capo al Double Coda",
    "Dal Segno al Coda", "Dal Segno al Double Coda", "Dal Segno Segno al Coda",
    "Dal Segno Segno al Double Coda", "Da Capo al Fine", "Dal Segno al Fine",
    "Dal Segno Segno al Fine"
};

DirectionDialog::DirectionDialog(QWidget *parent)
    : QDialog(parent),
      ui(new Ui::DirectionDialog),
      myCurrentSymbol(0)
{
    ui->setupUi(this);

    // Add the initial symbol.
    myDirection.insertSymbol(DirectionSymbol(DirectionSymbol::Coda));

    for (int i = DirectionSymbol::Coda;
         i < DirectionSymbol::NumSymbolTypes; ++i)
    {
        ui->symbolComboBox->addItem(theDirectionText[i]);
    }

    ui->activeSymbolComboBox->addItem("None");
    ui->activeSymbolComboBox->addItem("D.C.");
    ui->activeSymbolComboBox->addItem("D.S.");
    ui->activeSymbolComboBox->addItem("D.S.S.");

    ui->repeatNumberSpinBox->setMinimum(0);
    ui->repeatNumberSpinBox->setValue(0);

    ui->directionComboBox->addItem("1");

    ui->removeDirectionButton->setDisabled(true);

    connect(ui->addDirectionButton, SIGNAL(clicked()),
            this, SLOT(onAddDirection()));
    connect(ui->removeDirectionButton, SIGNAL(clicked()),
            this, SLOT(onRemoveDirection()));
    connect(ui->directionComboBox, SIGNAL(currentIndexChanged(int)),
            this, SLOT(onSymbolIndexChanged(int)));
}

DirectionDialog::~DirectionDialog()
{
    delete ui;
}

Direction DirectionDialog::getDirection() const
{
    return myDirection;
}

void DirectionDialog::accept()
{
    myDirection.getSymbols()[myCurrentSymbol] = DirectionSymbol(
                static_cast<DirectionSymbol::SymbolType>(
                    ui->symbolComboBox->currentIndex()),
                static_cast<DirectionSymbol::ActiveSymbolType>(
                    ui->activeSymbolComboBox->currentIndex()),
                ui->repeatNumberSpinBox->value());
    done(Accepted);
}

void DirectionDialog::onAddDirection()
{
    myDirection.insertSymbol(DirectionSymbol(DirectionSymbol::Coda));
    ui->directionComboBox->addItem(
                QString::number(myDirection.getSymbols().size()));

    ui->directionComboBox->setCurrentIndex(myDirection.getSymbols().size() - 1);

    if (myDirection.getSymbols().size() == MAX_SYMBOLS)
        ui->addDirectionButton->setDisabled(true);

    ui->removeDirectionButton->setEnabled(true);
}

void DirectionDialog::onRemoveDirection()
{
    const int index = ui->directionComboBox->currentIndex();
    ui->directionComboBox->clear();

    myDirection.removeSymbol(index);

    // Rebuild the list of symbols.
    for (long i = 0; i < myDirection.getSymbols().size(); ++i)
        ui->directionComboBox->addItem(QString::number(i + 1));

    ui->directionComboBox->setCurrentIndex(
                std::min<int>(index, myDirection.getSymbols().size() - 1));

    // Update the Add/Remove buttons.
    ui->addDirectionButton->setEnabled(true);

    if (myDirection.getSymbols().size() == 1)
        ui->removeDirectionButton->setDisabled(true);
}

void DirectionDialog::onSymbolIndexChanged(int index)
{
    if (myCurrentSymbol >= 0 &&
        myCurrentSymbol < myDirection.getSymbols().size())
    {
        myDirection.getSymbols()[myCurrentSymbol] = DirectionSymbol(
                    static_cast<DirectionSymbol::SymbolType>(
                        ui->symbolComboBox->currentIndex()),
                    static_cast<DirectionSymbol::ActiveSymbolType>(
                        ui->activeSymbolComboBox->currentIndex()),
                    ui->repeatNumberSpinBox->value());
    }

    if (index >= 0 && index < myDirection.getSymbols().size())
    {
        const DirectionSymbol &symbol = myDirection.getSymbols()[index];

        ui->symbolComboBox->setCurrentIndex(symbol.getSymbolType());
        ui->activeSymbolComboBox->setCurrentIndex(symbol.getActiveSymbolType());
        ui->repeatNumberSpinBox->setValue(symbol.getRepeatNumber());
    }

    myCurrentSymbol = index;
}
