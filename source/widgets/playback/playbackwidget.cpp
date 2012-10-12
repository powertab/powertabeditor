/*
  * Copyright (C) 2012 Cameron White
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

#include "playbackwidget.h"
#include "ui_playbackwidget.h"

PlaybackWidget::PlaybackWidget(QWidget* parent) :
    QWidget(parent),
    ui(new Ui::PlaybackWidget)
{
    ui->setupUi(this);

    ui->speedSpinner->setMinimum(50);
    ui->speedSpinner->setMaximum(125);
    ui->speedSpinner->setSuffix("%");
    ui->speedSpinner->setValue(100);

    connect(ui->speedSpinner, SIGNAL(valueChanged(int)),
            this, SIGNAL(playbackSpeedChanged(int)));
}

PlaybackWidget::~PlaybackWidget()
{
    delete ui;
}

int PlaybackWidget::playbackSpeed() const
{
    return ui->speedSpinner->value();
}
