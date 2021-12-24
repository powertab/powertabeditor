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
  
#include "editkeysignature.h"

#include <score/barline.h>
#include <score/score.h>

EditKeySignature::EditKeySignature(const ScoreLocation &location,
                                   const KeySignature &newKey)
    : QUndoCommand(tr("Edit Key Signature")),
      myLocation(location),
      myNewKey(newKey),
      myOldKey(location.getBarline()->getKeySignature())
{
}

void EditKeySignature::redo()
{
    myLocation.getBarline()->setKeySignature(myNewKey);
    updateFollowingKeySignatures(myOldKey, myNewKey);
}

void EditKeySignature::undo()
{
    myLocation.getBarline()->setKeySignature(myOldKey);
    updateFollowingKeySignatures(myNewKey, myOldKey);
}

void EditKeySignature::updateFollowingKeySignatures(const KeySignature &oldKey,
                                                    const KeySignature &newKey)
{
    Score &score = myLocation.getScore();
    const size_t start_system_index = myLocation.getSystemIndex();

    for (size_t i = start_system_index; i < score.getSystems().size(); ++i)
    {
        for (Barline &bar : score.getSystems()[i].getBarlines())
        {
            if (i == start_system_index &&
                bar.getPosition() <= myLocation.getPositionIndex())
            {
                continue;
            }

            const KeySignature &currentKey = bar.getKeySignature();
            if (currentKey.getKeyType() == oldKey.getKeyType() &&
                currentKey.getNumAccidentals() == oldKey.getNumAccidentals() &&
                currentKey.usesSharps() == oldKey.usesSharps())
            {
                KeySignature key;
                key.setVisible(bar.getKeySignature().isVisible());
                key.setCancellation(bar.getKeySignature().isCancellation());
                key.setSharps(newKey.usesSharps());
                key.setKeyType(newKey.getKeyType());
                key.setNumAccidentals(newKey.getNumAccidentals());
                bar.setKeySignature(key);
            }
            else
            {
                return;
            }
        }
    }
}
