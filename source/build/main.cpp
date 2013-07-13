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
  
#include <QApplication>
#include <app/powertabeditor.h>
#include <app/options.h>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    // set app information
    QCoreApplication::setOrganizationName("Power Tab");
    QCoreApplication::setApplicationName("Power Tab Editor");
    QCoreApplication::setApplicationVersion("2.0");

    Options options;

    if (options.parse(QCoreApplication::arguments()))
    {
        PowerTabEditor w;
        w.show();
        w.openFiles(options.filesToOpen());

        return a.exec();
    }
    else
    {
        return EXIT_FAILURE;
    }
}
