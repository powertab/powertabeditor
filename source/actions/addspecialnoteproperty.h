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
  
#ifndef ACTIONS_ADDSPECIALNOTEPROPERTY_H
#define ACTIONS_ADDSPECIALNOTEPROPERTY_H

#include <functional>
#include <QCoreApplication>
#include <QUndoCommand>
#include <score/note.h>
#include <score/scorelocation.h>

/// Helper class to reduce code duplication for adding special note properties
/// such as bends or trills.
template <typename T>
class AddSpecialNoteProperty : public QUndoCommand
{
public:
    typedef std::function<void(Note *, T)> Setter;
    typedef std::function<void(Note *)> Clearer;

    AddSpecialNoteProperty(const ScoreLocation &location, const QString &text,
                           const T &value, const Setter &setter,
                           const Clearer &clearer)
        : QUndoCommand(text),
          myLocation(location),
          myValue(value),
          mySetter(setter),
          myClearer(clearer)
    {
    }

    virtual void redo() override { mySetter(myLocation.getNote(), myValue); }

    virtual void undo() override { myClearer(myLocation.getNote()); }

private:
    ScoreLocation myLocation;
    const T myValue;
    Setter mySetter;
    Clearer myClearer;
};

class AddTappedHarmonic : public AddSpecialNoteProperty<int>
{
    Q_DECLARE_TR_FUNCTIONS(AddTappedHarmonic)

public:
    AddTappedHarmonic(const ScoreLocation &location, int fret)
        : AddSpecialNoteProperty<int>(location,
                                      tr("Add Tapped Harmonic"), fret,
                                      std::mem_fn(&Note::setTappedHarmonicFret),
                                      std::mem_fn(&Note::clearTappedHarmonic))
    {
    }
};

class AddTrill : public AddSpecialNoteProperty<int>
{
    Q_DECLARE_TR_FUNCTIONS(AddTrill)

public:
    AddTrill(const ScoreLocation &location, int fret)
        : AddSpecialNoteProperty<int>(location, tr("Add Trill"), fret,
                                      std::mem_fn(&Note::setTrilledFret),
                                      std::mem_fn(&Note::clearTrill))
    {
    }
};

class AddArtificialHarmonic : public AddSpecialNoteProperty<ArtificialHarmonic>
{
    Q_DECLARE_TR_FUNCTIONS(AddArtificialHarmonic)

public:
    AddArtificialHarmonic(const ScoreLocation &location,
                          const ArtificialHarmonic &harmonic)
        : AddSpecialNoteProperty<ArtificialHarmonic>(
              location, tr("Add Artificial Harmonic"), harmonic,
              std::mem_fn(&Note::setArtificialHarmonic),
              std::mem_fn(&Note::clearArtificialHarmonic))
    {
    }
};

class AddBend : public AddSpecialNoteProperty<Bend>
{
    Q_DECLARE_TR_FUNCTIONS(AddBend)

public:
    AddBend(const ScoreLocation &location, const Bend &bend)
        : AddSpecialNoteProperty<Bend>(location, tr("Add Bend"), bend,
                                       std::mem_fn(&Note::setBend),
                                       std::mem_fn(&Note::clearBend))
    {
    }
};

class AddLeftHandFingering : public AddSpecialNoteProperty<LeftHandFingering>
{
    Q_DECLARE_TR_FUNCTIONS(AddLeftHandFingering)

public:
    AddLeftHandFingering(const ScoreLocation &location, const LeftHandFingering &fingering)
        : AddSpecialNoteProperty<LeftHandFingering>(location,
            tr("Add Left Hand Fingering"), fingering,
            std::mem_fn(&Note::setLeftHandFingering),
            std::mem_fn(&Note::clearLeftHandFingering))
    {
    }
};

#endif
