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

#ifndef VOLUMESWELLDIALOG_H
#define VOLUMESWELLDIALOG_H

#include <QDialog>
#include <boost/cstdint.hpp>

namespace Ui {
    class VolumeSwellDialog;
}

class Position;
class QButtonGroup;

class VolumeSwellDialog : public QDialog
{
    Q_OBJECT

public:
    explicit VolumeSwellDialog(const Position* position, QWidget *parent = 0);
    ~VolumeSwellDialog();

    void accept();

    uint8_t getNewStartVolume() const;
    uint8_t getNewEndVolume() const;
    uint8_t getNewDuration() const;

private:
    Ui::VolumeSwellDialog *ui;

    uint8_t newStartVolume;
    uint8_t newEndVolume;
    uint8_t newDuration;

    QButtonGroup* startVolumeLevels;
    QButtonGroup* endVolumeLevels;
};

#endif // VOLUMESWELLDIALOG_H
