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

#include <dialogs/filterrulewidget.h>

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

    connect(ui->nameLineEdit, &QLineEdit::textEdited, [&](const QString &s) {
        myPresenter->editFilterDescription(s.toStdString());
    });
    connect(ui->addRuleButton, &QToolButton::clicked, [&]() {
        myPresenter->addRule();
    });
}

bool ViewFilterDialog::launch()
{
    return exec() == QDialog::Accepted;
}

void ViewFilterDialog::update(const std::vector<std::string> &names,
                              const boost::optional<int> &selection,
                              const std::vector<FilterRule> &rules)
{
    ui->filterList->clear();

    QStringList q_names;
    for (auto &&name : names)
        q_names.append(QString::fromStdString(name));

    ui->filterList->addItems(q_names);
    ui->filterList->setCurrentRow(selection ? *selection : -1);
    ui->removeFilterButton->setEnabled(!names.empty());

    ui->nameLineEdit->setEnabled((bool)selection);
    ui->addRuleButton->setEnabled((bool)selection);

    // Remove old widgets.
    while (ui->filterRuleLayout->count() > static_cast<int>(rules.size()))
    {
        auto item =
            ui->filterRuleLayout->takeAt(ui->filterRuleLayout->count() - 1);
        item->widget()->deleteLater();
        delete item;
    }

    if (selection)
    {
        ui->nameLineEdit->setText(QString::fromStdString(names[*selection]));

        // Add new widgets if necessary.
        for (size_t i = ui->filterRuleLayout->count(), n = rules.size(); i < n;
             ++i)
        {
            auto widget = new FilterRuleWidget(this);
            ui->filterRuleLayout->addWidget(widget);

            connect(widget, &FilterRuleWidget::changed,
                    [=](const FilterRule &rule) {
                myPresenter->editRule(i, rule);
            });
            connect(widget, &FilterRuleWidget::removeRequested, [=]() {
                myPresenter->removeRule(i);
            });
        }

        // Update widgets.
        for (size_t i = 0, n = rules.size(); i < n; ++i)
        {
            auto widget = dynamic_cast<FilterRuleWidget *>(
                ui->filterRuleLayout->itemAt(i)->widget());
            assert(widget);

            widget->update(rules[i]);
        }
    }
    else
    {
        ui->nameLineEdit->clear();
    }
}
