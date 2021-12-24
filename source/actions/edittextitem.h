/*
  * Copyright (C) 2014 Cameron White
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

#ifndef ACTIONS_EDITTEXTITEM_H
#define ACTIONS_EDITTEXTITEM_H

#include <QCoreApplication>
#include <QUndoCommand>
#include <score/scorelocation.h>
#include <score/textitem.h>

class AddTextItem : public QUndoCommand
{
    Q_DECLARE_TR_FUNCTIONS(AddTextItem)

public:
    AddTextItem(const ScoreLocation &location, const TextItem &text);

    virtual void redo() override;
    virtual void undo() override;

private:
    ScoreLocation myLocation;
    const TextItem myText;
};

class RemoveTextItem : public QUndoCommand
{
    Q_DECLARE_TR_FUNCTIONS(RemoveTextItem)

public:
    RemoveTextItem(const ScoreLocation &location);

    virtual void redo() override;
    virtual void undo() override;

private:
    ScoreLocation myLocation;
    const TextItem myOriginalText;
};

#endif
