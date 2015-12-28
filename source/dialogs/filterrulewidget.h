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

#ifndef DIALOGS_FILTERRULEWIDGET_H
#define DIALOGS_FILTERRULEWIDGET_H

#include <memory>
#include <QWidget>

class FilterRule;

namespace Ui {
    class FilterRuleWidget;
}

class FilterRuleWidget : public QWidget
{
    Q_OBJECT

public:
    explicit FilterRuleWidget(QWidget *parent);
    ~FilterRuleWidget();

    void update(const FilterRule &filter);

signals:
    void changed(const FilterRule &);
    void removeRequested();

private:
    void updateRule();

    std::unique_ptr<Ui::FilterRuleWidget> ui;
};

#endif
