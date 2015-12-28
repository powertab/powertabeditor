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

#include "filterrulewidget.h"
#include "ui_filterrulewidget.h"

#include <score/tuning.h>
#include <score/viewfilter.h>

FilterRuleWidget::FilterRuleWidget(QWidget *parent)
    : QWidget(parent), ui(new Ui::FilterRuleWidget)
{
    ui->setupUi(this);

    ui->removeButton->setIcon(
        style()->standardIcon(QStyle::SP_TitleBarCloseButton));

    ui->stringsSpinBox->setMinimum(Tuning::MIN_STRING_COUNT);
    ui->stringsSpinBox->setMaximum(Tuning::MAX_STRING_COUNT);

    connect(ui->removeButton, &QPushButton::clicked, this,
            &FilterRuleWidget::removeRequested);

    connect(ui->subjectComboBox, static_cast<void (QComboBox::*)(int)>(
                                     &QComboBox::currentIndexChanged),
            this, &FilterRuleWidget::updateRule);
    connect(ui->regexLineEdit, &QLineEdit::textEdited, this,
            &FilterRuleWidget::updateRule);
    connect(ui->stringsSpinBox,
            static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged), this,
            &FilterRuleWidget::updateRule);
    connect(ui->operationComboBox, static_cast<void (QComboBox::*)(int)>(
                                       &QComboBox::currentIndexChanged),
            this, &FilterRuleWidget::updateRule);
}

FilterRuleWidget::~FilterRuleWidget()
{
}

void FilterRuleWidget::update(const FilterRule &rule)
{
    ui->subjectComboBox->setCurrentIndex(rule.getSubject());
    ui->stackedWidget->setCurrentIndex(rule.getSubject());

    ui->regexLineEdit->setText(QString::fromStdString(rule.getStringValue()));

    ui->operationComboBox->setCurrentIndex(rule.getOperation());
    ui->stringsSpinBox->setValue(rule.getIntValue());
}

void FilterRuleWidget::updateRule()
{
    if (ui->subjectComboBox->currentIndex() == 0)
    {
        emit changed(FilterRule(FilterRule::PLAYER_NAME,
                                ui->regexLineEdit->text().toStdString()));
    }
    else
    {
        emit changed(FilterRule(FilterRule::NUM_STRINGS,
                                static_cast<FilterRule::Operation>(
                                    ui->operationComboBox->currentIndex()),
                                ui->stringsSpinBox->value()));
    }
}
