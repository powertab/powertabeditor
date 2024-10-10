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
    ClipboardSelection() = default;

    ClipboardSelection(int num_strings, const std::vector<const Position *> &positions,
                       const std::vector<const IrregularGrouping *> &groups)
        : myNumStrings(num_strings)
    {
        myPositions.reserve(positions.size());
        for (const Position *pos : positions)
            myPositions.push_back(*pos);

        myGroups.reserve(groups.size());
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
    int myNumStrings = 0;
    std::vector<Position> myPositions;
    std::vector<IrregularGrouping> myGroups;
};

void
Clipboard::copySelection(const ScoreLocation &location)
{
    const std::vector<const Position *> selected_positions = location.getSelectedPositions();
    if (selected_positions.empty())
        return;

    const int num_strings = location.getStaff().getStringCount();
    
    ClipboardSelection selection(num_strings, selected_positions,
                                 location.getSelectedIrregularGroupings());

    // Serialize the selected items to save to the clipboard.
    std::ostringstream ss;
    ScoreUtils::save(ss, "clipboard_selection", selection);
    const std::string data = ss.str();

    // Copy the data to the clipboard.
    auto mime_data = std::make_unique<QMimeData>();
    mime_data->setData(PTB_MIME_TYPE,
                      QByteArray(data.c_str(), static_cast<qsizetype>(data.length())));

    QClipboard *clipboard = QApplication::clipboard();
    // Ownership is transferred to the clipboard.
    clipboard->setMimeData(mime_data.release());
}

bool
Clipboard::paste(UndoManager &undoManager, ScoreLocation &location, QString &error_msg)
{
    // Load data from the clipboard and deserialize.
    const QByteArray raw_data = QApplication::clipboard()->mimeData()->data(PTB_MIME_TYPE);
    Q_ASSERT(!raw_data.isEmpty());

    std::istringstream input_data(std::string(raw_data.data(), raw_data.length()));

    ClipboardSelection selection;
    ScoreUtils::load(input_data, "clipboard_selection", selection);

    // For safety, prevent pasting into a tuning with a different number of
    // strings.
    const int current_num_strings = location.getStaff().getStringCount();
    if (current_num_strings != selection.getNumStrings())
    {
        error_msg = QCoreApplication::translate(
            "PowerTabEditor", "Cannot paste notes from a different number of strings.");
        return false;
    }

    undoManager.push(new InsertNotes(location, selection.getPositions(),
                                     selection.getIrregularGroupings()),
                     location.getSystemIndex());
    return true;
}

bool Clipboard::hasData()
{
    return !QApplication::clipboard()->mimeData()->data(PTB_MIME_TYPE).isEmpty();
}
