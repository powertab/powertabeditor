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

class ClipboardSelection
{
public:
    ClipboardSelection() : myNumStrings(0)
    {
    }

    ClipboardSelection(int numStrings,
                       const std::vector<const Position *> &positions,
                       const std::vector<const IrregularGrouping *> &groups)
        : myNumStrings(numStrings)
    {
        for (const Position *pos : positions)
            myPositions.push_back(*pos);
        for (const IrregularGrouping *group : groups)
            myGroups.push_back(*group);
    }

    int getNumStrings() const
    {
        return myNumStrings;
    }

    const std::vector<Position> &getPositions() const
    {
        return myPositions;
    }

    const std::vector<IrregularGrouping> &getIrregularGroupings() const
    {
        return myGroups;
    }

    template <class Archive>
    void serialize(Archive &ar, const FileVersion /*version*/)
    {
        ar("num_strings", myNumStrings);
        ar("positions", myPositions);
        ar("irregular_groupings", myGroups);
    }

private:
    int myNumStrings;
    std::vector<Position> myPositions;
    std::vector<IrregularGrouping> myGroups;
};

void Clipboard::copySelection(const ScoreLocation &location)
{
    const auto selectedPositions = location.getSelectedPositions();
    const int numStrings = location.getStaff().getStringCount();
    
    if (selectedPositions.empty())
        return;

    
    ClipboardSelection selection(numStrings, selectedPositions,
                                 location.getSelectedIrregularGroupings());

    // Serialize the notes to a string.
    std::ostringstream ss;
    ScoreUtils::save(ss, "clipboard_selection", selection);
    const std::string data = ss.str();

    // Copy the data to the clipboard.
    auto mimeData = new QMimeData();
    mimeData->setData(
        PTB_MIME_TYPE,
        QByteArray(data.c_str(), static_cast<int>(data.length())));

    QClipboard *clipboard = QApplication::clipboard();
    clipboard->setMimeData(mimeData);
}

void Clipboard::paste(QWidget *parent, UndoManager &undoManager,
                      ScoreLocation &location)
{
    const int currentStaffSize = location.getStaff().getStringCount();

    // Load data from the clipboard and deserialize.
    const QByteArray rawData =
        QApplication::clipboard()->mimeData()->data(PTB_MIME_TYPE);
    Q_ASSERT(!rawData.isEmpty());

    std::istringstream inputData(std::string(rawData.data(), rawData.length()));

    ClipboardSelection selection;
    ScoreUtils::load(inputData, "clipboard_selection", selection);

    // For safety, prevent pasting into a tuning with a different number of
    // strings.
    if (currentStaffSize != selection.getNumStrings())
    {
        QMessageBox msg(parent);
        msg.setText(QCoreApplication::translate(
            "PowerTabEditor", "Cannot paste notes from a different tuning."));
        msg.exec();
        return;
    }

    undoManager.push(new InsertNotes(location, selection.getPositions(),
                                     selection.getIrregularGroupings()),
                     location.getSystemIndex());
}

bool Clipboard::hasData()
{
    return !QApplication::clipboard()
                ->mimeData()
                ->data(PTB_MIME_TYPE)
                .isEmpty();
}
