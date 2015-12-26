/*
  * Copyright (C) 2015 Cameron White
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

#include "viewfilterdialog.h"
#include "ui_viewfilterdialog.h"

ViewFilterDialog::ViewFilterDialog(QWidget *parent)
    : QDialog(parent), ui(new Ui::ViewFilterDialog)
{
    ui->setupUi(this);
}

ViewFilterDialog::~ViewFilterDialog()
{
}

void ViewFilterDialog::setPresenter(ViewFilterPresenter *presenter)
{
    myPresenter = presenter;

    connect(ui->addFilterButton, &QToolButton::clicked, [&]() {
        myPresenter->addFilter();
    });
    connect(ui->removeFilterButton, &QToolButton::clicked, [&]() {
        myPresenter->removeSelectedFilter();
    });
    connect(ui->filterList, &QListWidget::itemClicked, [&](QListWidgetItem *) {
        myPresenter->selectFilter(ui->filterList->currentRow());
    });
}

void ViewFilterDialog::setFilterNames(const std::vector<std::string> &names,
                                      const boost::optional<int> &selection)
{
    ui->filterList->clear();

    QStringList q_names;
    for (auto &&name : names)
        q_names.append(QString::fromStdString(name));

    ui->filterList->addItems(q_names);
    ui->filterList->setCurrentRow(selection ? *selection : -1);
    ui->removeFilterButton->setEnabled(!names.empty());
}
