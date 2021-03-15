/*
  * Copyright (C) 2021 Simon Symeonidis
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

#include "infodialog.h"
#include "ui_infodialog.h"

#include <app/appinfo.h>

#include <QClipboard>

InfoDialog::InfoDialog(QWidget *parent) :
  QDialog(parent),
  ui(new Ui::InfoDialog)
{
    auto flags = windowFlags();
    setWindowFlags(flags | Qt::WindowStaysOnTopHint);
    ui->setupUi(this);

    setInfo();

    connect(ui->copyToClipboardButton, &QAbstractButton::clicked, [=]() {
        QClipboard *clipboard = QApplication::clipboard();
        clipboard->setText(ui->appInfo->toPlainText());
    });
}

void InfoDialog::setInfo()
{
  // TODO: this is copied from powertabeditor.cpp
  // might be a good idea to have a util function
  // to make the name
  auto name = QString("%1 %2 Beta").arg(
      AppInfo::APPLICATION_NAME,
      AppInfo::APPLICATION_VERSION
  );

  // This should probably be in utils as well, so that we can get the
  // version number that way, anywhere in the application
#ifdef VERSION
    name += QString(" (v") + BOOST_STRINGIZE(VERSION) + ")";
#endif

  ui->appInfo->setText(name);
}
