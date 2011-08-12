/*
  * Copyright (C) 2011 Cameron White
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

#include <QHBoxLayout>
#include <QSpinBox>

PlaybackWidget::PlaybackWidget(QWidget* parent) :
    QWidget(parent)
{
    QHBoxLayout* layout = new QHBoxLayout;

    playbackSpeedSpinner = new QSpinBox;
    playbackSpeedSpinner->setMinimum(50);
    playbackSpeedSpinner->setMaximum(125);
    playbackSpeedSpinner->setSuffix("%");
    playbackSpeedSpinner->setValue(100);
    // prevent spinbox from stealing focus unless the user explicitly clicks in it
    // - this allows the user to change speed during playback with the scrollwheel without the spinbox stealing focus
    playbackSpeedSpinner->setFocusPolicy(Qt::StrongFocus);

    connect(playbackSpeedSpinner, SIGNAL(valueChanged(int)), this, SIGNAL(playbackSpeedChanged(int)));

    layout->addWidget(playbackSpeedSpinner);

    layout->setSizeConstraint(QLayout::SetFixedSize);
    layout->setSpacing(0);
    layout->setMargin(0);
    setLayout(layout);
}

int PlaybackWidget::getPlaybackSpeed() const
{
    return playbackSpeedSpinner->value();
}
