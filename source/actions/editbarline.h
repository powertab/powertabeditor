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
  
#ifndef DIALOGS_EDITBARLINE_H
#define DIALOGS_EDITBARLINE_H

#include <QCoreApplication>
#include <QUndoCommand>
#include <score/barline.h>
#include <score/scorelocation.h>

class EditBarline : public QUndoCommand
{
    Q_DECLARE_TR_FUNCTIONS(EditBarline)

public:
    EditBarline(const ScoreLocation &location, Barline::BarType type,
                int repeats);

    virtual void redo() override;
    virtual void undo() override;

private:
    ScoreLocation myLocation;
    const Barline::BarType myBarType;
    const Barline::BarType myOriginalBarType;
    const int myRepeats;
    const int myOriginalRepeats;
};

#endif
