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

#ifndef TUNINGDICTIONARY_H
#define TUNINGDICTIONARY_H

#include <vector>
#include <boost/shared_ptr.hpp>
#include <QMutex>

class Tuning;

class TuningDictionary
{
public:
    TuningDictionary();

    void load();
    void save();

    void loadInBackground();

private:
    static std::string tuningFilePath();

    QMutex mutex;
    std::vector<boost::shared_ptr<Tuning> > tunings;
};

#endif // TUNINGDICTIONARY_H
