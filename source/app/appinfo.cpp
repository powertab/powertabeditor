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

#include "appinfo.h"

#include <QCoreApplication>
#include <QtGlobal>

namespace AppInfo
{
    const char *BUG_TRACKER_URL =
        "https://github.com/powertab/powertabeditor/issues/";
    const char *APPLICATION_NAME = "Power Tab Editor";
    const char *APPLICATION_VERSION = "2.0";

#if defined(Q_OS_WIN)
    const char *ORGANIZATION_NAME = "Power Tab";
    const char *APPLICATION_ID = "Power Tab Editor";
#else
    const char *ORGANIZATION_NAME = "powertab";
    const char *APPLICATION_ID = "powertabeditor";
#endif

    std::string getAbsolutePath(const char *relative_path)
    {
        std::string path = QCoreApplication::applicationDirPath().toStdString();
        path += "/";
        path += relative_path;
        return path;
    }
}
