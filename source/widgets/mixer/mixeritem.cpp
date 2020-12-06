/*
  * Copyright (C) 2013 Cameron White
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

#include "mixeritem.h"
#include "ui_mixeritem.h"

#include <dialogs/tuningdialog.h>
#include <score/player.h>
#include <util/tostring.h>

#include <QStyle>
#include <memory>

MixerItem::MixerItem(QWidget *parent, int playerIndex, const Player &player,
                     const TuningDictionary &dictionary)
    : QWidget(parent),
      ui(new Ui::MixerItem),
      myDictionary(dictionary),
      myTuning(player.getTuning())
{
    ui->setupUi(this);

    ui->playerIndexLabel->setText(QStringLiteral("%1.").arg(playerIndex + 1));
    ui->playerNameLabel->setText(
        QString::fromStdString(player.getDescription()));
    ui->playerNameEdit->setText(ui->playerNameLabel->text());
    ui->playerVolume->setValue(player.getMaxVolume());
    ui->playerPan->setValue(player.getPan());
    ui->playerTuning->setText(
        QString::fromStdString(Util::toString(player.getTuning())));

    ui->removeButton->setIcon(
        style()->standardIcon(QStyle::SP_TitleBarCloseButton));

    ui->playerNameEdit->hide();

    connect(ui->playerNameLabel, &ClickableLabel::clicked, ui->playerNameLabel,
            &QWidget::hide);
    connect(ui->playerNameLabel, &ClickableLabel::clicked, ui->playerNameEdit,
            &QWidget::show);
    connect(ui->playerNameLabel, &ClickableLabel::clicked, [=]() {
        ui->playerNameEdit->setFocus();
    });

    connect(ui->playerNameEdit, &QLineEdit::editingFinished, this,
            &MixerItem::onPlayerNameEdited);

    connect(ui->playerVolume, &QSlider::valueChanged, [=]() {
        onEdited(false);
    });
    connect(ui->playerPan, &QDial::valueChanged, [=]() {
        onEdited(false);
    });

    connect(ui->playerTuning, &ClickableLabel::clicked, this,
            &MixerItem::editTuning);

    connect(ui->removeButton, &QPushButton::clicked, this,
            &MixerItem::playerRemoved);
}

MixerItem::~MixerItem()
{
    delete ui;
}

void MixerItem::onPlayerNameEdited()
{
    // Avoid sending another message when the editor becomes hidden.
    if (ui->playerNameEdit->isHidden())
        return;

    ui->playerNameLabel->setText(ui->playerNameEdit->text());
    ui->playerNameEdit->hide();
    ui->playerNameLabel->show();

    onEdited(true);
}

void MixerItem::editTuning()
{
    // We need to make sure that the TuningDialog is deleted before we call
    // onEdited, otherwise it will get deleted twice.
    std::unique_ptr<TuningDialog> dialog(
        new TuningDialog(this, myTuning, myDictionary));

    if (dialog->exec() == QDialog::Accepted)
    {
        myTuning = dialog->getTuning();
        dialog.reset();
        onEdited(true);
    }
}

void MixerItem::onEdited(bool undoable)
{
    Player player;
    player.setDescription(ui->playerNameLabel->text().toStdString());
    player.setMaxVolume(ui->playerVolume->value());
    player.setPan(ui->playerPan->value());
    player.setTuning(myTuning);

    playerEdited(player, undoable);
}
