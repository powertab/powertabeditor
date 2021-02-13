/*
  * Copyright (C) 2014 Cameron White
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

#ifndef DIALOGS_TEXTITEMDIALOG_H
#define DIALOGS_TEXTITEMDIALOG_H

#include <QDialog>
#include <string>

class TextItem;

namespace Ui {
class TextItemDialog;
}

class TextItemDialog : public QDialog
{
    Q_OBJECT

public:
    explicit TextItemDialog(QWidget *parent, const TextItem *current_item);
    ~TextItemDialog();

    std::string getContents() const;

protected slots:
    virtual void accept() override;

private:
    Ui::TextItemDialog *ui;
};

#endif
