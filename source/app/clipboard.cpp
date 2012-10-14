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
#include <QMessageBox>

#include <sstream>

#include <actions/undomanager.h>
#include <actions/insertnotes.h>

#include <painters/caret.h>
#include <powertabdocument/guitar.h>
#include <powertabdocument/powertabfileheader.h>
#include <powertabdocument/powertabinputstream.h>
#include <powertabdocument/powertaboutputstream.h>
#include <powertabdocument/position.h>
#include <powertabdocument/score.h>
#include <powertabdocument/staff.h>
#include <powertabdocument/tuning.h>

namespace
{
const QString PTB_MIME_TYPE = "application/ptb";
}

/// Stores the selected data on the clipboard
void Clipboard::copySelection(const std::vector<Position*>& selectedPositions,
                              const Tuning& tuning)
{
    // serialize the notes to a string
    std::ostringstream ss(std::ios::binary);
    PowerTabOutputStream outputStream(ss);
    tuning.Serialize(outputStream);
    outputStream.WriteVector(selectedPositions);
    const std::string data = ss.str();

    // copy the data to the clipboard
    QMimeData* mimeData = new QMimeData;
    mimeData->setData(PTB_MIME_TYPE, QByteArray(data.c_str(), data.length()));

    QClipboard* clipboard = QApplication::clipboard();
    clipboard->setMimeData(mimeData);
}

void Clipboard::paste(QWidget* parent, UndoManager* undoManager,
                      const Caret* caret)
{
    const Tuning& currentTuning = caret->getCurrentScore()->GetGuitar(
                caret->getCurrentStaffIndex())->GetTuning();

    // load data from the clipboard and deserialize
    const QByteArray rawData = QApplication::clipboard()->mimeData()->data(PTB_MIME_TYPE);
    if (rawData.isEmpty())
    {
        QMessageBox msg(parent);
        msg.setText(QObject::tr("The clipboard does not contain any notes."));
        msg.exec();
        return;
    }

    std::istringstream inputData(std::string(rawData.data(), rawData.length()));
    PowerTabInputStream inputStream(inputData);

    Tuning tuning;
    tuning.Deserialize(inputStream, PowerTabFileHeader::FILEVERSION_CURRENT);

    // For safety, prevent pasting into a different tuning.
    // TODO - it should be possible to e.g. paste notes from a 6-string guitar
    // into a 7-string guitar.
    if (!currentTuning.IsSameTuning(tuning))
    {
        QMessageBox msg(parent);
        msg.setText(QObject::tr("Cannot paste notes from a different tuning."));
        msg.exec();
        return;
    }

    std::vector<Position*> positions;
    inputStream.ReadVector(positions, PowerTabFileHeader::FILEVERSION_CURRENT);

    undoManager->push(new InsertNotes(caret->getCurrentSystem(), caret->getCurrentStaff(),
                                      caret->getCurrentPositionIndex(), positions));
}
