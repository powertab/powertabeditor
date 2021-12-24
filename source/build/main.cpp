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
#include <csignal>
#include <dialogs/crashdialog.h>
#include <exception>
#include <iostream>
#include <QApplication>
#include <QCommandLineParser>
#include <QFileOpenEvent>
#include <QLibraryInfo>
#include <QLocalServer>
#include <QLocalSocket>
#include <QTranslator>
#include <string>

#ifdef __APPLE__
#define BOOST_STACKTRACE_GNU_SOURCE_NOT_REQUIRED
#endif
#include <boost/stacktrace.hpp>

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif

static void displayError(const std::string &reason)
{
    std::string message = reason;
#if BOOST_VERSION >= 106900
    message += boost::stacktrace::to_string(
        boost::stacktrace::stacktrace());
#else
    for (auto &&frame : boost::stacktrace::stacktrace())
    {
        message += boost::stacktrace::to_string(frame);
        message += "\n";
    }
#endif

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

static void
loadTranslations(QApplication &app, QTranslator &qt_translator,
                 QTranslator &ptb_translator)
{
    QLocale locale;
    qDebug() << "Finding translations for locale" << locale
             << "with UI languages" << locale.uiLanguages();

    for (auto &&path : Paths::getTranslationDirs())
    {
        QString dir = Paths::toQString(path);
        qDebug() << "  - Checking" << dir;

        if (ptb_translator.isEmpty() &&
            ptb_translator.load(locale, QStringLiteral("powertabeditor"),
                                QStringLiteral("_"), dir))
        {
#if (QT_VERSION >= QT_VERSION_CHECK(5,15,0))
            qDebug() << "Loaded application translations from"
                     << ptb_translator.filePath();
#else
            qDebug() << "Loaded application translations";
#endif
            app.installTranslator(&ptb_translator);
        }

        if (qt_translator.isEmpty() &&
            qt_translator.load(locale, QStringLiteral("qt"),
                               QStringLiteral("_"), dir))
        {
#if (QT_VERSION >= QT_VERSION_CHECK(5,15,0))
            qDebug() << "Loaded Qt translations from"
                     << qt_translator.filePath();
#else
            qDebug() << "Loaded Qt translations";
#endif
            app.installTranslator(&qt_translator);
        }
    }
}

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

    QTranslator qt_translator;
    QTranslator ptb_translator;
    loadTranslations(a, qt_translator, ptb_translator);

    // Allow QWidget::activateWindow() to bring the application into the
    // foreground when running on Windows.
#ifdef _WIN32
    AllowSetForegroundWindow(ASFW_ANY);
#endif

    // Parse command line arguments.
    QStringList files_to_open;
    {
        QCommandLineParser parser;
        parser.setApplicationDescription(QCoreApplication::translate(
            "PowerTabEditor", "A guitar tablature editor."));
        parser.addHelpOption();
        parser.addVersionOption();
        parser.addPositionalArgument(
            QStringLiteral("files"),
            QCoreApplication::translate("PowerTabEditor",
                                        "The files to be opened"),
            QStringLiteral("[files...]"));
        parser.process(a);

        files_to_open = parser.positionalArguments();
    }

    {
        SettingsManager settings_manager;
        settings_manager.load(Paths::getConfigDir());

        auto settings = settings_manager.getReadHandle();
        bool single_window_mode = !settings->get(Settings::OpenFilesInNewWindow);

        // If an instance of the program is already running and we're in
        // single-window mode, tell the running instance to open the files in
        // new tabs.
        if (!files_to_open.empty() && single_window_mode)
        {
            QLocalSocket socket;
            socket.connectToServer(AppInfo::APPLICATION_ID,
                                   QIODevice::WriteOnly);
            if (socket.waitForConnected(500))
            {
                {
                    QTextStream out(&socket);
                    for (const QString &file : files_to_open)
                        out << file << "\n";
                }

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
        if (program.isMinimized())
            program.showNormal();
        program.activateWindow();

        delete socket;
    });

    server.listen(AppInfo::APPLICATION_ID);

    // Launch the application.
    program.show();
    program.openFiles(files_to_open);

    return a.exec();
}
