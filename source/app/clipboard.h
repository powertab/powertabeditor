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
  
#ifndef CLIPBOARD_H
#define CLIPBOARD_H

#include <vector>

class Position;
class Tuning;
class Caret;
class UndoManager;
class QWidget;

namespace Clipboard
{

void copySelection(const std::vector<Position*>& selectedPositions,
                   const Tuning& tuning);

void paste(QWidget* parent, UndoManager *undoManager, const Caret *caret);

}
#endif // CLIPBOARD_H
