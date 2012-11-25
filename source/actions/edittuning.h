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
  
#ifndef EDITTUNING_H
#define EDITTUNING_H

#include <QUndoCommand>

#include <boost/shared_ptr.hpp>

#include <powertabdocument/tuning.h>

class Guitar;
class MixerInstrument;
class Score;

class EditTuning : public QUndoCommand
{
public:
    EditTuning(MixerInstrument* mixer, Score* score,
               boost::shared_ptr<Guitar> guitar, const Tuning& newTuning,
               uint8_t capo);
    void redo();
    void undo();

    static bool canChangeTuning(const Score* score,
                                boost::shared_ptr<Guitar> guitar,
                                const Tuning& newTuning);
    
private:
    MixerInstrument* mixer;
    Score* score;
    boost::shared_ptr<Guitar> guitar;
    const Tuning newTuning;
    const Tuning oldTuning;

    const uint8_t oldCapo;
    const uint8_t newCapo;
};

#endif // EDITTUNING_H
