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

#include <app/appinfo.h>
#include <fstream>
#include <QDebug>
#include <QDir>
#include <QMutexLocker>
#include <QStandardPaths>
#include <QtConcurrentRun>
#include <score/serialization.h>
#include <stdexcept>

void TuningDictionary::load()
{
    try
    {
        QStringList paths = availablePaths();
        for (const QString &path : paths)
        {
            if (!QFile(path).exists())
                continue;

            std::ifstream file(path.toLocal8Bit().constData());

            QMutexLocker lock(&myMutex);
            ScoreUtils::load(file, "tunings", myTunings);
            break;
        }
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
        QString path =
            QStandardPaths::writableLocation(QStandardPaths::DataLocation) +
            "/tunings.json";

        // Ensure the directory exists first.
        if (!QDir().mkpath(QFileInfo(path).path()))
            throw std::runtime_error("Error creating data directory.");

        std::ofstream file(path.toLocal8Bit().constData());
        if (!file)
            throw std::runtime_error("Error opening file for writing.");

        QMutexLocker lock(&myMutex);
        ScoreUtils::save(file, "tunings", myTunings);
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
    for (Tuning &tuning : myTunings)
    {
        if (tuning.getStringCount() == numStrings)
            tunings.push_back(&tuning);
    }
}

void TuningDictionary::findTunings(int numStrings,
                                   std::vector<const Tuning *> &tunings) const
{
    for (const Tuning &tuning : myTunings)
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

QStringList TuningDictionary::availablePaths()
{
    QStringList paths = QStandardPaths::standardLocations(QStandardPaths::DataLocation);
    paths.append(QString::fromStdString(AppInfo::getAbsolutePath("data")));

    for (QString &path : paths)
        path.append("/tunings.json");

    return paths;
}
