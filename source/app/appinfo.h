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

#ifndef APP_APPINFO_H
#define APP_APPINFO_H

#include <string>

namespace AppInfo
{
    extern const char *BUG_TRACKER_URL;
    extern const char *TRANSLATIONS_URL;
    /// Organization name used by QSettings.
    extern const char *ORGANIZATION_NAME;
    /// Application name used by QSettings.
    extern const char *APPLICATION_ID;
    /// Application name that should be displayed to the user.
    extern const char *APPLICATION_NAME;
    extern const char *APPLICATION_VERSION;

    /// Make application name with version
    std::string makeApplicationName(void);
}

#endif
