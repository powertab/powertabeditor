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
  
#ifndef ACTIONS_REMOVESPECIALNOTEPROPERTY_H
#define ACTIONS_REMOVESPECIALNOTEPROPERTY_H

#include <functional>
#include <QCoreApplication>
#include <QUndoCommand>
#include <score/note.h>
#include <score/scorelocation.h>

/// Helper class to reduce code duplication for removing special note properties
/// such as bends or trills.
template <typename T>
class RemoveSpecialNoteProperty : public QUndoCommand
{
public:
    typedef std::function<void(Note *)> Clearer;
    typedef std::function<void(Note *, T)> Setter;

    RemoveSpecialNoteProperty(const ScoreLocation &location,
                              const QString &text, const T &currentValue,
                              const Clearer &clearer, const Setter &setter)
        : QUndoCommand(text),
          myLocation(location),
          myOriginalValue(currentValue),
          myClearer(clearer),
          mySetter(setter) {}

    virtual void redo() override
	{
		myClearer(myLocation.getNote());
	}

    virtual void undo() override
    {
		mySetter(myLocation.getNote(), myOriginalValue);
	}

private:
    ScoreLocation myLocation;
    const T myOriginalValue;
    Clearer myClearer;
    Setter mySetter;
};

class RemoveTappedHarmonic : public RemoveSpecialNoteProperty<int>
{
    Q_DECLARE_TR_FUNCTIONS(RemoveTappedHarmonic)

public:
    RemoveTappedHarmonic(const ScoreLocation &location)
        : RemoveSpecialNoteProperty<int>(
              location, tr("Remove Tapped Harmonic"),
              location.getNote()->getTappedHarmonicFret(),
              std::mem_fn(&Note::clearTappedHarmonic),
              std::mem_fn(&Note::setTappedHarmonicFret)) {}
};

class RemoveTrill : public RemoveSpecialNoteProperty<int>
{
    Q_DECLARE_TR_FUNCTIONS(RemoveTrill)

public:
    RemoveTrill(const ScoreLocation &location)
        : RemoveSpecialNoteProperty<int>(location, tr("Remove Trill"),
                                         location.getNote()->getTrilledFret(),
                                         std::mem_fn(&Note::clearTrill),
                                         std::mem_fn(&Note::setTrilledFret)) {}
};

class RemoveArtificialHarmonic
    : public RemoveSpecialNoteProperty<ArtificialHarmonic>
{
    Q_DECLARE_TR_FUNCTIONS(RemoveArtificialHarmonic)

public:
    RemoveArtificialHarmonic(const ScoreLocation &location)
        : RemoveSpecialNoteProperty<ArtificialHarmonic>(
              location, tr("Remove Artificial Harmonic"),
              location.getNote()->getArtificialHarmonic(),
              std::mem_fn(&Note::clearArtificialHarmonic),
              std::mem_fn(&Note::setArtificialHarmonic)) {}
};

class RemoveBend : public RemoveSpecialNoteProperty<Bend>
{
    Q_DECLARE_TR_FUNCTIONS(RemoveBend)

public:
    RemoveBend(const ScoreLocation &location)
        : RemoveSpecialNoteProperty<Bend>(location, tr("Remove Bend"),
                                          location.getNote()->getBend(),
                                          std::mem_fn(&Note::clearBend),
                                          std::mem_fn(&Note::setBend)) {}
};

class RemoveLeftHandFingering : public RemoveSpecialNoteProperty<LeftHandFingering>
{
    Q_DECLARE_TR_FUNCTIONS(RemoveLeftHandFingering)

public:
    RemoveLeftHandFingering(const ScoreLocation &location)
        : RemoveSpecialNoteProperty<LeftHandFingering>(location,
            tr("Remove Left Hand Fingering"),
            location.getNote()->getLeftHandFingering(),
            std::mem_fn(&Note::clearLeftHandFingering),
            std::mem_fn(&Note::setLeftHandFingering)) {}
};

#endif
