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

#include <QDebug>

DirectionDialog::DirectionDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DirectionDialog),
    currentSymbol(0)
{
    ui->setupUi(this);

    // Add the initial symbol.
    direction.AddSymbol(Direction::coda, Direction::activeNone, 0);

    for (uint8_t i = Direction::coda; i <= Direction::dalSegnoSegnoAlFine; ++i)
    {
        ui->symbolComboBox->addItem(
                    QString::fromStdString(Direction::GetDetailedText(i)));
    }

    ui->activeSymbolComboBox->addItem("None");
    ui->activeSymbolComboBox->addItem("D.C.");
    ui->activeSymbolComboBox->addItem("D.S.");
    ui->activeSymbolComboBox->addItem("D.S.S.");

    ui->repeatNumberSpinBox->setMinimum(Direction::MIN_REPEAT_NUMBER);
    ui->repeatNumberSpinBox->setMaximum(Direction::MAX_REPEAT_NUMBER);
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
    return direction;
}

void DirectionDialog::accept()
{
    direction.SetSymbol(currentSymbol,
                        ui->symbolComboBox->currentIndex(),
                        ui->activeSymbolComboBox->currentIndex(),
                        ui->repeatNumberSpinBox->value());
    done(Accepted);
}

/// Add a new direction symbol.
void DirectionDialog::onAddDirection()
{
    direction.AddSymbol(Direction::coda, Direction::activeNone, 0);
    ui->directionComboBox->addItem(
                QString::number(direction.GetSymbolCount()));

    ui->directionComboBox->setCurrentIndex(direction.GetSymbolCount() - 1);

    if (direction.GetSymbolCount() == Direction::MAX_SYMBOLS)
    {
        ui->addDirectionButton->setDisabled(true);
    }

    ui->removeDirectionButton->setEnabled(true);
}

/// Remove the active direction symbol.
void DirectionDialog::onRemoveDirection()
{
    const int index = ui->directionComboBox->currentIndex();
    ui->directionComboBox->clear();

    direction.RemoveSymbolAtIndex(index);

    // Rebuild the list of symbols.
    for (size_t i = 0; i < direction.GetSymbolCount(); ++i)
    {
        ui->directionComboBox->addItem(QString::number(i + 1));
    }
    ui->directionComboBox->setCurrentIndex(
                std::min<int>(index, direction.GetSymbolCount() - 1));

    // Update the Add/Remove buttons.
    ui->addDirectionButton->setEnabled(true);

    if (direction.GetSymbolCount() == 1)
    {
        ui->removeDirectionButton->setDisabled(true);
    }
}

/// When the active symbol is changed, save the current symbol and load data
/// for the new symbol.
void DirectionDialog::onSymbolIndexChanged(int index)
{
    if (direction.IsValidSymbolIndex(currentSymbol))
    {
        direction.SetSymbol(currentSymbol,
                            ui->symbolComboBox->currentIndex(),
                            ui->activeSymbolComboBox->currentIndex(),
                            ui->repeatNumberSpinBox->value());
    }

    if (direction.IsValidSymbolIndex(index))
    {
        uint8_t symbolType = 0;
        uint8_t activeSymbol = 0;
        uint8_t repeatNumber = 0;
        direction.GetSymbol(index, symbolType, activeSymbol, repeatNumber);

        ui->symbolComboBox->setCurrentIndex(symbolType);
        ui->activeSymbolComboBox->setCurrentIndex(activeSymbol);
        ui->repeatNumberSpinBox->setValue(repeatNumber);
    }

    currentSymbol = index;
}
