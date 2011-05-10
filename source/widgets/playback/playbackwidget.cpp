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
