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
