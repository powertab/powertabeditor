/*
  * Copyright (C) 2020 Cameron White
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

#ifndef UTIL_DATE_H
#define UTIL_DATE_H

namespace Util
{
/// Simple struct to hold the year / month / day of a date (expected to use the
/// Gregorian calendar). This reduces bloat in headers - for actual date
/// manipulation, use a proper library like boost::date_time or QDate.
class Date
{
public:
    Date() = default;
    Date(int y, int m, int d);

    bool operator==(const Date &other) const;

    int year() const { return myYear; }
    int month() const { return myMonth; }
    int day() const { return myDay; }

private:
    int myYear = -1;
    int myMonth = -1;
    int myDay = -1;
};

inline Date::Date(int y, int m, int d) : myYear(y), myMonth(m), myDay(d)
{
}

inline bool
Date::operator==(const Date &other) const
{
    return myYear == other.myYear && myMonth == other.myMonth &&
           myDay == other.myDay;
}
} // namespace Util

#endif
