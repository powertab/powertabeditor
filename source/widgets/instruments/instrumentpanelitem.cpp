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

#include "instrumentpanelitem.h"
#include "ui_instrumentpanelitem.h"

#include <score/generalmidi.h>
#include <score/instrument.h>

InstrumentPanelItem::InstrumentPanelItem(QWidget *parent, int instrumentIndex,
                                         const Instrument &instrument)
    : QWidget(parent),
      ui(new Ui::InstrumentPanelItem),
      myInstrumentIndex(instrumentIndex)
{
    ui->setupUi(this);

    ui->instrumentNumber->setText(
        QStringLiteral("%1.").arg(instrumentIndex + 1));
    ui->instrumentNameLabel->setText(
        QString::fromStdString(instrument.getDescription()));
    ui->instrumentNameEdit->setText(ui->instrumentNameLabel->text());

    for (const std::string &name : Midi::getPresetNames())
        ui->midiInstrument->addItem(QString::fromStdString(name));
    ui->midiInstrument->setCurrentIndex(instrument.getMidiPreset());

    ui->instrumentNameEdit->hide();

    ui->removeButton->setIcon(
        style()->standardIcon(QStyle::SP_TitleBarCloseButton));

    connect(ui->instrumentNameLabel, &ClickableLabel::clicked,
            ui->instrumentNameLabel, &QWidget::hide);
    connect(ui->instrumentNameLabel, &ClickableLabel::clicked,
            ui->instrumentNameEdit, &QWidget::show);
    connect(ui->instrumentNameLabel, &ClickableLabel::clicked, [=]()
    { ui->instrumentNameEdit->setFocus(); });

    connect(ui->instrumentNameEdit, &QLineEdit::editingFinished, this,
            &InstrumentPanelItem::onInstrumentNameEdited);

    connect(ui->midiInstrument, qOverload<int>(&QComboBox::activated), this,
            &InstrumentPanelItem::onEdited);

    connect(ui->removeButton, &QPushButton::clicked, this,
            &InstrumentPanelItem::instrumentRemoved);
}

InstrumentPanelItem::~InstrumentPanelItem()
{
    delete ui;
}

void InstrumentPanelItem::update(const Instrument &instrument)
{
    ui->instrumentNameLabel->setText(
        QString::fromStdString(instrument.getDescription()));
    ui->instrumentNameEdit->setText(ui->instrumentNameLabel->text());
    ui->midiInstrument->setCurrentIndex(instrument.getMidiPreset());
}

void InstrumentPanelItem::onInstrumentNameEdited()
{
    // Avoid sending another message when the editor becomes hidden.
    if (ui->instrumentNameEdit->isHidden())
        return;

    ui->instrumentNameLabel->setText(ui->instrumentNameEdit->text());
    ui->instrumentNameEdit->hide();
    ui->instrumentNameLabel->show();

    onEdited();
}

void InstrumentPanelItem::onEdited()
{
    Instrument instrument;
    instrument.setDescription(ui->instrumentNameLabel->text().toStdString());
    instrument.setMidiPreset(ui->midiInstrument->currentIndex());

    instrumentEdited(instrument);
}
