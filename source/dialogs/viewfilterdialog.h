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

#ifndef DIALOGS_VIEWFILTERDIALOG_H
#define DIALOGS_VIEWFILTERDIALOG_H

#include <QDialog>
#include "viewfilterpresenter.h"

namespace Ui {
    class ViewFilterDialog;
}

class ViewFilterDialog : public QDialog, public ViewFilterView
{
    Q_OBJECT

public:
    explicit ViewFilterDialog(QWidget *parent = 0);
    ~ViewFilterDialog();

    void setPresenter(ViewFilterPresenter *presenter) override;
    bool launch() override;
    void update(const std::vector<std::string> &names,
                const std::optional<int> &selection,
                const std::vector<FilterRule> &rules,
                const std::vector<std::string> &matches) override;

private:
    std::unique_ptr<Ui::ViewFilterDialog> ui;
    ViewFilterPresenter *myPresenter;
};

#endif
