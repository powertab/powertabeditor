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

#ifndef WIDGETS_PLAYBACKWIDGET_H
#define WIDGETS_PLAYBACKWIDGET_H

#include <QWidget>

namespace Ui {
class PlaybackWidget;
}

class Document;
class QButtonGroup;

class PlaybackWidget : public QWidget
{
    Q_OBJECT

public:
    explicit PlaybackWidget(const QAction &play_pause_command,
                            const QAction &rewind_command,
                            const QAction &stop_command,
                            const QAction &metronome_command, QWidget *parent);
    ~PlaybackWidget();

    /// Reload any settings for the given score.
    void reset(const Document &doc);

    /// Get the current playback speed.
    int getPlaybackSpeed() const;

    /// Toggles the play/pause button.
    void setPlaybackMode(bool isPlaying);

    /// Updates the text containing the caret's location.
    void updateLocationLabel(const std::string &location);

signals:
    void playbackSpeedChanged(int speed);
    void activeVoiceChanged(int voice);
    void activeFilterChanged(int filter);
    void zoomChanged(double zoom);

private:
    void onSettingChanged(const std::string &setting);
    double validateZoom(double percent);

    Ui::PlaybackWidget *ui;
    QButtonGroup *myVoices;

    const double MAX_ZOOM = 200, MIN_ZOOM = 25;
};

#endif
