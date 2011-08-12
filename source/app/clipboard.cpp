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
  
#include "clipboard.h"

#include <QApplication>
#include <QClipboard>
#include <QMimeData>
#include <QDebug>

#include <sstream>

#include <actions/undomanager.h>
#include <actions/insertnotes.h>

#include <painters/caret.h>
#include <powertabdocument/powertaboutputstream.h>
#include <powertabdocument/powertabinputstream.h>
#include <powertabdocument/powertabfileheader.h>
#include <powertabdocument/position.h>
#include <powertabdocument/staff.h>

namespace
{
const QString PTB_MIME_TYPE = "application/ptb";
}

/// Stores the selected data on the clipboard
void Clipboard::copySelection(const std::vector<Position*>& selectedPositions)
{
    // serialize the notes to a string
    std::ostringstream ss(std::ios::binary);
    PowerTabOutputStream outputStream(ss);
    outputStream.WriteVector(selectedPositions);
    const std::string data = ss.str();

    // copy the data to the clipboard
    QMimeData* mimeData = new QMimeData;
    mimeData->setData(PTB_MIME_TYPE, QByteArray(data.c_str(), data.length()));

    QClipboard* clipboard = QApplication::clipboard();
    clipboard->setMimeData(mimeData);
}

void Clipboard::paste(UndoManager* undoManager, const Caret* caret)
{
    // load data from the clipboard and deserialize
    const QByteArray rawData = QApplication::clipboard()->mimeData()->data(PTB_MIME_TYPE);
    std::istringstream inputData(std::string(rawData.data(), rawData.length()));
    PowerTabInputStream inputStream(inputData);

    std::vector<Position*> positions;
    inputStream.ReadVector(positions, PowerTabFileHeader::FILEVERSION_CURRENT);

    undoManager->push(new InsertNotes(caret->getCurrentSystem(), caret->getCurrentStaff(),
                                      caret->getCurrentPositionIndex(), positions));
}
