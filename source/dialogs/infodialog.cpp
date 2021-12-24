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
    const std::string name = AppInfo::makeApplicationName();
    const QString qname = QString::fromStdString(name);

    const auto developmentBinaryLocation =
        tr("You can grab development binaries here:\n"
           "  https://github.com/powertab/powertabeditor/actions");

    const auto message = QString("%1\n\n%2").arg(
        qname,
        developmentBinaryLocation
    );

    ui->appInfo->setText(message);
}
