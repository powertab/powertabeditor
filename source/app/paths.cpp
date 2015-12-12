/*
  * Copyright (C) 2015 Cameron White
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

#include "paths.h"

#include <boost/filesystem/detail/utf8_codecvt_facet.hpp>
#include <QString>
#include <QStandardPaths>

static boost::filesystem::detail::utf8_codecvt_facet theUTF8;

/// Convert a QString to a boost::filesystem::path.
static boost::filesystem::path fromQString(const QString &str)
{
    return boost::filesystem::path(str.toStdString(), theUTF8);
}

namespace Paths
{
path getConfigDir()
{
    return fromQString(
        QStandardPaths::writableLocation(QStandardPaths::ConfigLocation));
}
}