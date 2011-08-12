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

#include <powertabdocument/score.h>
#include <powertabdocument/system.h>
#include <powertabdocument/barline.h>
#include <boost/foreach.hpp>

EditKeySignature::EditKeySignature(Score* score, const SystemLocation& location,
                                   uint8_t newKeyType, uint8_t newKeyAccidentals, bool isShown) :
    score(score),
    location(location),
    newKeySig(newKeyType, newKeyAccidentals)
{
    newKeySig.SetShown(isShown);
    // save the original key signature
    Score::SystemConstPtr system = score->GetSystem(location.getSystemIndex());
    System::BarlineConstPtr barline = system->GetBarlineAtPosition(location.getPositionIndex());
    Q_ASSERT(barline);
    oldKeySig = barline->GetKeySignature();

    setText(QObject::tr("Edit Key Signature"));
}

void EditKeySignature::redo()
{
    switchKeySignatures(oldKeySig, newKeySig);
    emit triggered();
}

void EditKeySignature::undo()
{
    switchKeySignatures(newKeySig, oldKeySig);
    emit triggered();
}

/// Switches from the old key signature to the new key signature, starting at the position
/// stored in the "location" member
/// Modifies all following key signatures until a different key signature is reached
void EditKeySignature::switchKeySignatures(const KeySignature& oldKey,
                                           const KeySignature& newKey)
{
    const size_t startSystem = location.getSystemIndex();
    for (size_t i = startSystem; i < score->GetSystemCount(); i++)
    {
        std::vector<System::BarlinePtr> barlines;
        score->GetSystem(i)->GetBarlines(barlines);

        BOOST_FOREACH(System::BarlinePtr barline, barlines)
        {
            if (i == startSystem && barline->GetPosition() < location.getPositionIndex())
            {
                continue;
            }

            KeySignature& key = barline->GetKeySignature();
            if (key.IsSameKey(oldKey))
            {
                key.SetKey(newKey.GetKeyType(), newKey.GetKeyAccidentals());

                if (i == startSystem && barline->GetPosition() == location.getPositionIndex())
                {
                    key.SetShown(newKey.IsShown());
                }
            }
            else
            {
                return;
            }
        }
    }
}
