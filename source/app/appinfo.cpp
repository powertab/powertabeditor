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

#include <sstream>

#include "appinfo.h"
#include <util/version.h>

namespace AppInfo
{
    const char *BUG_TRACKER_URL =
        "https://github.com/powertab/powertabeditor/issues/";
    const char *TRANSLATIONS_URL =
        "https://hosted.weblate.org/engage/powertabeditor";
    const char *APPLICATION_NAME = "Power Tab Editor";
    const char *APPLICATION_VERSION = "2.0.22";

#if defined(Q_OS_WIN)
    const char *ORGANIZATION_NAME = "Power Tab";
    const char *APPLICATION_ID = "Power Tab Editor";
#else
    const char *ORGANIZATION_NAME = "powertab";
    const char *APPLICATION_ID = "powertabeditor";
#endif

    std::string makeApplicationName(void)
    {
        std::stringstream ss;

        ss << APPLICATION_NAME << " "
           << APPLICATION_VERSION << " "
           << Version::get();

        return ss.str();
    }
}
