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
  
#ifndef EDITKEYSIGNATURE_H
#define EDITKEYSIGNATURE_H

#include <QUndoCommand>
#include <powertabdocument/systemlocation.h>
#include <powertabdocument/keysignature.h>

class Score;

class EditKeySignature : public QUndoCommand
{
public:
    EditKeySignature(Score* score, const SystemLocation& location,
                     uint8_t newKeyType, uint8_t newKeyAccidentals, bool isShown);

    void redo();
    void undo();

private:
    void switchKeySignatures(const KeySignature& oldKey, const KeySignature& newKey);

    Score* score;
    const SystemLocation location;
    KeySignature newKeySig;
    KeySignature oldKeySig;
};

#endif // EDITKEYSIGNATURE_H
