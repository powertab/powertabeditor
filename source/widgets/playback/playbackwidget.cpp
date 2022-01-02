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

#include <app/viewoptions.h>
#include <app/documentmanager.h>
#include <score/score.h>
#include <score/staff.h>
#include <widgets/common.h>

#include <algorithm>
#include <string>

#include <QAction>
#include <QButtonGroup>

static QString getShortcutHint(const QAction &action)
{
    if (!action.shortcut().isEmpty())
    {
        QString shortcut = action.shortcut().toString(QKeySequence::NativeText);
        return QStringLiteral(" (%1)").arg(shortcut);
    }
    else
        return QString();
}

static QString extractPercent(const QString &text, const QLocale &locale)
{
    QString number_only(text);
    number_only.remove(locale.percent());
    return number_only;
}

static QString
percentToString(double zoom)
{
    QLocale locale;
    return QStringLiteral("%1%2").arg(zoom).arg(locale.percent());
}

class PercentageValidator : public QValidator
{
public:
    PercentageValidator(QObject *parent)
        : QValidator(parent), myNumberValidator(1, 500, 2)
    {
    }

    State validate(QString &input, int &pos) const override
    {
        QString number = extractPercent(input, locale());
        auto state = myNumberValidator.validate(number, pos);

        // Record a property on the widget indicating whether it has a valid
        // value.
        QLocale locale;
        double percent = locale.toDouble(number);
        parent()->setProperty("acceptableInput",
                              (percent >= ViewOptions::MIN_ZOOM && percent <= ViewOptions::MAX_ZOOM));
        return state;
    }

private:
    QDoubleValidator myNumberValidator;
};

PlaybackWidget::PlaybackWidget(const QAction &play_pause_command,
                               const QAction &rewind_command,
                               const QAction &stop_command,
                               const QAction &metronome_command,
                               double initial_zoom,
                               QWidget *parent)
    : QWidget(parent),
      ui(new Ui::PlaybackWidget),
      myVoices(new QButtonGroup(this))
{
    ui->setupUi(this);

    myVoices->addButton(ui->voice1Button, 0);
    myVoices->addButton(ui->voice2Button, 1);
    Q_ASSERT(myVoices->buttons().length() == Staff::NUM_VOICES);
    ui->voice1Button->setChecked(true);

    ui->speedSpinner->setMinimum(50);
    ui->speedSpinner->setMaximum(125);
    ui->speedSpinner->setSuffix(QStringLiteral("%"));
    ui->speedSpinner->setValue(100);

    ui->rewindToStartButton->setIcon(
        style()->standardIcon(QStyle::SP_MediaSkipBackward));
    connect(&rewind_command, &QAction::changed, [&]() {
        ui->rewindToStartButton->setToolTip(
            tr("Click to move playback to the beginning of the score%1.")
                .arg(getShortcutHint(rewind_command)));
    });

    ui->playPauseButton->setIcon(style()->standardIcon(QStyle::SP_MediaPlay));
    connect(&play_pause_command, &QAction::changed, [&]() {
        ui->playPauseButton->setToolTip(
            tr("Click to start or pause playback%1.")
                .arg(getShortcutHint(play_pause_command)));
    });

    ui->stopButton->setIcon(style()->standardIcon(QStyle::SP_MediaStop));
    connect(&stop_command, &QAction::changed, [&]() {
        ui->stopButton->setToolTip(
            tr("Click to stop playback and return to the initial "
               "location%1.")
                .arg(getShortcutHint(stop_command)));
    });

    ui->metronomeToggleButton->setIcon(
        style()->standardIcon(QStyle::SP_MediaVolume));
    connect(&metronome_command, &QAction::changed, [&]() {
        ui->metronomeToggleButton->setToolTip(
            tr("Click to toggle whether the metronome is turned on%1.")
                .arg(getShortcutHint(metronome_command)));
    });

    connect(myVoices,
#if (QT_VERSION >= QT_VERSION_CHECK(5,15,0))
            &QButtonGroup::idClicked,
#else
            qOverload<int>(&QButtonGroup::buttonClicked),
#endif
            this, &PlaybackWidget::activeVoiceChanged);
    connect(ui->speedSpinner,qOverload<int>(&QSpinBox::valueChanged), this,
            &PlaybackWidget::playbackSpeedChanged);
    connect(ui->filterComboBox, qOverload<int>(&QComboBox::currentIndexChanged),
            this, &PlaybackWidget::activeFilterChanged);
    connectButtonToAction(ui->playPauseButton, &play_pause_command);
    connectButtonToAction(ui->metronomeToggleButton, &metronome_command);
    connectButtonToAction(ui->rewindToStartButton, &rewind_command);
    connectButtonToAction(ui->stopButton, &stop_command);

    setupZoomComboBox(initial_zoom);
}

PlaybackWidget::~PlaybackWidget()
{
    delete ui;
}

void PlaybackWidget::reset(const Document &doc)
{
    ui->filterComboBox->blockSignals(true);

    // Rebuild the filter list.
    ui->filterComboBox->clear();
    for (const ViewFilter &filter : doc.getScore().getViewFilters())
    {
        ui->filterComboBox->addItem(
            QString::fromStdString(filter.getDescription()));
    }

    // Update the selected filter.
    if (doc.getViewOptions().getFilter())
        ui->filterComboBox->setCurrentIndex(*doc.getViewOptions().getFilter());

    // Update the selected voice.
    myVoices->button(doc.getCaret().getLocation().getVoiceIndex())
        ->setChecked(true);

    // Update zoom.
    const double zoom = doc.getViewOptions().getZoom();
    ui->zoomComboBox->setCurrentText(percentToString(zoom));

    ui->filterComboBox->blockSignals(false);
}

int PlaybackWidget::getPlaybackSpeed() const
{
    return ui->speedSpinner->value();
}

void PlaybackWidget::setPlaybackMode(bool isPlaying)
{
    if (isPlaying)
    {
        ui->playPauseButton->setIcon(
            style()->standardIcon(QStyle::SP_MediaPause));
    }
    else
    {
        ui->playPauseButton->setIcon(
            style()->standardIcon(QStyle::SP_MediaPlay));
    }
}

void PlaybackWidget::updateLocationLabel(const std::string &location)
{
    ui->locationLabel->setText(QString::fromStdString(location));
}

void
PlaybackWidget::setupZoomComboBox(double initial_zoom)
{
    // Add zoom options
    for (int zoom : ViewOptions::ZOOM_LEVELS)
        ui->zoomComboBox->addItem(percentToString(zoom));

    ui->zoomComboBox->setCurrentText(percentToString(initial_zoom));

    // Display a different style for invalid zoom values.
    ui->zoomComboBox->setStyleSheet(
        QStringLiteral("QComboBox[acceptableInput=false] { color: red; }"));

    ui->zoomComboBox->setValidator(new PercentageValidator(ui->zoomComboBox));

    connect(ui->zoomComboBox, &QComboBox::currentTextChanged,
            [=](const QString &text) {
                // Trigger an update for the stylesheet.
                ui->zoomComboBox->style()->unpolish(ui->zoomComboBox);
                ui->zoomComboBox->style()->polish(ui->zoomComboBox);

                QLocale locale;
                double percentage =
                    std::clamp(locale.toDouble(extractPercent(text, locale)),
                               ViewOptions::MIN_ZOOM, ViewOptions::MAX_ZOOM);
                emit zoomChanged(percentage);
            });
}

