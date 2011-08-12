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
  
#ifndef MIXER_H
#define MIXER_H

#include <QFrame>

#include <vector>
#include <memory>

class SkinManager;
class MixerInstrument;
class Guitar;
class QVBoxLayout;

class Mixer : public QFrame
{
    Q_OBJECT
public:
    Mixer(std::shared_ptr<SkinManager> skinManager, QFrame *parent = 0);

    void addInstrument(std::shared_ptr<Guitar> guitar);
    void removeInstrument(size_t index);

    std::shared_ptr<Guitar> getInstrument(size_t index) const;

protected:
    std::vector<std::shared_ptr<MixerInstrument> > channelList;
    QVBoxLayout* layout;

public slots:
    void update();

};

#endif // MIXER_H
