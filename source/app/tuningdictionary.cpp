/*
  * Copyright (C) 2012 Cameron White
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

#include "tuningdictionary.h"

#include <QDebug>
#include <QMutexLocker>
#include <QtConcurrentRun>
#include <QCoreApplication>
#include <fstream>

#include <powertabdocument/powertabinputstream.h>
#include <powertabdocument/powertaboutputstream.h>
#include <powertabdocument/powertabfileheader.h>
#include <powertabdocument/tuning.h>

TuningDictionary::TuningDictionary()
{
}

/// Load the tuning dictionary from a file.
void TuningDictionary::load()
{
    try
    {
        std::ifstream tuningStream(tuningFilePath().c_str(),
                                   std::ios::in | std::ios::binary);
        PowerTabInputStream inputStream(tuningStream);

        std::vector<boost::shared_ptr<Tuning> > newTunings;
        inputStream.ReadVector(newTunings, PowerTabFileHeader::Version_2_0);

        QMutexLocker lock(&mutex);
        Q_UNUSED(lock);
        tunings = newTunings;
    }
    catch (std::exception &e)
    {
        qDebug() << "Error loading tuning dictionary.";
        qDebug() << "Exception: " << e.what();
    }
}

/// Saves the tuning dictionary to a file.
void TuningDictionary::save() const
{
    try
    {
        std::ofstream tuningStream(tuningFilePath().c_str(),
                                   std::ios::out | std::ios::binary);
        PowerTabOutputStream outputStream(tuningStream);

        QMutexLocker lock(&mutex);
        Q_UNUSED(lock);
        outputStream.WriteVector(tunings);
    }
    catch (std::exception &e)
    {
        qDebug() << "Error saving tuning dictionary.";
        qDebug() << "Exception: " << e.what();
    }
}

/// Loads the tuning dictionary, and runs in a separate thread.
void TuningDictionary::loadInBackground()
{
    QtConcurrent::run(this, &TuningDictionary::load);
}

/// Returns all tunings with the specified number of strings.
void TuningDictionary::findTunings(
        std::vector<boost::shared_ptr<Tuning> >& outTunings, size_t numStrings) const
{
    outTunings.clear();

    for (size_t i = 0; i < tunings.size(); ++i)
    {
        boost::shared_ptr<Tuning> tuning = tunings[i];
        if (tuning->GetStringCount() == numStrings)
        {
            outTunings.push_back(tuning);
        }
    }
}

/// Adds a new tuning to the tuning dictionary.
void TuningDictionary::addTuning(boost::shared_ptr<Tuning> tuning)
{
    tunings.push_back(tuning);
}

/// Removes the specified tuning from the dictionary.
void TuningDictionary::removeTuning(boost::shared_ptr<Tuning> tuning)
{
    tunings.erase(std::remove(tunings.begin(), tunings.end(), tuning));
}

std::string TuningDictionary::tuningFilePath()
{
    QString path = QCoreApplication::applicationDirPath() +
            "/data/tunings.dat";
    return path.toStdString();
}
