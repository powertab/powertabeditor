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
 
#include <app/powertabeditor.h>
#include <app/options.h>
#include <app/settings.h>
#include <QApplication>
#include <QLocalServer>
#include <QLocalSocket>
#include <QSettings>

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    // Set the app information (used by e.g. QSettings).
    QCoreApplication::setOrganizationName("Power Tab");
    QCoreApplication::setApplicationName("Power Tab Editor");
    QCoreApplication::setApplicationVersion("2.0");

    // Allow QWidget::activateWindow() to bring the application into the
    // foreground when running on Windows.
#ifdef _WIN32
    AllowSetForegroundWindow(ASFW_ANY);
#endif

    Options options;
    if (options.parse(QCoreApplication::arguments()))
    {
        QSettings settings;

        // If an instance of the program is already running and we're in
        // single-window mode, tell the running instance to open the files in
        // new tabs.
        if (!options.filesToOpen().empty() &&
            !settings.value(Settings::GENERAL_OPEN_IN_NEW_WINDOW,
                            Settings::GENERAL_OPEN_IN_NEW_WINDOW_DEFAULT).toBool())
        {
            QLocalSocket socket;
            socket.connectToServer(QCoreApplication::applicationFilePath(),
                                   QIODevice::WriteOnly);
            if (socket.waitForConnected(500))
            {
                QTextStream out(&socket);
                for (const std::string &file : options.filesToOpen())
                    out << QString::fromStdString(file) << "\n";
                socket.waitForBytesWritten();
                return EXIT_SUCCESS;
            }
        }

        // Otherwise, launch a new window.
        PowerTabEditor program;

        // Set up a server to listen for messages about new files being opened.
        QLocalServer server;
        QObject::connect(&server, &QLocalServer::newConnection, [&]() {
            QLocalSocket *socket = server.nextPendingConnection();
            if (!socket->canReadLine())
                socket->waitForReadyRead();

            QTextStream in(socket);
            std::vector<std::string> files;

            while (!in.atEnd())
                files.push_back(in.readLine().toStdString());

            program.openFiles(files);
            program.showNormal();
            program.activateWindow();

            delete socket;
        });

        server.listen(QCoreApplication::applicationFilePath());

        // Launch the application.
        program.show();
        program.openFiles(options.filesToOpen());

        return a.exec();
    }
    else
    {
        return EXIT_FAILURE;
    }
}
