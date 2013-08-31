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

#include <boost/foreach.hpp>
#include <fstream>
#include <QCoreApplication>
#include <QDebug>
#include <QDir>
#include <QMutexLocker>
#include <QtConcurrentRun>
#include <score/serialization.h>
#include <stdexcept>

void TuningDictionary::load()
{
    try
    {
        QByteArray path = tuningFilePath().toLocal8Bit();
        std::ifstream file(path.constData(), std::ios::in | std::ios::binary);

        QMutexLocker lock(&myMutex);
        ScoreUtils::load(file, myTunings);
    }
    catch (const std::exception &e)
    {
        qDebug() << "Error loading tuning dictionary.";
        qDebug() << "Exception: " << e.what();
    }
}

void TuningDictionary::save() const
{
    try
    {
        // Ensure the directory exists first.
        if (!QDir().mkpath(QFileInfo(tuningFilePath()).path()))
        {
            throw std::runtime_error("Could not create data directory");
        }

        QByteArray path = tuningFilePath().toLocal8Bit();
        std::ofstream file(path.constData());

        QMutexLocker lock(&myMutex);
        ScoreUtils::save(file, myTunings);
    }
    catch (const std::exception &e)
    {
        qDebug() << "Error saving tuning dictionary.";
        qDebug() << "Exception: " << e.what();
    }
}

void TuningDictionary::loadInBackground()
{
    QtConcurrent::run(this, &TuningDictionary::load);
}

void TuningDictionary::findTunings(int numStrings,
                                   std::vector<Tuning *> &tunings)
{
    BOOST_FOREACH(Tuning &tuning, myTunings)
    {
        if (tuning.getStringCount() == numStrings)
            tunings.push_back(&tuning);
    }
}

void TuningDictionary::findTunings(int numStrings,
                                   std::vector<const Tuning *> &tunings) const
{
    BOOST_FOREACH(const Tuning &tuning, myTunings)
    {
        if (tuning.getStringCount() == numStrings)
            tunings.push_back(&tuning);
    }
}

void TuningDictionary::addTuning(const Tuning &tuning)
{
    myTunings.push_back(tuning);
}

void TuningDictionary::removeTuning(const Tuning &tuning)
{
    myTunings.erase(std::remove(myTunings.begin(), myTunings.end(), tuning));
}

QString TuningDictionary::tuningFilePath()
{
    return QCoreApplication::applicationDirPath() + "/data/tunings.dat";
}
