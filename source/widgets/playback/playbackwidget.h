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

#ifndef PLAYBACKWIDGET_H
#define PLAYBACKWIDGET_H

#include <QWidget>

namespace Ui {
class PlaybackWidget;
}

class PlaybackWidget : public QWidget
{
    Q_OBJECT
public:
    explicit PlaybackWidget(QWidget* parent = 0);
    ~PlaybackWidget();

    int playbackSpeed() const;
    void setPlaybackMode(bool isPlaying);

signals:
    void playbackSpeedChanged(int speed);
    void playbackButtonToggled();

private:
    Ui::PlaybackWidget *ui;
};

#endif // PLAYBACKWIDGET_H
