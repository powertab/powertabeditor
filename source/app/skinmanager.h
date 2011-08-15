/*
  * Copyright (C) 2011 Cameron White
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
  
#ifndef SKINMANAGER_H
#define SKINMANAGER_H

#include <QString>

class SkinManager
{
public:
    SkinManager();

    void reload();

    QString getDocumentTabStyle() const;
    QString getToolboxTabStyle() const;
    QString getToolboxPageStyle() const;
    QString getMixerStyle() const;

    static QStringList availableSkins();

private:
    static QString skinDir();

    QString documentTabStyle;
    QString toolboxTabStyle, toolboxPageStyle;
    QString mixerStyle;

    QString readSegment(const QString& skinname, const QString& filename);
};

#endif // SKINMANAGER_H
