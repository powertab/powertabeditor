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

#include <QValidator>

#include <score/tuning.h>
#include <score/viewfilter.h>

namespace
{
class StringFilterValidator : public QValidator
{
public:
    StringFilterValidator(QObject *parent) : QValidator(parent)
    {
    }

    State validate(QString &input, int &) const override
    {
        // Verify that the filter is valid.
        try
        {
            FilterRule rule(FilterRule::Subject::PlayerName,
                            input.toStdString());
            return State::Acceptable;
        }
        catch (const std::exception &)
        {
            return State::Intermediate;
        }
    }
};
}

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

    connect(ui->subjectComboBox,
            qOverload<int>(&QComboBox::currentIndexChanged), this,
            &FilterRuleWidget::updateRule);
    connect(ui->regexLineEdit, &QLineEdit::textEdited, this,
            &FilterRuleWidget::updateRule);
    connect(ui->stringsSpinBox, qOverload<int>(&QSpinBox::valueChanged), this,
            &FilterRuleWidget::updateRule);
    connect(ui->operationComboBox,
            qOverload<int>(&QComboBox::currentIndexChanged), this,
            &FilterRuleWidget::updateRule);

    // Add a regex validator and update its color to flag invalid patterns.
    ui->regexLineEdit->setValidator(new StringFilterValidator(this));

    // Highlight invalid text.
    ui->regexLineEdit->setStyleSheet(
        QStringLiteral("QLineEdit[acceptableInput=false] { color: red; }"));
    // Update the style when the input has changed. This is required due to
    // using the acceptableInput dynamic property.
    connect(ui->regexLineEdit, &QLineEdit::textEdited, [&]() {
        ui->regexLineEdit->style()->unpolish(ui->regexLineEdit);
        ui->regexLineEdit->style()->polish(ui->regexLineEdit);
    });
}

FilterRuleWidget::~FilterRuleWidget()
{
}

void FilterRuleWidget::update(const FilterRule &rule)
{
    ui->subjectComboBox->setCurrentIndex(int(rule.getSubject()));
    ui->stackedWidget->setCurrentIndex(int(rule.getSubject()));

    ui->regexLineEdit->setText(QString::fromStdString(rule.getStringValue()));

    ui->operationComboBox->setCurrentIndex(int(rule.getOperation()));
    ui->stringsSpinBox->setValue(rule.getIntValue());
}

void FilterRuleWidget::updateRule()
{
    if (ui->subjectComboBox->currentIndex() == 0)
    {
        if (ui->regexLineEdit->hasAcceptableInput())
        {
            emit changed(FilterRule(FilterRule::Subject::PlayerName,
                                    ui->regexLineEdit->text().toStdString()));
        }
    }
    else
    {
        emit changed(FilterRule(FilterRule::Subject::NumStrings,
                                static_cast<FilterRule::Operation>(
                                    ui->operationComboBox->currentIndex()),
                                ui->stringsSpinBox->value()));
    }
}
