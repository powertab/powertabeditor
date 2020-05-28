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

#include "crashdialog.h"
#include "ui_crashdialog.h"

#include <app/appinfo.h>
#include <QDesktopServices>
#include <QPlainTextEdit>

CrashDialog::CrashDialog(const QString &stacktrace, QWidget *parent)
    : QDialog(parent), ui(new Ui::CrashDialog)
{
    ui->setupUi(this);
    ui->stacktraceText->setPlainText(stacktrace);

    connect(ui->closeButton, &QPushButton::clicked, this, &QDialog::accept);
    connect(ui->reportBugButton, &QPushButton::clicked, this,
            &CrashDialog::launchBugReport);
}

CrashDialog::~CrashDialog()
{
    delete ui;
}

void CrashDialog::launchBugReport()
{
    QDesktopServices::openUrl(QUrl(AppInfo::BUG_TRACKER_URL));
}
