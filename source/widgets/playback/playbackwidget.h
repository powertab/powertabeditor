#ifndef PLAYBACKWIDGET_H
#define PLAYBACKWIDGET_H

#include <QWidget>

class QSpinBox;

class PlaybackWidget : public QWidget
{
    Q_OBJECT
public:
    explicit PlaybackWidget(QWidget* parent = 0);

    int getPlaybackSpeed() const;

signals:
    void playbackSpeedChanged(int speed);

private:
    QSpinBox* playbackSpeedSpinner;
};

#endif // PLAYBACKWIDGET_H
