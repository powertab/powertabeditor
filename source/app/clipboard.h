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
  
#ifndef APP_CLIPBOARD_H
#define APP_CLIPBOARD_H

class QString;
class ScoreLocation;
class UndoManager;

namespace Clipboard
{

/// Stores the selected data on the clipboard.
void copySelection(const ScoreLocation &location);

/// Pastes notes from the clipboard at the current position.
bool paste(UndoManager &undoManager, ScoreLocation &location, QString &error_msg);

/// Returns true if any data is on the clipboard.
bool hasData();

}

#endif
