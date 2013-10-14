/*
  * Copyright (C) 2013 Cameron White
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

#include "options.h"

#include <QCoreApplication>
#include <QDebug>
#include <QStringList>
#include "tclap/CmdLine.h"

Options::Options()
{
}

bool Options::parse(const QStringList &argList)
{
    using namespace TCLAP;

    try
    {
        CmdLine cmd("A guitar tablature editor.", ' ',
                    QCoreApplication::applicationVersion().toStdString());

        UnlabeledMultiArg<std::string> files("files",
                                             "Names of files to be opened.",
                                             false, "FILE");
        cmd.add(files);

        std::vector<std::string> args = toStdStringList(argList);
        cmd.parse(args);

        myFilenames = files.getValue();

        return true;
    }
    catch (ArgException &e)
    {
        qDebug() << "Error: " << QString::fromStdString(e.error()) <<
                    " for arg " << QString::fromStdString(e.argId());
        return false;
    }
}

std::vector<std::string> Options::filesToOpen()
{
    return myFilenames;
}

std::vector<std::string> Options::toStdStringList(const QStringList &list)
{
    std::vector<std::string> vec;
    vec.reserve(list.length());

    for (const QString &str : list)
        vec.push_back(str.toStdString());

    return vec;
}
