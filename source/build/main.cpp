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
 
#include <app/appinfo.h>
#include <app/paths.h>
#include <app/powertabeditor.h>
#include <app/settings.h>
#include <app/settingsmanager.h>
#include <boost/program_options.hpp>
#include <csignal>
#include <dialogs/crashdialog.h>
#include <exception>
#include <iostream>
#include <QApplication>
#include <QFileOpenEvent>
#include <QLocalServer>
#include <QLocalSocket>
#include <string>
#include <withershins.hpp>

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif

static void displayError(const std::string &reason)
{
    std::string message = reason;

    // Generate a stack trace.
    std::vector<withershins::frame> trace = withershins::trace();
    for (const withershins::frame &frame : trace)
    {
        message += '\n';
        message += frame.module_name();

        if (!frame.file_name().empty())
        {
            message += " (";
            message += frame.file_name();
            message += ':';
            message += std::to_string(frame.line_number());
            message += ')';
        }

        message += " [";
        if (!frame.function_name().empty())
            message += frame.function_name();
        else
            message += "???";
        message += ']';
    }

    // If there is no QApplication instance, something went seriously wrong
    // during startup - just dump the error to the console.
    if (!QApplication::instance())
        std::cerr << message << std::endl;
    else
    {
        CrashDialog dialog(QString::fromStdString(message),
                           QApplication::activeWindow());
        dialog.exec();
    }

    std::exit(EXIT_FAILURE);
}

static void terminateHandler()
{
    std::string message;

    // If an exception was thrown, grab its message.
    std::exception_ptr eptr = std::current_exception();
    if (eptr)
    {
        message = "Unhandled exception: ";
        try
        {
            std::rethrow_exception(eptr);
        }
        catch (const std::exception &e)
        {
            message += e.what();
        }
    }
    else
        message = "Program terminated unexpectedly";

    displayError(message);
}

static void signalHandler(int /*signal*/)
{
    displayError("Segmentation fault");
}

class Application : public QApplication
{
public:
    Application(int &argc, char **argv) : QApplication(argc, argv)
    {
    }

protected:
    virtual bool event(QEvent *event) override
    {
        switch (event->type())
        {
            // Forward file open requests to the application (e.g. double
            // clicking a file on OSX).
            case QEvent::FileOpen:
            {
                auto app = dynamic_cast<PowerTabEditor *>(activeWindow());
                Q_ASSERT(app);

                app->openFiles({
                    static_cast<QFileOpenEvent *>(event)->file()
                });
                return true;
            }
            default:
                return QApplication::event(event);
        }
    }
};

int main(int argc, char *argv[])
{
    // Register handlers for unhandled exceptions and segmentation faults.
    std::set_terminate(terminateHandler);
    std::signal(SIGSEGV, signalHandler);

    Application a(argc, argv);

    // Set the app information (used by e.g. QSettings).
    QCoreApplication::setOrganizationName(AppInfo::ORGANIZATION_NAME);
    QCoreApplication::setApplicationName(AppInfo::APPLICATION_ID);
    QCoreApplication::setApplicationVersion(AppInfo::APPLICATION_VERSION);

    // Allow QWidget::activateWindow() to bring the application into the
    // foreground when running on Windows.
#ifdef _WIN32
    AllowSetForegroundWindow(ASFW_ANY);
#endif

    QStringList filesToOpen;

    namespace po = boost::program_options;
    po::options_description desc("Usage: powertabeditor [options] [files...] "
                                 "\nA guitar tablature editor.\n\nOptions");
    try
    {
        desc.add_options()
            ("help,h", "Displays this help.")
            ("version,v", "Displays version information.")
            ("files", po::value<std::vector<std::string>>(),
             "The files to be opened, optionally.");
        po::positional_options_description p;
        p.add("files", -1);
        po::variables_map vm;
        po::store(po::command_line_parser(argc, argv)
                      .options(desc)
                      .positional(p)
                      .run(),
                  vm);
        po::notify(vm);

        if (vm.count("help"))
        {
            std::cout << desc << std::endl;
            return EXIT_SUCCESS;
        }

        if (vm.count("version"))
        {
            std::cout << QCoreApplication::applicationName().toStdString()
                      << " "
                      << QCoreApplication::applicationVersion().toStdString()
                      << std::endl;
            return EXIT_SUCCESS;
        }

        if (vm.count("files"))
        {
            auto files = vm["files"].as<std::vector<std::string>>();
            for (auto &file : files)
                filesToOpen.push_back(QString::fromStdString(file));
        }
    }
    catch(po::error &e)
    {
        std::cerr << "Error: " << e.what() << std::endl << std::endl;
        std::cerr << desc << std::endl;
        return EXIT_FAILURE;
    }

    {
        SettingsManager settings_manager;
        settings_manager.load(Paths::getConfigDir());

        auto settings = settings_manager.getReadHandle();
        bool single_window_mode = !settings->get(Settings::OpenFilesInNewWindow);

        // If an instance of the program is already running and we're in
        // single-window mode, tell the running instance to open the files in
        // new tabs.
        if (!filesToOpen.empty() && single_window_mode)
        {
            QLocalSocket socket;
            socket.connectToServer(QCoreApplication::applicationFilePath(),
                                   QIODevice::WriteOnly);
            if (socket.waitForConnected(500))
            {
                QTextStream out(&socket);
                for (const QString &file : filesToOpen)
                    out << file << "\n";
                socket.waitForBytesWritten();
                return EXIT_SUCCESS;
            }
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
        QStringList files;

        while (!in.atEnd())
            files.push_back(in.readLine());

        program.openFiles(files);
        program.showNormal();
        program.activateWindow();

        delete socket;
    });

    server.listen(QCoreApplication::applicationFilePath());

    // Launch the application.
    program.show();
    program.openFiles(filesToOpen);

    return a.exec();
}
