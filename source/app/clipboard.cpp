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

#include <actions/insertnotes.h>
#include <actions/undomanager.h>
#include <boost/foreach.hpp>
#include <QApplication>
#include <QClipboard>
#include <QMessageBox>
#include <QMimeData>
#include <QString>
#include <score/position.h>
#include <score/scorelocation.h>
#include <score/serialization.h>
#include <score/staff.h>
#include <sstream>

static const QString PTB_MIME_TYPE = "application/ptb";

void Clipboard::copySelection(const std::vector<Position *> &selectedPositions,
                              int numStrings)
{
    if (selectedPositions.empty())
        return;

    std::vector<Position> positions;
    BOOST_FOREACH(const Position *pos, selectedPositions)
    {
        positions.push_back(*pos);
    }

    // Serialize the notes to a string.
    std::ostringstream ss;
    ScoreUtils::save(ss, numStrings);
    ScoreUtils::save(ss, positions);
    const std::string data = ss.str();

    // Copy the data to the clipboard.
    QMimeData *mimeData = new QMimeData();
    mimeData->setData(PTB_MIME_TYPE, QByteArray(data.c_str(), data.length()));

    QClipboard *clipboard = QApplication::clipboard();
    clipboard->setMimeData(mimeData);
}

void Clipboard::paste(QWidget *parent, UndoManager &undoManager, ScoreLocation &location)
{
    const int currentStaffSize = location.getStaff().getStringCount();

    // Load data from the clipboard and deserialize.
    const QByteArray rawData = QApplication::clipboard()->mimeData()->data(
                PTB_MIME_TYPE);
    Q_ASSERT(!rawData.isEmpty());

    std::istringstream inputData(std::string(rawData.data(), rawData.length()));

    int staffSize = 0;
    ScoreUtils::load(inputData, staffSize);

    // For safety, prevent pasting into a tuning with a different number of
    // strings.
    if (currentStaffSize != staffSize)
    {
        QMessageBox msg(parent);
        msg.setText(QObject::tr("Cannot paste notes from a different tuning."));
        msg.exec();
        return;
    }

    std::vector<Position> positions;
    ScoreUtils::load(inputData, positions);
    Q_ASSERT(!positions.empty());

    undoManager.push(new InsertNotes(location, positions),
                     location.getSystemIndex());
}

bool Clipboard::hasData()
{
    return !QApplication::clipboard()->mimeData()->data(PTB_MIME_TYPE).isEmpty();
}
