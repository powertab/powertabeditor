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
  
#include "powertabeditor.h"

#include <actions/addbarline.h>
#include <actions/adddynamic.h>
#include <actions/addnote.h>
#include <actions/addnoteproperty.h>
#include <actions/addplayerchange.h>
#include <actions/addpositionproperty.h>
#include <actions/addrehearsalsign.h>
#include <actions/addsystem.h>
#include <actions/addtappedharmonic.h>
#include <actions/addtrill.h>
#include <actions/adjustlinespacing.h>
#include <actions/editbarline.h>
#include <actions/editkeysignature.h>
#include <actions/editnoteduration.h>
#include <actions/edittabnumber.h>
#include <actions/removedynamic.h>
#include <actions/removenoteproperty.h>
#include <actions/removeplayerchange.h>
#include <actions/removepositionproperty.h>
#include <actions/removerehearsalsign.h>
#include <actions/removesystem.h>
#include <actions/removetappedharmonic.h>
#include <actions/removetrill.h>
#include <actions/undomanager.h>

#include <app/caret.h>
#include <app/command.h>
#include <app/documentmanager.h>
#include <app/pubsub/scorelocationpubsub.h>
#include <app/recentfiles.h>
#include <app/scorearea.h>
#include <app/settings.h>

#include <boost/bind.hpp>
#include <boost/foreach.hpp>
#include <boost/timer.hpp>

#include <dialogs/barlinedialog.h>
#include <dialogs/dynamicdialog.h>
#include <dialogs/gotobarlinedialog.h>
#include <dialogs/gotorehearsalsigndialog.h>
#include <dialogs/keyboardsettingsdialog.h>
#include <dialogs/keysignaturedialog.h>
#include <dialogs/playerchangedialog.h>
#include <dialogs/rehearsalsigndialog.h>
#include <dialogs/tappedharmonicdialog.h>
#include <dialogs/trilldialog.h>

#include <formats/fileformatmanager.h>

#include <QCoreApplication>
#include <QDebug>
#include <QFileDialog>
#include <QFontDatabase>
#include <QKeyEvent>
#include <QMenuBar>
#include <QMessageBox>
#include <QSettings>

#include <score/utils.h>
#include <sigfwd/sigfwd.hpp>

PowerTabEditor::PowerTabEditor() :
    QMainWindow(0),
    myDocumentManager(new DocumentManager()),
    myFileFormatManager(new FileFormatManager()),
    myUndoManager(new UndoManager()),
    myPreviousDirectory(QSettings().value(Settings::APP_PREVIOUS_DIRECTORY,
                                          QDir::homePath()).toString()),
    myRecentFiles(NULL),
    myActiveDurationType(Position::EighthNote),
    myTabWidget(NULL)
#if 0
    mixerList(new QStackedWidget),
    playbackToolbarList(new QStackedWidget),
    settingsPubSub(new SettingsPubSub()),
    tuningDictionary(new TuningDictionary())
#endif
{
    this->setWindowIcon(QIcon(":icons/app_icon.png"));

    // Load the music notation font.
    QFontDatabase::addApplicationFont(":fonts/emmentaler-13.otf");
    // Load the tab note font.
    QFontDatabase::addApplicationFont(":fonts/LiberationSans-Regular.ttf");

#if 0
    skinManager.reset(new SkinManager);
#endif

    connect(myUndoManager.get(), SIGNAL(redrawNeeded(int)), this,
            SLOT(redrawSystem(int)));
    connect(myUndoManager.get(), SIGNAL(fullRedrawNeeded()), this,
            SLOT(redrawScore()));
#if 0
    connect(myUndoManager.get(), SIGNAL(cleanChanged(bool)), this,
            SLOT(updateModified(bool)));
#endif

    createCommands();
    createMenus();
    myRecentFiles = new RecentFiles(myRecentFilesMenu, this);
    connect(myRecentFiles, SIGNAL(fileSelected(QString)), this,
            SLOT(openFile(QString)));
    createTabArea();

#if 0
    isPlaying = false;
#endif

    setMinimumSize(800, 600);
    setWindowState(Qt::WindowMaximized);
    setWindowTitle(getApplicationName());

#if 0
    QSplitter* playbackArea = new QSplitter(Qt::Vertical);
    playbackArea->addWidget(tabWidget);
    playbackArea->addWidget(playbackToolbarList.get());

    horSplitter = new QSplitter();
    horSplitter->setOrientation(Qt::Horizontal);

    toolBox = new Toolbox(this, skinManager);
    horSplitter->addWidget(toolBox);
    horSplitter->addWidget(playbackArea);

    vertSplitter = new QSplitter();
    vertSplitter->setOrientation(Qt::Vertical);

    vertSplitter->addWidget(horSplitter);

    mixerList->setMinimumHeight(150);
    vertSplitter->addWidget(mixerList.get());

    setCentralWidget(vertSplitter);

    // Install a top-level event filter to catch keyboard events (i.e. for
    // entering tab numbers) regardless of which widget has focus.
    installEventFilter(this);

    tuningDictionary->loadInBackground();
#else
    setCentralWidget(myTabWidget);
#endif
}

PowerTabEditor::~PowerTabEditor()
{
}

void PowerTabEditor::openFiles(const std::vector<std::string> &files)
{
    BOOST_FOREACH(const std::string &file, files)
    {
        openFile(QString::fromStdString(file));
    }
}

void PowerTabEditor::createNewDocument()
{
    myDocumentManager->addDefaultDocument();
    setupNewTab();
}

void PowerTabEditor::openFile(QString filename)
{
    if (filename.isEmpty())
    {
        filename = QFileDialog::getOpenFileName(this, tr("Open"),
                myPreviousDirectory,
                QString::fromStdString(myFileFormatManager->importFileFilter()));
    }

    if (filename.isEmpty())
        return;

    boost::timer timer;
    qDebug() << "Opening file: " << filename;

    QFileInfo fileInfo(filename);
    boost::optional<FileFormat> format = myFileFormatManager->findFormat(
                fileInfo.suffix().toStdString());

    if (!format)
    {
        QMessageBox::warning(this, tr("Error Opening File"),
                             tr("Unsupported file type."));
        return;
    }

    Document &doc = myDocumentManager->addDocument();
    if (myFileFormatManager->importFile(doc.getScore(), filename.toStdString(),
                                        *format, this))
    {
        qDebug() << "File loaded in" << timer.elapsed() << "seconds";

        doc.setFilename(filename.toStdString());
        setPreviousDirectory(filename);
        myRecentFiles->add(filename);
        setupNewTab();
    }
    else
    {
        myDocumentManager->removeDocument(
                    myDocumentManager->getCurrentDocumentIndex());
    }
}

void PowerTabEditor::switchTab(int index)
{
    myDocumentManager->setCurrentDocumentIndex(index);
#if 0
    mixerList->setCurrentIndex(index);
    playbackToolbarList->setCurrentIndex(index);
#endif
    myUndoManager->setActiveStackIndex(index);

    updateWindowTitle();
}

bool PowerTabEditor::closeTab(int index)
{
    // Prompt to save modified documents.
    if (isWindowModified())
    {
        QMessageBox msg(this);
        msg.setWindowTitle(tr("Close Document"));
        msg.setText(tr("The document has been modified."));
        msg.setInformativeText(tr("Do you want to save your changes?"));
        msg.setStandardButtons(QMessageBox::Save | QMessageBox::Discard |
                               QMessageBox::Cancel);
        msg.setDefaultButton(QMessageBox::Save);

        const int ret = msg.exec();
        if (ret == QMessageBox::Save)
        {
            if (!saveFileAs())
                return false;
        }
        else if (ret == QMessageBox::Cancel)
            return false;
    }

#if 0
    if (getScoreArea(index)->getCaret()->isInPlaybackMode())
    {
        startStopPlayback();
    }
#endif

    myUndoManager->removeStack(index);
    myDocumentManager->removeDocument(index);
    delete myTabWidget->widget(index);
#if 0
    mixerList->removeWidget(mixerList->widget(index));
    playbackToolbarList->removeWidget(playbackToolbarList->widget(index));
#endif
    // Get the index of the tab that we will now switch to.
    const int currentIndex = myTabWidget->currentIndex();

    myUndoManager->setActiveStackIndex(currentIndex);
#if 0
    mixerList->setCurrentIndex(currentIndex);
    playbackToolbarList->setCurrentIndex(currentIndex);
#endif
    myDocumentManager->setCurrentDocumentIndex(currentIndex);

    onDocumentOpenedOrClosed(currentIndex != -1);
    return true;
}

bool PowerTabEditor::closeCurrentTab()
{
    return closeTab(myDocumentManager->getCurrentDocumentIndex());
}

bool PowerTabEditor::saveFileAs()
{
    const QString filter(QString::fromStdString(
                             myFileFormatManager->exportFileFilter()));
    QString path = QFileDialog::getSaveFileName(this, tr("Save As"),
                                                myPreviousDirectory, filter);

    if (!path.isEmpty())
    {
        // If the user didn't type the extension, add it in.
        QFileInfo info(path);
        QString extension = info.suffix();
        if (extension.isEmpty())
        {
            extension = "pt2";
            path += "." + extension;
        }

        boost::optional<FileFormat> format = myFileFormatManager->findFormat(
                    extension.toStdString());
        if (!format)
        {
            QMessageBox::warning(this, tr("Error Saving File"),
                                 tr("Unsupported file type."));
            return false;
        }

        const std::string newPath = path.toStdString();
        Document &doc = myDocumentManager->getCurrentDocument();

        if (myFileFormatManager->exportFile(doc.getScore(), newPath, *format))
        {
            doc.setFilename(newPath);

            // Update window title and tab bar.
            updateWindowTitle();
            const QString fileName = QFileInfo(path).fileName();
            myTabWidget->setTabText(myTabWidget->currentIndex(), fileName);
            myTabWidget->setTabToolTip(myTabWidget->currentIndex(), fileName);

            return true;
        }
    }

    return false;
}

void PowerTabEditor::cycleTab(int offset)
{
    int newIndex = (myTabWidget->currentIndex() + offset) % myTabWidget->count();

    if (newIndex < 0) // Make sure that negative array indices wrap around.
    {
        newIndex += myTabWidget->count();
    }

    myTabWidget->setCurrentIndex(newIndex);
}

void PowerTabEditor::editKeyboardShortcuts()
{
    QList<Command *> registeredCommands = findChildren<Command *>();

    KeyboardSettingsDialog dialog(this, registeredCommands);
    dialog.exec();
}

void PowerTabEditor::editPreferences()
{
#if 0
    PreferencesDialog(this, settingsPubSub, tuningDictionary).exec();
    skinManager->reload();
#else
    Q_ASSERT(false);
#endif
}

void PowerTabEditor::redrawSystem(int index)
{
    getScoreArea()->redrawSystem(index);
    updateCommands();
}

void PowerTabEditor::redrawScore()
{
    getScoreArea()->renderDocument(myDocumentManager->getCurrentDocument(),
                                   Staff::GuitarView);
    updateCommands();
}

void PowerTabEditor::moveCaretToStart()
{
    getCaret().moveToStartPosition();
}

void PowerTabEditor::moveCaretRight()
{
    getCaret().moveHorizontal(1);
}

void PowerTabEditor::moveCaretLeft()
{
    getCaret().moveHorizontal(-1);
}

void PowerTabEditor::moveCaretDown()
{
    getCaret().moveVertical(1);
}

void PowerTabEditor::moveCaretUp()
{
    getCaret().moveVertical(-1);
}

void PowerTabEditor::moveCaretToEnd()
{
    getCaret().moveToEndPosition();
}

void PowerTabEditor::moveCaretToFirstSection()
{
    getCaret().moveToFirstSystem();
}

void PowerTabEditor::moveCaretToNextSection()
{
    getCaret().moveSystem(1);
}

void PowerTabEditor::moveCaretToPrevSection()
{
    getCaret().moveSystem(-1);
}

void PowerTabEditor::moveCaretToLastSection()
{
    getCaret().moveToLastSystem();
}

void PowerTabEditor::moveCaretToNextStaff()
{
    getCaret().moveStaff(1);
}

void PowerTabEditor::moveCaretToPrevStaff()
{
    getCaret().moveStaff(-1);
}

void PowerTabEditor::moveCaretToNextBar()
{
    getCaret().moveToNextBar();
}

void PowerTabEditor::moveCaretToPrevBar()
{
    getCaret().moveToPrevBar();
}

void PowerTabEditor::gotoBarline()
{
    GoToBarlineDialog dialog(this, getLocation().getScore());

    if (dialog.exec() == QDialog::Accepted)
    {
        ScoreLocation location(dialog.getLocation());
        getCaret().moveToSystem(location.getSystemIndex(), true);
        getCaret().moveToPosition(location.getPositionIndex());
    }
}

void PowerTabEditor::gotoRehearsalSign()
{
    GoToRehearsalSignDialog dialog(this, getLocation().getScore());

    if (dialog.exec() == QDialog::Accepted)
    {
        ScoreLocation location(dialog.getLocation());
        getCaret().moveToSystem(location.getSystemIndex(), true);
        getCaret().moveToPosition(location.getPositionIndex());
    }
}

void PowerTabEditor::insertSystemAtEnd()
{
    insertSystem(getLocation().getScore().getSystems().size());
}

void PowerTabEditor::insertSystemBefore()
{
    insertSystem(getLocation().getSystemIndex());
}

void PowerTabEditor::insertSystemAfter()
{
    insertSystem(getLocation().getSystemIndex() + 1);
}

void PowerTabEditor::removeCurrentSystem()
{
    ScoreLocation &location = getLocation();
    myUndoManager->push(new RemoveSystem(location.getScore(),
                                         location.getSystemIndex()),
                        UndoManager::AFFECTS_ALL_SYSTEMS);
}

void PowerTabEditor::updateNoteDuration(Position::DurationType duration)
{
    // Set the duration for future notes that are added.
    myActiveDurationType = duration;

    if (!getLocation().getSelectedPositions().empty())
    {
        myUndoManager->push(new EditNoteDuration(getLocation(), duration),
                            getLocation().getSystemIndex());
    }
}

void PowerTabEditor::changeNoteDuration(bool increase)
{
    if (getLocation().getSelectedPositions().empty())
    {
        if (increase && myActiveDurationType == Position::WholeNote)
            return;
        if (!increase && myActiveDurationType == Position::SixtyFourthNote)
            return;

        updateNoteDuration(static_cast<Position::DurationType>(
                               increase ? myActiveDurationType >> 1 :
                                          myActiveDurationType << 1));
        return;
    }

    myUndoManager->beginMacro(tr("Edit Note Duration"));

    // Increase the duration of each selected position.
    BOOST_FOREACH(const Position *pos, getLocation().getSelectedPositions())
    {
        ScoreLocation location(getLocation());
        location.setPositionIndex(pos->getPosition());
        location.setSelectionStart(pos->getPosition());

        Position::DurationType duration = pos->getDurationType();
        if (increase && duration == Position::WholeNote)
            continue;
        if (!increase && duration == Position::SixtyFourthNote)
            continue;

        myUndoManager->push(new EditNoteDuration(location,
                static_cast<Position::DurationType>(increase ? duration >> 1 :
                                                               duration << 1)),
                            location.getSystemIndex());
    }

    myUndoManager->endMacro();
}

void PowerTabEditor::editRehearsalSign()
{
    const ScoreLocation &location = getLocation();
    const Barline *barline = location.getBarline();
    Q_ASSERT(barline);

    if (barline->hasRehearsalSign())
    {
        myUndoManager->push(new RemoveRehearsalSign(location),
                            UndoManager::AFFECTS_ALL_SYSTEMS);
    }
    else
    {
        RehearsalSignDialog dialog(this);

        if (dialog.exec() == QDialog::Accepted)
        {
            myUndoManager->push(new AddRehearsalSign(location,
                                                     dialog.getDescription()),
                                UndoManager::AFFECTS_ALL_SYSTEMS);
        }
        else
            myRehearsalSignCommand->setChecked(false);
    }
}

void PowerTabEditor::editKeySignatureFromCaret()
{
    editKeySignature(getLocation());
}

void PowerTabEditor::insertStandardBarline()
{
    ScoreLocation &location = getLocation();
    myUndoManager->push(new AddBarline(location,
                                       Barline(location.getPositionIndex(),
                                               Barline::SingleBar)),
                        location.getSystemIndex());
}

void PowerTabEditor::editBarlineFromCaret()
{
    editBarline(getLocation());
}

void PowerTabEditor::editDynamic()
{
    ScoreLocation &location = getLocation();
    const Dynamic *dynamic = ScoreUtils::findByPosition(
                location.getStaff().getDynamics(), location.getPositionIndex());

    if (dynamic)
    {
        myUndoManager->push(new RemoveDynamic(location),
                            location.getSystemIndex());
    }
    else
    {
        DynamicDialog dialog(this);
        if (dialog.exec() == QDialog::Accepted)
        {
            Dynamic dynamic(location.getPositionIndex(),
                            dialog.getVolumeLevel());

            myUndoManager->push(new AddDynamic(location, dynamic),
                                location.getSystemIndex());
        }
        else
            myDynamicCommand->setChecked(false);
    }
}

void PowerTabEditor::editTappedHarmonic()
{
    const ScoreLocation &location = getLocation();
    const Note *note = location.getNote();
    Q_ASSERT(note);

    if (note->hasTappedHarmonic())
        myUndoManager->push(new RemoveTappedHarmonic(location),
                            location.getSystemIndex());
    else
    {
        TappedHarmonicDialog dialog(this, note->getFretNumber());
        if (dialog.exec() == QDialog::Accepted)
        {
            myUndoManager->push(new AddTappedHarmonic(location,
                                                      dialog.getTappedFret()),
                                location.getSystemIndex());
        }
        else
            myTappedHarmonicCommand->setChecked(false);
    }
}

void PowerTabEditor::editTrill()
{
    const ScoreLocation &location = getLocation();
    const Note *note = location.getNote();
    Q_ASSERT(note);

    if (note->hasTrill())
        myUndoManager->push(new RemoveTrill(location), location.getSystemIndex());
    else
    {
        TrillDialog dialog(this, note->getFretNumber());
        if (dialog.exec() == QDialog::Accepted)
        {
            myUndoManager->push(new AddTrill(location, dialog.getTrilledFret()),
                                location.getSystemIndex());
        }
        else
            myTrillCommand->setChecked(false);
    }
}

void PowerTabEditor::editPlayerChange()
{
    const ScoreLocation &location = getLocation();

    // Note that adding/removing a player change affects multiple systems,
    // since the standard notation will need to be updated if the new player
    // has a different tuning.
    if (ScoreUtils::findByPosition(location.getSystem().getPlayerChanges(),
                                   location.getPositionIndex()))
    {
        myUndoManager->push(new RemovePlayerChange(location),
                            UndoManager::AFFECTS_ALL_SYSTEMS);
    }
    else
    {
        // Initialize the dialog with the current staves for each player.
        const PlayerChange *currentPlayers =
                ScoreUtils::getCurrentPlayers(location.getScore(),
                                              location.getSystemIndex(),
                                              location.getPositionIndex());

        PlayerChangeDialog dialog(this, location.getScore(),
                                  location.getSystem(), currentPlayers);
        if (dialog.exec() == QDialog::Accepted)
        {
            myUndoManager->push(
                        new AddPlayerChange(location, dialog.getPlayerChange()),
                        UndoManager::AFFECTS_ALL_SYSTEMS);

        }
        else
            myPlayerChangeCommand->setChecked(false);
    }
}

bool PowerTabEditor::eventFilter(QObject *object, QEvent *event)
{
    ScoreArea *scorearea = getScoreArea();
    if (scorearea && event->type() == QEvent::KeyPress)
    {
        QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);
        if (keyEvent->key() >= Qt::Key_0 && keyEvent->key() <= Qt::Key_9)
        {
            const int number = keyEvent->key() - Qt::Key_0;
            ScoreLocation &location = getLocation();

            // Don't allow inserting notes at the same position as a barline,
            // unless it's the first position of the system.
            if (!location.getBarline() || location.getPositionIndex() == 0)
            {
                // Update the existing note if possible.
                if (location.getNote())
                {
                    myUndoManager->push(new EditTabNumber(location, number),
                                        location.getSystemIndex());
                }
                else
                {
                    myUndoManager->push(
                                new AddNote(location,
                                            Note(location.getString(), number),
                                            myActiveDurationType),
                                location.getSystemIndex());
                }

                return true;
            }
        }
    }

    return QMainWindow::eventFilter(object, event);
}

QString PowerTabEditor::getApplicationName() const
{
    QString name = QString("%1 %2 Beta").arg(
                QCoreApplication::applicationName(),
                QCoreApplication::applicationVersion());

#ifdef SVN_REVISION
    name += QString(" r") + BOOST_STRINGIZE(SVN_REVISION);
#endif

    return name;
}

void PowerTabEditor::updateWindowTitle()
{
    if (myDocumentManager->hasOpenDocuments() &&
        myDocumentManager->getCurrentDocument().hasFilename())
    {
        const Document &doc = myDocumentManager->getCurrentDocument();
        const QString path = QString::fromStdString(doc.getFilename());
        const QString docName = QFileInfo(path).fileName();
        // Need the [*] for using setWindowModified.
        setWindowTitle(docName + "[*] - " + getApplicationName());
    }
    else
        setWindowTitle(getApplicationName());
}

void PowerTabEditor::createCommands()
{
    // File-related commands.
    myNewDocumentCommand = new Command(tr("&New"), "File.New",
                                       QKeySequence::New, this);
    connect(myNewDocumentCommand, SIGNAL(triggered()), this,
            SLOT(createNewDocument()));

    myOpenFileCommand = new Command(tr("&Open..."), "File.Open",
                                    QKeySequence::Open, this);
    connect(myOpenFileCommand, SIGNAL(triggered()), this, SLOT(openFile()));

    myCloseTabCommand = new Command(tr("&Close Tab"), "File.CloseTab",
                                    Qt::CTRL + Qt::Key_W, this);
    connect(myCloseTabCommand, SIGNAL(triggered()), this,
            SLOT(closeCurrentTab()));

    mySaveAsCommand = new Command(tr("Save As..."), "File.SaveAs",
                                  QKeySequence::SaveAs, this);
    connect(mySaveAsCommand, SIGNAL(triggered()), this, SLOT(saveFileAs()));

    myEditShortcutsCommand = new Command(tr("Customize Shortcuts..."),
                                         "File.CustomizeShortcuts",
                                         QKeySequence(), this);
    connect(myEditShortcutsCommand, SIGNAL(triggered()), this,
            SLOT(editKeyboardShortcuts()));

    myEditPreferencesCommand = new Command(tr("&Preferences..."),
                                           "File.Preferences",
                                           QKeySequence::Preferences, this);
    connect(myEditPreferencesCommand, SIGNAL(triggered()), this,
            SLOT(editPreferences()));

    myExitCommand = new Command(tr("&Quit"), "File.Quit", QKeySequence::Quit,
                                this);
    connect(myExitCommand, SIGNAL(triggered()), this, SLOT(close()));

    // Undo / Redo actions.
    myUndoAction = myUndoManager->createUndoAction(this, tr("&Undo"));
    myUndoAction->setShortcuts(QKeySequence::Undo);

    myRedoAction = myUndoManager->createRedoAction(this, tr("&Redo"));
    myRedoAction->setShortcuts(QKeySequence::Redo);

#if 0
    // Copy/Paste
    cutAct = new Command(tr("Cut"), "Edit.Cut", QKeySequence::Cut, this);
    connect(cutAct, SIGNAL(triggered()), this, SLOT(cutSelectedNotes()));

    copyAct = new Command(tr("Copy"), "Edit.Copy", QKeySequence::Copy, this);
    connect(copyAct, SIGNAL(triggered()), this, SLOT(copySelectedNotes()));

    pasteAct = new Command(tr("Paste"), "Edit.Paste", QKeySequence::Paste, this);
    connect(pasteAct, SIGNAL(triggered()), this, SLOT(doPaste()));

    // File Information
    fileInfoAct = new Command(tr("File Information..."), "Edit.FileInformation",
                              QKeySequence(), this);
    connect(fileInfoAct, SIGNAL(triggered()), this, SLOT(openFileInformation()));

    // Playback-related actions
    playPauseAct = new Command(tr("Play"), "PlayPause.Play", Qt::Key_Space, this);
    connect(playPauseAct, SIGNAL(triggered()), this, SLOT(startStopPlayback()));

#endif
    // Section navigation actions.
    myFirstSectionCommand = new Command(tr("First Section"),
                                        "Section.FirstSection",
                                        QKeySequence::MoveToStartOfDocument,
                                        this);
    connect(myFirstSectionCommand, SIGNAL(triggered()), this,
            SLOT(moveCaretToFirstSection()));

    myNextSectionCommand = new Command(tr("Next Section"),
                                       "Section.NextSection",
                                       QKeySequence::MoveToNextPage, this);
    connect(myNextSectionCommand, SIGNAL(triggered()), this,
            SLOT(moveCaretToNextSection()));

    myPrevSectionCommand = new Command(tr("Previous Section"),
                                       "Section.PreviousSection",
                                       QKeySequence::MoveToPreviousPage, this);
    connect(myPrevSectionCommand, SIGNAL(triggered()), this,
            SLOT(moveCaretToPrevSection()));

    myLastSectionCommand = new Command(tr("Last Section"),
                                       "Section.LastSection",
                                       QKeySequence::MoveToEndOfDocument, this);
    connect(myLastSectionCommand, SIGNAL(triggered()), this,
            SLOT(moveCaretToLastSection()));

#if 0
    shiftForwardAct = new Command(tr("Shift Forward"), "Position.ShiftForward",
                                  QKeySequence(Qt::Key_Insert), this);
    connect(shiftForwardAct, SIGNAL(triggered()), this, SLOT(shiftForward()));

    shiftBackwardAct = new Command(tr("Shift Backward"), "Position.ShiftBackward",
                                   QKeySequence(Qt::SHIFT + Qt::Key_Insert), this);
    connect(shiftBackwardAct, SIGNAL(triggered()), this, SLOT(shiftBackward()));
#endif

    // Position-related actions.
    myStartPositionCommand = new Command(tr("Move to &Start"),
                                         "Staff.MoveToStart",
                                         QKeySequence::MoveToStartOfLine, this);
    connect(myStartPositionCommand, SIGNAL(triggered()), this,
            SLOT(moveCaretToStart()));

    myNextPositionCommand = new Command(tr("&Next Position"),
                                        "Staff.NextPosition",
                                        QKeySequence::MoveToNextChar, this);
    connect(myNextPositionCommand, SIGNAL(triggered()), this,
            SLOT(moveCaretRight()));

    myPrevPositionCommand = new Command(tr("&Previous Position"),
                                        "Staff.PreviousPosition",
                                        QKeySequence::MoveToPreviousChar, this);
    connect(myPrevPositionCommand, SIGNAL(triggered()), this,
            SLOT(moveCaretLeft()));

    myNextStringCommand = new Command(tr("Next String"), "Staff.NextString",
                                      QKeySequence::MoveToNextLine, this);
    connect(myNextStringCommand, SIGNAL(triggered()), this,
            SLOT(moveCaretDown()));

    myPrevStringCommand = new Command(tr("Previous String"),
                                      "Staff.PreviousString",
                                      QKeySequence::MoveToPreviousLine, this);
    connect(myPrevStringCommand, SIGNAL(triggered()), this,
            SLOT(moveCaretUp()));

    myLastPositionCommand = new Command(tr("Move to &End"), "Staff.MoveToEnd",
                                        QKeySequence::MoveToEndOfLine, this);
    connect(myLastPositionCommand, SIGNAL(triggered()), this,
            SLOT(moveCaretToEnd()));

    myNextStaffCommand = new Command(tr("Next Staff"), "Staff.NextStaff",
                                     Qt::ALT + Qt::Key_Down, this);
    connect(myNextStaffCommand, SIGNAL(triggered()), this,
            SLOT(moveCaretToNextStaff()));

    myPrevStaffCommand = new Command(tr("Previous Staff"), "Staff.PreviousStaff",
                                     Qt::ALT + Qt::Key_Up, this);
    connect(myPrevStaffCommand, SIGNAL(triggered()), this,
            SLOT(moveCaretToPrevStaff()));

    myNextBarCommand = new Command(tr("Next Bar"), "Staff.NextBar",
                                   Qt::Key_Tab, this);
    connect(myNextBarCommand, SIGNAL(triggered()), this,
            SLOT(moveCaretToNextBar()));

    myPrevBarCommand = new Command(tr("Previous Bar"), "Staff.PreviousBar",
                                   Qt::SHIFT + Qt::Key_Tab, this);
    connect(myPrevBarCommand, SIGNAL(triggered()), this,
            SLOT(moveCaretToPrevBar()));
#if 0
    // Actions for shifting tab numbers up/down a string
    shiftTabNumUp = new Command(tr("Shift Tab Number Up"), "Position.ShiftTabNumberUp",
                                Qt::CTRL + Qt::Key_Up, this);
    sigfwd::connect(shiftTabNumUp, SIGNAL(triggered()),
                    boost::bind(&PowerTabEditor::shiftTabNumber, this, Position::SHIFT_UP));

    shiftTabNumDown = new Command(tr("Shift Tab Number Down"), "Position.ShiftTabNumberDown",
                                  Qt::CTRL + Qt::Key_Down, this);
    sigfwd::connect(shiftTabNumDown, SIGNAL(triggered()),
                    boost::bind(&PowerTabEditor::shiftTabNumber, this, Position::SHIFT_DOWN));

    clearNoteAct = new Command(tr("Clear Note"), "Position.ClearNote", QKeySequence::Delete, this);
    connect(clearNoteAct, SIGNAL(triggered()), this, SLOT(clearNote()));

    clearCurrentPositionAct = new Command(tr("Clear Position"), "Position.ClearPosition",
                                          QKeySequence::DeleteEndOfWord, this);
    connect(clearCurrentPositionAct, SIGNAL(triggered()), this, SLOT(clearCurrentPosition()));

#endif
    myGoToBarlineCommand = new Command(tr("Go To Barline..."),
                                       "Position.GoToBarline",
                                       Qt::CTRL + Qt::Key_G, this);
    connect(myGoToBarlineCommand, SIGNAL(triggered()), this,
            SLOT(gotoBarline()));

    myGoToRehearsalSignCommand = new Command(tr("Go To Rehearsal Sign..."),
                                             "Position.GoToRehearsalSign",
                                             Qt::CTRL + Qt::Key_H, this);
    connect(myGoToRehearsalSignCommand, SIGNAL(triggered()), this,
            SLOT(gotoRehearsalSign()));
#if 0

    // Text-related actions
    chordNameAct = new Command(tr("Chord Name..."), "Text.ChordName", Qt::Key_C, this);
    chordNameAct->setCheckable(true);
    connect(chordNameAct, SIGNAL(triggered()), this, SLOT(editChordName()));

    // Section-related actions
    increasePositionSpacingAct = new Command(tr("Increase Position Spacing"),
                                             "Section.IncreaseSpacing", Qt::Key_Plus, this);
    sigfwd::connect(increasePositionSpacingAct, SIGNAL(triggered()),
                    boost::bind(&PowerTabEditor::changePositionSpacing, this, 1));

    decreasePositionSpacingAct = new Command(tr("Decrease Position Spacing"),
                                             "Section.DecreaseSpacing", Qt::Key_Minus, this);
    sigfwd::connect(decreasePositionSpacingAct, SIGNAL(triggered()),
                    boost::bind(&PowerTabEditor::changePositionSpacing, this, -1));

#endif
    myInsertSystemAtEndCommand = new Command(tr("Insert System At End"),
                                             "Section.InsertSystemAtEnd",
                                             Qt::Key_N, this);
    connect(myInsertSystemAtEndCommand, SIGNAL(triggered()), this,
            SLOT(insertSystemAtEnd()));

    myInsertSystemBeforeCommand = new Command(tr("Insert System Before"),
                                              "Section.InsertSystemBefore",
                                              QKeySequence(Qt::ALT + Qt::SHIFT +
                                                           Qt::Key_N),this);
    connect(myInsertSystemBeforeCommand, SIGNAL(triggered()), this,
            SLOT(insertSystemBefore()));

    myInsertSystemAfterCommand = new Command(tr("Insert System After"),
                                             "Section.InsertSystemAfter",
                                             QKeySequence(Qt::SHIFT + Qt::Key_N),
                                             this);
    connect(myInsertSystemAfterCommand, SIGNAL(triggered()), this,
            SLOT(insertSystemAfter()));

    myRemoveCurrentSystemCommand = new Command(tr("Remove Current System"),
                                         "Section.RemoveCurrentSystem",
                                         QKeySequence(Qt::CTRL + Qt::SHIFT +
                                                      Qt::Key_N), this);
    connect(myRemoveCurrentSystemCommand, SIGNAL(triggered()), this,
            SLOT(removeCurrentSystem()));

    myIncreaseLineSpacingCommand = new Command(tr("Increase"),
                                               "Section.IncreaseLineSpacing",
                                               QKeySequence(), this);
    sigfwd::connect(myIncreaseLineSpacingCommand, SIGNAL(triggered()),
                    boost::bind(&PowerTabEditor::adjustLineSpacing, this, 1));

    myDecreaseLineSpacingCommand = new Command(tr("Decrease"),
                                               "Section.DecreaseLineSpacing",
                                               QKeySequence(), this);
    sigfwd::connect(myDecreaseLineSpacingCommand, SIGNAL(triggered()),
                    boost::bind(&PowerTabEditor::adjustLineSpacing, this, -1));

    // Note-related actions.
    myNoteDurationGroup = new QActionGroup(this);
    createNoteDurationCommand(myWholeNoteCommand, tr("Whole"), "Note.WholeNote",
                              Position::WholeNote);
    createNoteDurationCommand(myHalfNoteCommand, tr("Half"), "Note.HalfNote",
                              Position::HalfNote);
    createNoteDurationCommand(myQuarterNoteCommand, tr("Quarter"),
                              "Note.QuarterNote", Position::QuarterNote);
    createNoteDurationCommand(myEighthNoteCommand, tr("8th"),
                              "Note.EighthNote", Position::EighthNote);
    createNoteDurationCommand(mySixteenthNoteCommand, tr("16th"),
                              "Note.SixteenthNote", Position::SixteenthNote);
    createNoteDurationCommand(myThirtySecondNoteCommand, tr("32nd"),
                              "Note.ThirtySecondNote", Position::ThirtySecondNote);
    createNoteDurationCommand(mySixtyFourthNoteCommand, tr("64th"),
                              "Note.SixtyFourthNote", Position::SixtyFourthNote);

    myIncreaseDurationCommand = new Command(tr("Increase Duration"),
                                            "Note.IncreaseDuration",
                                            Qt::SHIFT + Qt::Key_Up, this);
    sigfwd::connect(myIncreaseDurationCommand, SIGNAL(triggered()),
                    boost::bind(&PowerTabEditor::changeNoteDuration, this, true));

    myDecreaseDurationCommand = new Command(tr("Decrease Duration"),
                                            "Note.DecreaseDuration",
                                            Qt::SHIFT + Qt::Key_Down, this);
    sigfwd::connect(myDecreaseDurationCommand, SIGNAL(triggered()),
                    boost::bind(&PowerTabEditor::changeNoteDuration, this, false));

    createPositionPropertyCommand(myDottedCommand, tr("Dotted"), "Note.Dotted",
                                  QKeySequence(), Position::Dotted);

    createPositionPropertyCommand(myDoubleDottedCommand, tr("Double Dotted"),
                                  "Note.DoubleDotted", QKeySequence(),
                                  Position::DoubleDotted);
#if 0
    addDotAct = new Command(tr("Add Dot"), "Note.AddDot", Qt::SHIFT + Qt::Key_Right, this);
    removeDotAct = new Command(tr("Remove Dot"), "Note.RemoveDot", Qt::SHIFT + Qt::Key_Left, this);

    tiedNoteAct = new Command(tr("Tied"), "Note.Tied", Qt::Key_Y, this);
    tiedNoteAct->setCheckable(true);
    connect(tiedNoteAct, SIGNAL(triggered()), this, SLOT(editTiedNote()));
#endif
    createNotePropertyCommand(myMutedCommand, tr("Muted"), "Note.Muted",
                              Qt::Key_X, Note::Muted);
    createNotePropertyCommand(myGhostNoteCommand, tr("Ghost Note"),
                              "Note.GhostNote", Qt::Key_G, Note::GhostNote);
#if 0
    fermataAct = new Command(tr("Fermata"), "Note.Fermata", Qt::Key_F, this);
    fermataAct->setCheckable(true);
    connectToggleProperty<Position>(fermataAct, &getSelectedPositions,
                                    &Position::HasFermata, &Position::SetFermata);

#endif
    createPositionPropertyCommand(myLetRingCommand, tr("Let Ring"),
                                  "Note.LetRing", QKeySequence(),
                                  Position::LetRing);
#if 0

    graceNoteAct = new Command(tr("Grace Note"), "Note.GraceNote", QKeySequence(), this);
    graceNoteAct->setCheckable(true);
    connectToggleProperty<Position>(graceNoteAct, &getSelectedPositions,
                                    &Position::IsAcciaccatura, &Position::SetAcciaccatura);

    staccatoNoteAct = new Command(tr("Staccato"), "Note.Staccato", Qt::Key_Z, this);
    staccatoNoteAct->setCheckable(true);
    connectToggleProperty<Position>(staccatoNoteAct, &getSelectedPositions,
                                    &Position::IsStaccato, &Position::SetStaccato);

    marcatoAct = new Command(tr("Accent"), "Note.Accent", Qt::Key_A, this);
    marcatoAct->setCheckable(true);
    connectToggleProperty<Position>(marcatoAct, &getSelectedPositions,
                                    &Position::HasMarcato, &Position::SetMarcato);

    sforzandoAct = new Command(tr("Heavy Accent"), "Note.HeavyAccent", QKeySequence(), this);
    sforzandoAct->setCheckable(true);
    connectToggleProperty<Position>(sforzandoAct, &getSelectedPositions,
                                    &Position::HasSforzando, &Position::SetSforzando);

#endif
    // Octave actions
    createNotePropertyCommand(myOctave8vaCommand, tr("8va"), "Note.Octave8va",
                              QKeySequence(), Note::Octave8va);
    createNotePropertyCommand(myOctave15maCommand, tr("15ma"), "Note.Octave15ma",
                              QKeySequence(), Note::Octave15ma);
    createNotePropertyCommand(myOctave8vbCommand, tr("8vb"), "Note.Octave8vb",
                              QKeySequence(), Note::Octave8vb);
    createNotePropertyCommand(myOctave15mbCommand, tr("15mb"),
                              "Octave15mb.Octave8va", QKeySequence(),
                              Note::Octave15mb);
#if 0
    tripletAct = new Command(tr("Triplet"), "Note.Triplet", Qt::Key_E, this);
    tripletAct->setCheckable(true);
    sigfwd::connect(tripletAct, SIGNAL(triggered()),
                    boost::bind(&PowerTabEditor::editIrregularGrouping, this, true));

    irregularGroupingAct = new Command(tr("Irregular Grouping"), "Note.IrregularGrouping",
                                       Qt::Key_I, this);
    irregularGroupingAct->setCheckable(true);
    connect(irregularGroupingAct, SIGNAL(triggered()), this, SLOT(editIrregularGrouping()));

    // Rest Actions
    restDurationsGroup = new QActionGroup(this);

    wholeRestAct = new Command(tr("Whole"), "Rests.Whole", QKeySequence(), this);
    wholeRestAct->setCheckable(true);
    restDurationsGroup->addAction(wholeRestAct);
    sigfwd::connect(wholeRestAct, SIGNAL(triggered()),
                    boost::bind(&PowerTabEditor::editRest, this, 1));

    halfRestAct = new Command(tr("Half"), "Rests.Half", QKeySequence(), this);
    halfRestAct->setCheckable(true);
    restDurationsGroup->addAction(halfRestAct);
    sigfwd::connect(halfRestAct, SIGNAL(triggered()),
                    boost::bind(&PowerTabEditor::editRest, this, 2));

    quarterRestAct = new Command(tr("Quarter"), "Rests.Quarter", QKeySequence(), this);
    quarterRestAct->setCheckable(true);
    restDurationsGroup->addAction(quarterRestAct);
    sigfwd::connect(quarterRestAct, SIGNAL(triggered()),
                    boost::bind(&PowerTabEditor::editRest, this, 4));

    eighthRestAct = new Command(tr("8th"), "Rests.Eighth", QKeySequence(), this);
    eighthRestAct->setCheckable(true);
    restDurationsGroup->addAction(eighthRestAct);
    sigfwd::connect(eighthRestAct, SIGNAL(triggered()),
                    boost::bind(&PowerTabEditor::editRest, this, 8));

    sixteenthRestAct = new Command(tr("16th"), "Rests.Sixteenth", QKeySequence(), this);
    sixteenthRestAct->setCheckable(true);
    restDurationsGroup->addAction(sixteenthRestAct);
    sigfwd::connect(sixteenthRestAct, SIGNAL(triggered()),
                    boost::bind(&PowerTabEditor::editRest, this, 16));

    thirtySecondRestAct = new Command(tr("32nd"), "Rests.ThirtySecond", QKeySequence(), this);
    thirtySecondRestAct->setCheckable(true);
    restDurationsGroup->addAction(thirtySecondRestAct);
    sigfwd::connect(thirtySecondRestAct, SIGNAL(triggered()),
                    boost::bind(&PowerTabEditor::editRest, this, 32));

    sixtyFourthRestAct = new Command(tr("64th"), "Rests.SixtyFourth", QKeySequence(), this);
    sixtyFourthRestAct->setCheckable(true);
    restDurationsGroup->addAction(sixtyFourthRestAct);
    sigfwd::connect(sixtyFourthRestAct, SIGNAL(triggered()),
                    boost::bind(&PowerTabEditor::editRest, this, 64));

    addRestAct = new Command(tr("Add Rest"), "Rests.AddRest", QKeySequence(Qt::Key_R), this);
    sigfwd::connect(addRestAct, SIGNAL(triggered()),
                    boost::bind(&PowerTabEditor::addRest, this));

#endif
    // Music Symbol Actions
    myRehearsalSignCommand = new Command(tr("Rehearsal Sign..."),
                                         "MusicSymbols.EditRehearsalSign",
                                         Qt::SHIFT + Qt::Key_R, this);
    myRehearsalSignCommand->setCheckable(true);
    connect(myRehearsalSignCommand, SIGNAL(triggered()), this,
            SLOT(editRehearsalSign()));

#if 0
    tempoMarkerAct = new Command(tr("Tempo Marker..."),"MusicSymbols.EditTempoMarker",
                                 Qt::Key_O, this);
    tempoMarkerAct->setCheckable(true);
    connect(tempoMarkerAct, SIGNAL(triggered()), this, SLOT(editTempoMarker()));

#endif
    myKeySignatureCommand = new Command(tr("Edit Key Signature..."),
                                        "MusicSymbols.EditKeySignature",
                                        Qt::Key_K, this);
    connect(myKeySignatureCommand, SIGNAL(triggered()), this,
            SLOT(editKeySignatureFromCaret()));
#if 0
    timeSignatureAct = new Command(tr("Edit Time Signature..."), "MusicSymbols.EditTimeSignature",
                                   Qt::Key_T, this);
    connect(timeSignatureAct, SIGNAL(triggered()), this,
            SLOT(editTimeSignatureFromCaret()));
#endif
    myStandardBarlineCommand = new Command(tr("Insert Standard Barline"),
                                     "MusicSymbols.InsertStandardBarline",
                                     Qt::Key_B, this);
    connect(myStandardBarlineCommand, SIGNAL(triggered()), this,
            SLOT(insertStandardBarline()));

    myBarlineCommand = new Command(tr("Barline..."), "MusicSymbols.EditBarline",
                             Qt::SHIFT + Qt::Key_B, this);
    connect(myBarlineCommand, SIGNAL(triggered()), this,
            SLOT(editBarlineFromCaret()));
#if 0
    musicalDirectionAct = new Command(tr("Musical Direction..."),
            "MusicSymbols.EditMusicalDirection", Qt::SHIFT + Qt::Key_D, this);
    musicalDirectionAct->setCheckable(true);
    connect(musicalDirectionAct, SIGNAL(triggered()), this,
            SLOT(editMusicalDirection()));

    repeatEndingAct = new Command(tr("Repeat Ending..."),
                                  "MusicSymbols.EditRepeatEnding",
                                  Qt::SHIFT + Qt::Key_E, this);
    repeatEndingAct->setCheckable(true);
    connect(repeatEndingAct, SIGNAL(triggered()), this,
            SLOT(editRepeatEnding()));
#endif
    myDynamicCommand = new Command(tr("Dynamic..."), "MusicSymbols.EditDynamic",
                                   Qt::Key_D, this);
    myDynamicCommand->setCheckable(true);
    connect(myDynamicCommand, SIGNAL(triggered()), this, SLOT(editDynamic()));
#if 0
    volumeSwellAct = new Command(tr("Volume Swell ..."), "MusicSymbols.VolumeSwell",
                                    QKeySequence(), this);
    volumeSwellAct->setCheckable(true);
    connect(volumeSwellAct, SIGNAL(triggered()), this, SLOT(editVolumeSwell()));

    // Tab Symbol Actions.
    hammerPullAct = new Command(tr("Hammer On/Pull Off"), "TabSymbols.HammerPull", Qt::Key_H, this);
    hammerPullAct->setCheckable(true);
    connect(hammerPullAct, SIGNAL(triggered()), this, SLOT(editHammerPull()));
#endif
    createNotePropertyCommand(myNaturalHarmonicCommand, tr("Natural Harmonic"),
                              "TabSymbols.NaturalHarmonic", QKeySequence(),
                              Note::NaturalHarmonic);
#if 0
    artificialHarmonicAct = new Command(tr("Artificial Harmonic..."),
            "TabSymbols.ArtificialHarmonic", QKeySequence(), this);
    artificialHarmonicAct->setCheckable(true);
    connect(artificialHarmonicAct, SIGNAL(triggered()),
            this, SLOT(editArtificialHarmonic()));
#endif
    myTappedHarmonicCommand = new Command(tr("Tapped Harmonic..."),
                                          "TabSymbols.TappedHarmonic",
                                          QKeySequence(), this);
    myTappedHarmonicCommand->setCheckable(true);
    connect(myTappedHarmonicCommand, SIGNAL(triggered()), this,
            SLOT(editTappedHarmonic()));
#if 0
    shiftSlideAct = new Command(tr("Shift Slide"), "TabSymbols.ShiftSlide", Qt::Key_S, this);
    shiftSlideAct->setCheckable(true);
    sigfwd::connect(shiftSlideAct, SIGNAL(triggered()),
                    boost::bind(&PowerTabEditor::editSlideOutOf, this, Note::slideOutOfShiftSlide));

    legatoSlideAct = new Command(tr("Legato Slide"), "TabSymbols.LegatoSlide", Qt::Key_L, this);
    legatoSlideAct->setCheckable(true);
    sigfwd::connect(legatoSlideAct, SIGNAL(triggered()),
                    boost::bind(&PowerTabEditor::editSlideOutOf, this, Note::slideOutOfLegatoSlide));
#endif
    createPositionPropertyCommand(myVibratoCommand, tr("Vibrato"),
                                  "TabSymbols.Vibrato", Qt::Key_V,
                                  Position::Vibrato);

    createPositionPropertyCommand(myWideVibratoCommand, tr("Wide Vibrato"),
                                  "TabSymbols.WideVibrato", Qt::Key_W,
                                  Position::WideVibrato);

    createPositionPropertyCommand(myPalmMuteCommand, tr("Palm Mute"),
                                  "TabSymbols.PalmMute", Qt::Key_M,
                                  Position::PalmMuting);

    createPositionPropertyCommand(myTremoloPickingCommand, tr("Tremolo Picking"),
                                  "TabSymbols.TremoloPicking", QKeySequence(),
                                  Position::TremoloPicking);

    createPositionPropertyCommand(myArpeggioUpCommand, tr("Arpeggio Up"),
                                  "TabSymbols.ArpeggioUp", QKeySequence(),
                                  Position::ArpeggioUp);

    createPositionPropertyCommand(myArpeggioDownCommand, tr("Arpeggio Down"),
                                  "TabSymbols.ArpeggioDown", QKeySequence(),
                                  Position::ArpeggioDown);

    createPositionPropertyCommand(myTapCommand, tr("Tap"), "TabSymbols.Tap",
                                  Qt::Key_P, Position::Tap);

    myTrillCommand = new Command(tr("Trill..."), "TabSymbols.Trill",
                                 QKeySequence(), this);
    myTrillCommand->setCheckable(true);
    connect(myTrillCommand, SIGNAL(triggered()), this, SLOT(editTrill()));

    createPositionPropertyCommand(myPickStrokeUpCommand, tr("Pickstroke Up"),
                                  "TabSymbols.PickStrokeUp", QKeySequence(),
                                  Position::PickStrokeUp);

    createPositionPropertyCommand(myPickStrokeDownCommand, tr("Pickstroke Down"),
                                  "TabSymbols.PickStrokeDown", QKeySequence(),
                                  Position::PickStrokeDown);
#if 0
    // Slide Into Menu
    slideIntoFromAboveAct = new Command(tr("Slide Into From Above"), "SlideInto.FromAbove",
                                        QKeySequence(), this);
    slideIntoFromAboveAct->setCheckable(true);
    sigfwd::connect(slideIntoFromAboveAct, SIGNAL(triggered()),
                    boost::bind(&PowerTabEditor::editSlideInto, this, Note::slideIntoFromAbove));

    slideIntoFromBelowAct = new Command(tr("Slide Into From Below"), "SlideInto.FromBelow",
                                        QKeySequence(), this);
    slideIntoFromBelowAct->setCheckable(true);
    sigfwd::connect(slideIntoFromBelowAct, SIGNAL(triggered()),
                    boost::bind(&PowerTabEditor::editSlideInto, this, Note::slideIntoFromBelow));

    // Slide Out Of Menu
    slideOutOfDownwardsAct = new Command(tr("Slide Out Of Downwards"), "SlideOut.Downwards",
                                         QKeySequence(), this);
    slideOutOfDownwardsAct->setCheckable(true);
    sigfwd::connect(slideOutOfDownwardsAct, SIGNAL(triggered()),
                    boost::bind(&PowerTabEditor::editSlideOutOf, this, Note::slideOutOfDownwards));

    slideOutOfUpwardsAct = new Command(tr("Slide Out Of Upwards"), "SlideOut.Upwards",
                                       QKeySequence(), this);
    slideOutOfUpwardsAct->setCheckable(true);
    sigfwd::connect(slideOutOfUpwardsAct, SIGNAL(triggered()),
                    boost::bind(&PowerTabEditor::editSlideOutOf, this, Note::slideOutOfUpwards));
#endif
    myPlayerChangeCommand = new Command("Player Change...",
                                        "Player.PlayerChange", QKeySequence(),
                                        this);
    myPlayerChangeCommand->setCheckable(true);
    connect(myPlayerChangeCommand, SIGNAL(triggered()), this,
            SLOT(editPlayerChange()));
#if 0
    // Guitar Menu
    addGuitarAct = new Command(tr("Add Guitar"), "Guitar.AddGuitar", QKeySequence(), this);
    connect(addGuitarAct, SIGNAL(triggered()), this, SLOT(addGuitar()));

    tuningDictionaryAct = new Command(tr("Tuning Dictionary"),
                            "Guitar.TuningDictionary", QKeySequence(), this);
    connect(tuningDictionaryAct, SIGNAL(triggered()),
            this, SLOT(showTuningDictionary()));
#endif

    // Window Menu commands.
    myNextTabCommand = new Command(tr("Next Tab"), "Window.NextTab",
                                   Qt::CTRL + Qt::Key_Tab, this);
    sigfwd::connect(myNextTabCommand, SIGNAL(triggered()),
                    boost::bind(&PowerTabEditor::cycleTab, this, 1));

    myPrevTabCommand = new Command(tr("Previous Tab"), "Window.PreviousTab",
                                   Qt::CTRL + Qt::SHIFT + Qt::Key_Tab, this);
    sigfwd::connect(myPrevTabCommand, SIGNAL(triggered()),
                    boost::bind(&PowerTabEditor::cycleTab, this, -1));
}

void PowerTabEditor::createNoteDurationCommand(
        Command *&command, const QString &menuName, const QString &commandName,
        Position::DurationType durationType)
{
    command = new Command(menuName, commandName, QKeySequence(), this);
    command->setCheckable(true);
    sigfwd::connect(command, SIGNAL(triggered()),
                    boost::bind(&PowerTabEditor::updateNoteDuration, this,
                                durationType));
    myNoteDurationGroup->addAction(command);
}

void PowerTabEditor::createNotePropertyCommand(
        Command *&command, const QString &menuName, const QString &commandName,
        const QKeySequence &shortcut, Note::SimpleProperty property)
{
    command = new Command(menuName, commandName, shortcut, this);
    command->setCheckable(true);
    sigfwd::connect(command, SIGNAL(triggered()),
                    boost::bind(&PowerTabEditor::editSimpleNoteProperty,
                                this, command, property));
}

void PowerTabEditor::createPositionPropertyCommand(
        Command *&command, const QString &menuName, const QString &commandName,
        const QKeySequence &shortcut, Position::SimpleProperty property)
{
    command = new Command(menuName, commandName, shortcut, this);
    command->setCheckable(true);
    sigfwd::connect(command, SIGNAL(triggered()),
                    boost::bind(&PowerTabEditor::editSimplePositionProperty,
                                this, command, property));
}

void PowerTabEditor::createMenus()
{
    // File Menu.
    myFileMenu = menuBar()->addMenu(tr("&File"));
    myFileMenu->addAction(myNewDocumentCommand);
    myFileMenu->addAction(myOpenFileCommand);
    myFileMenu->addAction(myCloseTabCommand);
    myFileMenu->addSeparator();
    myFileMenu->addAction(mySaveAsCommand);
    myFileMenu->addSeparator();
    myRecentFilesMenu = myFileMenu->addMenu(tr("Recent Files"));
    myFileMenu->addSeparator();
    myFileMenu->addAction(myEditShortcutsCommand);
    myFileMenu->addAction(myEditPreferencesCommand);
    myFileMenu->addSeparator();
    myFileMenu->addAction(myExitCommand);

    // Edit Menu.
    myEditMenu = menuBar()->addMenu(tr("&Edit"));
    myEditMenu->addAction(myUndoAction);
    myEditMenu->addAction(myRedoAction);
#if 0
    editMenu->addSeparator();
    editMenu->addAction(cutAct);
    editMenu->addAction(copyAct);
    editMenu->addAction(pasteAct);
    editMenu->addSeparator();
    editMenu->addAction(fileInfoAct);

    // Playback Menu
    playbackMenu = menuBar()->addMenu(tr("Play&back"));
    playbackMenu->addAction(playPauseAct);

#endif
    // Position Menu.
    myPositionMenu = menuBar()->addMenu(tr("&Position"));
    myPositionSectionMenu = myPositionMenu->addMenu(tr("&Section"));
    myPositionSectionMenu->addAction(myFirstSectionCommand);
    myPositionSectionMenu->addAction(myNextSectionCommand);
    myPositionSectionMenu->addAction(myPrevSectionCommand);
    myPositionSectionMenu->addAction(myLastSectionCommand);

    myPositionStaffMenu = myPositionMenu->addMenu(tr("&Staff"));
    myPositionStaffMenu->addAction(myStartPositionCommand);
    myPositionStaffMenu->addAction(myNextPositionCommand);
    myPositionStaffMenu->addAction(myPrevPositionCommand);
    myPositionStaffMenu->addAction(myNextStringCommand);
    myPositionStaffMenu->addAction(myPrevStringCommand);
    myPositionStaffMenu->addAction(myLastPositionCommand);
    myPositionStaffMenu->addAction(myNextStaffCommand);
    myPositionStaffMenu->addAction(myPrevStaffCommand);
    myPositionStaffMenu->addAction(myNextBarCommand);
    myPositionStaffMenu->addAction(myPrevBarCommand);

#if 0
    positionMenu->addSeparator();
    positionMenu->addAction(shiftTabNumUp);
    positionMenu->addAction(shiftTabNumDown);
    positionMenu->addSeparator();
    positionMenu->addAction(shiftForwardAct);
    positionMenu->addAction(shiftBackwardAct);
    positionMenu->addSeparator();
    positionMenu->addAction(clearNoteAct);
    positionMenu->addAction(clearCurrentPositionAct);
#endif
    myPositionMenu->addSeparator();
    myPositionMenu->addAction(myGoToBarlineCommand);
    myPositionMenu->addAction(myGoToRehearsalSignCommand);
#if 0

    // Text Menu
    textMenu = menuBar()->addMenu(tr("&Text"));
    textMenu->addAction(chordNameAct);

#endif
    // Section Menu.
    mySectionMenu = menuBar()->addMenu(tr("&Section"));
#if 0
    mySectionMenu->addAction(increasePositionSpacingAct);
    mySectionMenu->addAction(decreasePositionSpacingAct);
    mySectionMenu->addSeparator();
#endif
    mySectionMenu->addAction(myInsertSystemAtEndCommand);
    mySectionMenu->addAction(myInsertSystemBeforeCommand);
    mySectionMenu->addAction(myInsertSystemAfterCommand);
    mySectionMenu->addSeparator();
    mySectionMenu->addAction(myRemoveCurrentSystemCommand);
    mySectionMenu->addSeparator();
    myLineSpacingMenu = mySectionMenu->addMenu(tr("&Line Spacing"));
    myLineSpacingMenu->addAction(myIncreaseLineSpacingCommand);
    myLineSpacingMenu->addAction(myDecreaseLineSpacingCommand);

    // Notes Menu.
    myNotesMenu = menuBar()->addMenu(tr("&Notes"));
    myNotesMenu->addAction(myWholeNoteCommand);
    myNotesMenu->addAction(myHalfNoteCommand);
    myNotesMenu->addAction(myQuarterNoteCommand);
    myNotesMenu->addAction(myEighthNoteCommand);
    myNotesMenu->addAction(mySixteenthNoteCommand);
    myNotesMenu->addAction(myThirtySecondNoteCommand);
    myNotesMenu->addAction(mySixtyFourthNoteCommand);
    myNotesMenu->addSeparator();
    myNotesMenu->addAction(myIncreaseDurationCommand);
    myNotesMenu->addAction(myDecreaseDurationCommand);
    myNotesMenu->addSeparator();
    myNotesMenu->addAction(myDottedCommand);
    myNotesMenu->addAction(myDoubleDottedCommand);
    myNotesMenu->addSeparator();
#if 0
    myNotesMenu->addAction(addDotAct);
    myNotesMenu->addAction(removeDotAct);
    myNotesMenu->addSeparator();
    myNotesMenu->addAction(tiedNoteAct);
    myNotesMenu->addSeparator();
#endif
    myNotesMenu->addAction(myMutedCommand);
    myNotesMenu->addAction(myGhostNoteCommand);
    myNotesMenu->addSeparator();
    myNotesMenu->addAction(myLetRingCommand);
#if 0
    myNotesMenu->addAction(fermataAct);
#endif
    myNotesMenu->addSeparator();
#if 0
    myNotesMenu->addActions(QList<QAction*>() << graceNoteAct << staccatoNoteAct <<
                          marcatoAct << sforzandoAct);

#endif
    myOctaveMenu = myNotesMenu->addMenu(tr("Octave"));
    myOctaveMenu->addActions(QList<QAction*>() << myOctave8vaCommand << myOctave15maCommand
                           << myOctave8vbCommand << myOctave15mbCommand);
#if 0
    myNotesMenu->addAction(tripletAct);
    myNotesMenu->addAction(irregularGroupingAct);

    // Rests Menu
    restsMenu = menuBar()->addMenu(tr("Rests"));
    restsMenu->addActions(QList<QAction*>() << wholeRestAct << halfRestAct <<
                          quarterRestAct << eighthRestAct << sixteenthRestAct <<
                          thirtySecondRestAct << sixtyFourthRestAct);
    restsMenu->addSeparator();
    restsMenu->addAction(addRestAct);

#endif
    // Music Symbols Menu.
    myMusicSymbolsMenu = menuBar()->addMenu(tr("&Music Symbols"));
    myMusicSymbolsMenu->addAction(myRehearsalSignCommand);
#if 0
    myMusicSymbolsMenu->addAction(tempoMarkerAct);
#endif
    myMusicSymbolsMenu->addAction(myKeySignatureCommand);
#if 0
    myMusicSymbolsMenu->addAction(timeSignatureAct);
#endif
    myMusicSymbolsMenu->addAction(myStandardBarlineCommand);
    myMusicSymbolsMenu->addAction(myBarlineCommand);
#if 0
    myMusicSymbolsMenu->addAction(musicalDirectionAct);
    myMusicSymbolsMenu->addAction(repeatEndingAct);
#endif
    myMusicSymbolsMenu->addAction(myDynamicCommand);
#if 0
    myMusicSymbolsMenu->addAction(volumeSwellAct);

#endif
    // Tab Symbols Menu
    myTabSymbolsMenu = menuBar()->addMenu(tr("&Tab Symbols"));
#if 0
    tabSymbolsMenu->addAction(hammerPullAct);
#endif
    myTabSymbolsMenu->addAction(myNaturalHarmonicCommand);
#if 0
    tabSymbolsMenu->addAction(artificialHarmonicAct);
#endif
    myTabSymbolsMenu->addAction(myTappedHarmonicCommand);
    myTabSymbolsMenu->addSeparator();
#if 0

    slideIntoMenu = tabSymbolsMenu->addMenu(tr("Slide Into"));
    slideIntoMenu->addAction(slideIntoFromBelowAct);
    slideIntoMenu->addAction(slideIntoFromAboveAct);

    tabSymbolsMenu->addAction(shiftSlideAct);
    tabSymbolsMenu->addAction(legatoSlideAct);

    slideOutOfMenu = tabSymbolsMenu->addMenu(tr("Slide Out Of"));
    slideOutOfMenu->addAction(slideOutOfDownwardsAct);
    slideOutOfMenu->addAction(slideOutOfUpwardsAct);    
#endif
    myPlayerMenu = menuBar()->addMenu(tr("&Player"));
    myPlayerMenu->addAction(myPlayerChangeCommand);
#if 0
    guitarMenu = menuBar()->addMenu(tr("&Guitar"));
    guitarMenu->addAction(addGuitarAct);
    guitarMenu->addSeparator();
    guitarMenu->addAction(tuningDictionaryAct);
#endif
    myTabSymbolsMenu->addSeparator();
    myTabSymbolsMenu->addAction(myVibratoCommand);
    myTabSymbolsMenu->addAction(myWideVibratoCommand);
    myTabSymbolsMenu->addSeparator();
    myTabSymbolsMenu->addAction(myPalmMuteCommand);
    myTabSymbolsMenu->addAction(myTremoloPickingCommand);
    myTabSymbolsMenu->addAction(myTrillCommand);
    myTabSymbolsMenu->addAction(myTapCommand);
    myTabSymbolsMenu->addSeparator();
    myTabSymbolsMenu->addAction(myArpeggioUpCommand);
    myTabSymbolsMenu->addAction(myArpeggioDownCommand);
    myTabSymbolsMenu->addSeparator();
    myTabSymbolsMenu->addAction(myPickStrokeUpCommand);
    myTabSymbolsMenu->addAction(myPickStrokeDownCommand);

    // Window Menu.
    myWindowMenu = menuBar()->addMenu(tr("&Window"));
    myWindowMenu->addAction(myNextTabCommand);
    myWindowMenu->addAction(myPrevTabCommand);
}

void PowerTabEditor::createTabArea()
{
    myTabWidget = new QTabWidget(this);
    myTabWidget->setTabsClosable(true);

#if 0
    tabWidget->setStyleSheet(skinManager->getDocumentTabStyle());
#endif

    connect(myTabWidget, SIGNAL(tabCloseRequested(int)), this,
            SLOT(closeTab(int)));
    connect(myTabWidget, SIGNAL(currentChanged(int)), this,
            SLOT(switchTab(int)));

    onDocumentOpenedOrClosed(false);
}

void PowerTabEditor::setPreviousDirectory(const QString &fileName)
{
    QFileInfo fileInfo(fileName);
    myPreviousDirectory = fileInfo.absolutePath();
    QSettings settings;
    settings.setValue(Settings::APP_PREVIOUS_DIRECTORY, myPreviousDirectory);
}

void PowerTabEditor::setupNewTab()
{
    boost::timer timer;
    qDebug() << "Tab creation started ...";

    Q_ASSERT(myDocumentManager->hasOpenDocuments());
    Document &doc = myDocumentManager->getCurrentDocument();

    doc.getCaret().subscribeToChanges(
                boost::bind(&PowerTabEditor::updateCommands, this));

    ScoreArea *scorearea = new ScoreArea(this);
    scorearea->renderDocument(doc, Staff::GuitarView);
    scorearea->installEventFilter(this);
#if 0
    // Connect the signals for mouse clicks on time signatures, barlines, etc.
    // to the appropriate event handlers.
    score->timeSignaturePubSub()->subscribe(
                boost::bind(&PowerTabEditor::editTimeSignature, this, _1));
#endif
    scorearea->getKeySignaturePubSub()->subscribe(
                boost::bind(&PowerTabEditor::editKeySignature, this, _1));
    scorearea->getBarlinePubSub()->subscribe(
                boost::bind(&PowerTabEditor::editBarline, this, _1));

    myUndoManager->addNewUndoStack();

    QString filename = "Untitled";
    if (doc.hasFilename())
        filename = QString::fromStdString(doc.getFilename());

    QFileInfo fileInfo(filename);

    // Create title for the tab bar.
    QString title = fileInfo.fileName();
    QFontMetrics fm (myTabWidget->font());

    // Each tab is 200px wide, so we want to shorten the name if it's wider
    // than 140px.
    bool chopped = false;
    while (fm.width(title) > 140)
    {
        title.chop(1);
        chopped = true;
    }

    if (chopped)
        title.append("...");

    const int tabIndex = myTabWidget->addTab(scorearea, title);
    myTabWidget->setTabToolTip(tabIndex, fileInfo.fileName());

#if 0
    // add the guitars to a new mixer
    Mixer* mixer = new Mixer(doc->GetPlayerScore(), tuningDictionary);
    connect(mixer, SIGNAL(visibilityToggled(uint32_t,bool)),
            this, SLOT(toggleGuitarVisible(uint32_t,bool)));

    QScrollArea* scrollArea = new QScrollArea;
    // show all players
    for (quint32 i=0; i < doc->GetPlayerScore()->GetGuitarCount(); i++)
    {
        mixer->addInstrument(doc->GetPlayerScore()->GetGuitar(i));
    }
    scrollArea->setWidget(mixer);
    mixerList->addWidget(scrollArea);

    PlaybackWidget* playback = new PlaybackWidget(settingsPubSub);
    connect(playback, SIGNAL(playbackButtonToggled()),
            this, SLOT(startStopPlayback()));
    connect(playback, SIGNAL(rewindToStartClicked()),
            this, SLOT(rewindPlaybackToStart()));
    connect(playback, SIGNAL(scoreSelected(int)),
            score, SLOT(setScoreIndex(int)));
    connect(playback, SIGNAL(activeVoiceChanged(int)),
            this, SLOT(updateActiveVoice(int)));

    playbackToolbarList->addWidget(playback);

    // ensure all the scores are visible to select
    QStringList scores;
    for (size_t i = 0; i < doc->GetNumberOfScores(); ++i)
    {
        scores.append(doc->GetScore(i)->GetScoreName().c_str());
    }
    playback->onDocumentUpdated(scores);

    connect(undoManager.get(), SIGNAL(indexChanged(int)), mixer, SLOT(update()));

#endif
    // Switch to the new document.
    myTabWidget->setCurrentIndex(myDocumentManager->getCurrentDocumentIndex());

    onDocumentOpenedOrClosed(true);
    updateCommands();
    scorearea->setFocus();

    qDebug() << "Tab opened in" << timer.elapsed() << "seconds";
}

namespace
{
inline void updatePositionProperty(Command *command, const Position *pos,
                                   Position::SimpleProperty property)
{
    command->setEnabled(pos);
    command->setChecked(pos && pos->hasProperty(property));
}

inline void updateNoteProperty(Command *command, const Note *note,
                               Note::SimpleProperty property)
{
    command->setEnabled(note);
    command->setChecked(note && note->hasProperty(property));
}
}

void PowerTabEditor::updateCommands()
{
    const ScoreLocation &location = getLocation();
    const Score &score = location.getScore();
    const System &system = location.getSystem();
    const Staff &staff = location.getStaff();
    const Position *pos = location.getPosition();
    const int position = location.getPositionIndex();
    const Note *note = location.getNote();
    const Barline *barline = location.getBarline();

    myRemoveCurrentSystemCommand->setEnabled(score.getSystems().size() > 1);
    myIncreaseLineSpacingCommand->setEnabled(
                score.getLineSpacing() < Score::MAX_LINE_SPACING);
    myDecreaseLineSpacingCommand->setEnabled(
                score.getLineSpacing() > Score::MIN_LINE_SPACING);

    // Note durations
    Position::DurationType durationType = myActiveDurationType;
    if (pos)
        durationType = pos->getDurationType();

    switch (durationType)
    {
    case Position::WholeNote:
        myWholeNoteCommand->setChecked(true);
        break;
    case Position::HalfNote:
        myHalfNoteCommand->setChecked(true);
        break;
    case Position::QuarterNote:
        myQuarterNoteCommand->setChecked(true);
        break;
    case Position::EighthNote:
        myEighthNoteCommand->setChecked(true);
        break;
    case Position::SixteenthNote:
        mySixteenthNoteCommand->setChecked(true);
        break;
    case Position::ThirtySecondNote:
        myThirtySecondNoteCommand->setChecked(true);
        break;
    case Position::SixtyFourthNote:
        mySixtyFourthNoteCommand->setChecked(true);
        break;
    }

    myIncreaseDurationCommand->setEnabled(durationType != Position::WholeNote);
    myDecreaseDurationCommand->setEnabled(durationType != Position::SixtyFourthNote);

    updatePositionProperty(myDottedCommand, pos, Position::Dotted);
    updatePositionProperty(myDoubleDottedCommand, pos, Position::DoubleDotted);
    updateNoteProperty(myMutedCommand, note, Note::Muted);
    updateNoteProperty(myGhostNoteCommand, note, Note::GhostNote);
    updatePositionProperty(myLetRingCommand, pos, Position::LetRing);

    updateNoteProperty(myOctave8vaCommand, note, Note::Octave8va);
    updateNoteProperty(myOctave8vbCommand, note, Note::Octave8vb);
    updateNoteProperty(myOctave15maCommand, note, Note::Octave15ma);
    updateNoteProperty(myOctave15mbCommand, note, Note::Octave15mb);

    myRehearsalSignCommand->setEnabled(barline);
    myRehearsalSignCommand->setChecked(barline && barline->hasRehearsalSign());
    myKeySignatureCommand->setEnabled(barline);
    myStandardBarlineCommand->setEnabled(!pos && !barline);
    myDynamicCommand->setChecked(ScoreUtils::findByPosition(staff.getDynamics(),
                                                      position));

    if (barline) // Current position is bar.
    {
        myBarlineCommand->setText(tr("Edit Barline"));
        myBarlineCommand->setEnabled(true);
    }
    else if (!pos) // Current position is empty.
    {
        myBarlineCommand->setText(tr("Insert Barline"));
        myBarlineCommand->setEnabled(true);
    }
    else // Current position has notes.
    {
        myBarlineCommand->setDisabled(true);
        myBarlineCommand->setText(tr("Barline"));
    }

    updateNoteProperty(myNaturalHarmonicCommand, note, Note::NaturalHarmonic);
    myTappedHarmonicCommand->setEnabled(note);
    myTappedHarmonicCommand->setChecked(note && note->hasTappedHarmonic());
    updatePositionProperty(myVibratoCommand, pos, Position::Vibrato);
    updatePositionProperty(myWideVibratoCommand, pos, Position::WideVibrato);
    updatePositionProperty(myPalmMuteCommand, pos, Position::PalmMuting);
    updatePositionProperty(myTremoloPickingCommand, pos, Position::TremoloPicking);
    myTrillCommand->setEnabled(note);
    myTrillCommand->setChecked(note && note->hasTrill());
    updatePositionProperty(myTapCommand, pos, Position::Tap);
    updatePositionProperty(myArpeggioUpCommand, pos, Position::ArpeggioUp);
    updatePositionProperty(myArpeggioDownCommand, pos, Position::ArpeggioDown);
    updatePositionProperty(myPickStrokeUpCommand, pos, Position::PickStrokeUp);
    updatePositionProperty(myPickStrokeDownCommand, pos,
                           Position::PickStrokeDown);

    myPlayerChangeCommand->setChecked(ScoreUtils::findByPosition(
                                          system.getPlayerChanges(), position));
}

void PowerTabEditor::onDocumentOpenedOrClosed(bool hasOpenDocuments)
{
    QList<QMenu *> menuList;
    menuList << myPositionMenu << myPositionSectionMenu << myPositionStaffMenu <<
                mySectionMenu << myLineSpacingMenu << myNotesMenu <<
                myOctaveMenu << myMusicSymbolsMenu << myTabSymbolsMenu <<
                myPlayerMenu << myWindowMenu;

    foreach(QMenu *menu, menuList)
    {
        foreach(QAction *action, menu->actions())
        {
            action->setEnabled(hasOpenDocuments);
        }
    }

    myCloseTabCommand->setEnabled(hasOpenDocuments);
    mySaveAsCommand->setEnabled(hasOpenDocuments);
#if 0
    addGuitarAct->setEnabled(enable);
#endif
}

void PowerTabEditor::editKeySignature(const ScoreLocation &keyLocation)
{
    ScoreLocation location(getLocation());
    location.setSystemIndex(keyLocation.getSystemIndex());
    location.setPositionIndex(keyLocation.getPositionIndex());

    const Barline *barline = location.getBarline();
    Q_ASSERT(barline);

    KeySignatureDialog dialog(this, barline->getKeySignature());
    if (dialog.exec() == QDialog::Accepted)
    {
        myUndoManager->push(new EditKeySignature(location, dialog.getNewKey()),
                            UndoManager::AFFECTS_ALL_SYSTEMS);
    }
}

void PowerTabEditor::editBarline(const ScoreLocation &barLocation)
{
    ScoreLocation location(getLocation());
    location.setSystemIndex(barLocation.getSystemIndex());
    location.setPositionIndex(barLocation.getPositionIndex());
    System &system = location.getSystem();

    Barline *barline = ScoreUtils::findByPosition(system.getBarlines(),
                                                  location.getPositionIndex());

    if (barline)
    {
        const bool isStartBar = *barline == system.getBarlines().front();
        const bool isEndBar = *barline == system.getBarlines().back();

        BarlineDialog dialog(this, barline->getBarType(),
                             barline->getRepeatCount(), isStartBar, isEndBar);
        if (dialog.exec() == QDialog::Accepted)
        {
            myUndoManager->push(new EditBarline(location, dialog.getBarType(),
                                                dialog.getRepeatCount()),
                                location.getSystemIndex());
        }
    }
    else
    {
        BarlineDialog dialog(this, Barline::SingleBar, Barline::MIN_REPEAT_COUNT,
                             false, false);
        if (dialog.exec() == QDialog::Accepted)
        {
            Barline bar(location.getPositionIndex(), dialog.getBarType(),
                        dialog.getRepeatCount());
            myUndoManager->push(new AddBarline(location, bar),
                                location.getSystemIndex());
        }
    }
}

void PowerTabEditor::editSimplePositionProperty(Command *command,
                                                Position::SimpleProperty property)
{
    ScoreLocation &location = getLocation();
    std::vector<Position *> selectedPositions = location.getSelectedPositions();
    if (selectedPositions.empty())
        return;

    // If at least one position doesn't have the property set, enable it for
    // all of them.
    bool enableProperty = false;
    BOOST_FOREACH(const Position *pos, selectedPositions)
    {
        if (!pos->hasProperty(property))
            enableProperty = true;
    }

    if (enableProperty)
    {
        myUndoManager->push(new AddPositionProperty(location, property,
                                                    command->text()),
                            location.getSystemIndex());
    }
    else
    {
        myUndoManager->push(new RemovePositionProperty(location, property,
                                                       command->text()),
                            location.getSystemIndex());
    }
}

void PowerTabEditor::editSimpleNoteProperty(Command *command,
                                            Note::SimpleProperty property)
{
    ScoreLocation &location = getLocation();
    std::vector<Note *> selectedNotes = location.getSelectedNotes();
    if (selectedNotes.empty())
        return;

    // If at least one note doesn't have the property set, enable it for
    // all of them.
    bool enableProperty = false;
    BOOST_FOREACH(const Note *note, selectedNotes)
    {
        if (!note->hasProperty(property))
            enableProperty = true;
    }

    if (enableProperty)
    {
        myUndoManager->push(new AddNoteProperty(location, property,
                                                command->text()),
                            location.getSystemIndex());
    }
    else
    {
        myUndoManager->push(new RemoveNoteProperty(location, property,
                                                   command->text()),
                            location.getSystemIndex());
    }
}

void PowerTabEditor::insertSystem(int index)
{
    myUndoManager->push(new AddSystem(getLocation().getScore(), index),
                        UndoManager::AFFECTS_ALL_SYSTEMS);
}

void PowerTabEditor::adjustLineSpacing(int amount)
{
    myUndoManager->push(new AdjustLineSpacing(getLocation().getScore(), amount),
                        UndoManager::AFFECTS_ALL_SYSTEMS);
}

ScoreArea *PowerTabEditor::getScoreArea()
{
    return dynamic_cast<ScoreArea *>(myTabWidget->currentWidget());
}

Caret &PowerTabEditor::getCaret()
{
    return myDocumentManager->getCurrentDocument().getCaret();
}

ScoreLocation &PowerTabEditor::getLocation()
{
    return getCaret().getLocation();
}

#if 0
/// Before exiting, prompt to save any modified documents.
void PowerTabEditor::closeEvent(QCloseEvent* event)
{
    while (tabWidget->currentIndex() != -1)
    {
        bool closed = closeCurrentTab();
        if (!closed)
        {
            // Don't close if the user pressed Cancel.
            event->ignore();
            return;
        }
    }

    tuningDictionary->save();
}

/// Marks the window as modified/unmodified depending on the state of the active UndoStack
void PowerTabEditor::updateModified(bool clean)
{
    setWindowModified(!clean);
}

void PowerTabEditor::updateActiveVoice(int voice)
{
    getCurrentScoreArea()->getCaret()->setCurrentVoice(voice);
}

Mixer* PowerTabEditor::getCurrentMixer()
{
    QScrollArea* scrollArea = dynamic_cast<QScrollArea*>(mixerList->currentWidget());
    Mixer* currentMixer = dynamic_cast<Mixer*>(scrollArea->widget());
    Q_ASSERT(currentMixer != NULL);
    return currentMixer;
}

std::vector<Position*> PowerTabEditor::getSelectedPositions()
{
    return getCurrentScoreArea()->getCaret()->getSelectedPositions();
}

std::vector<Note*> PowerTabEditor::getSelectedNotes()
{
    return getCurrentScoreArea()->getCaret()->getSelectedNotes();
}

void PowerTabEditor::registerCaret(Caret* caret)
{
    connect(caret, SIGNAL(moved()), this, SLOT(updateActions()));
    connect(caret, SIGNAL(moved()), this, SLOT(updateLocationLabel()));
    connect(caret, SIGNAL(selectionChanged()), this, SLOT(updateActions()));
}

PlaybackWidget* PowerTabEditor::getCurrentPlaybackWidget() const
{
    return dynamic_cast<PlaybackWidget*>(playbackToolbarList->currentWidget());
}

/// Returns the current playback speed
int PowerTabEditor::getCurrentPlaybackSpeed() const
{
    if (playbackToolbarList->count() == 0)
    {
        return 0;
    }

    return getCurrentPlaybackWidget()->playbackSpeed();
}

void PowerTabEditor::startStopPlayback()
{
    isPlaying = !isPlaying;

    if (isPlaying)
    {
        playPauseAct->setText(tr("Pause"));

        getCurrentScoreArea()->getCaret()->setPlaybackMode(true);
        getCurrentPlaybackWidget()->setPlaybackMode(true);

        midiPlayer.reset(new MidiPlayer(getCurrentScoreArea()->getCaret(), getCurrentPlaybackSpeed()));
        connect(midiPlayer.get(), SIGNAL(playbackSystemChanged(quint32)), this, SLOT(moveCaretToSystem(quint32)));
        connect(midiPlayer.get(), SIGNAL(playbackPositionChanged(quint8)), this, SLOT(moveCaretToPosition(quint8)));
        connect(midiPlayer.get(), SIGNAL(finished()), this, SLOT(startStopPlayback()));
        connect(playbackToolbarList->currentWidget(), SIGNAL(playbackSpeedChanged(int)), midiPlayer.get(), SLOT(changePlaybackSpeed(int)));
        midiPlayer->start();
    }
    else
    {
        // If we manually stop playback, tell the midi thread to finish.
        if (midiPlayer && midiPlayer->isRunning())
        {
            // Avoid recursion from the finished() signal being called.
            midiPlayer->disconnect(this);
            midiPlayer.reset();
        }

        playPauseAct->setText(tr("Play"));
        getCurrentScoreArea()->getCaret()->setPlaybackMode(false);
        getCurrentPlaybackWidget()->setPlaybackMode(false);

        updateScoreAreaActions(true);
        updateActions();
    }
}

/// Move the caret back to the start, and restart playback if necessary.
void PowerTabEditor::rewindPlaybackToStart()
{
    const bool wasPlaying = isPlaying;
    if (wasPlaying)
    {
        startStopPlayback();
    }

    getCurrentScoreArea()->getCaret()->moveCaretToFirstSection();

    if (wasPlaying)
    {
        startStopPlayback();
    }
}

void PowerTabEditor::shiftTabNumber(int direction)
{
    const Position::ShiftType shiftType = static_cast<Position::ShiftType>(direction);
    Caret* caret = getCurrentScoreArea()->getCaret();
    Position* currentPos = caret->getCurrentPosition();
    Note* currentNote = caret->getCurrentNote();
    const Tuning& tuning = caret->getCurrentScore()->GetGuitar(caret->getCurrentStaffIndex())->GetTuning();

    if (!currentPos->CanShiftTabNumber(currentNote, shiftType, tuning))
    {
        return;
    }

    undoManager->push(new ShiftTabNumber(caret->getCurrentStaff(), currentPos,
                                         currentNote, caret->getCurrentVoice(),
                                         shiftType, tuning),
                      caret->getCurrentSystemIndex());
    caret->moveCaretVertical(direction == 1 ? direction : -1);
}

bool PowerTabEditor::moveCaretToSystem(quint32 system)
{
    Caret* caret = getCurrentScoreArea()->getCaret();
    const int offset = system - caret->getCurrentSystemIndex();
    return getCurrentScoreArea()->getCaret()->moveCaretSection(offset);
}

bool PowerTabEditor::moveCaretToPosition(quint8 position)
{
    return getCurrentScoreArea()->getCaret()->setCurrentPositionIndex(position);
}

bool PowerTabEditor::moveCaretToNextStaff()
{
    return getCurrentScoreArea()->getCaret()->moveCaretStaff(1);;
}

bool PowerTabEditor::moveCaretToPrevStaff()
{
    return getCurrentScoreArea()->getCaret()->moveCaretStaff(-1);
}

void PowerTabEditor::changePositionSpacing(int offset)
{
    const Caret* caret = getCurrentScoreArea()->getCaret();
    shared_ptr<System> currentSystem = caret->getCurrentSystem();

    const int newSpacing = currentSystem->GetPositionSpacing() + offset;
    if (currentSystem->IsValidPositionSpacing(newSpacing))
    {
        undoManager->push(new ChangePositionSpacing(currentSystem, newSpacing),
                          caret->getCurrentSystemIndex());
    }
}

void PowerTabEditor::shiftForward()
{
    Caret* caret = getCurrentScoreArea()->getCaret();

    undoManager->push(new PositionShift(caret->getCurrentScore(),
                                        caret->getCurrentSystem(),
                                        caret->getCurrentPositionIndex(),
                                        PositionShift::SHIFT_FORWARD),
                      caret->getCurrentSystemIndex());
}

void PowerTabEditor::shiftBackward()
{
    Caret* caret = getCurrentScoreArea()->getCaret();

    undoManager->push(new PositionShift(caret->getCurrentScore(),
                                        caret->getCurrentSystem(),
                                        caret->getCurrentPositionIndex(),
                                        PositionShift::SHIFT_BACKWARD),
                      caret->getCurrentSystemIndex());
}

/// Clears the note at the caret's current position
void PowerTabEditor::clearNote()
{
    const Caret* caret = getCurrentScoreArea()->getCaret();
    Position* position = caret->getCurrentPosition();
    const uint8_t string = caret->getCurrentStringIndex();

    if (DeleteNote::canExecute(position, string))
    {
        undoManager->push(new DeleteNote(caret->getCurrentStaff(), caret->getCurrentVoice(),
                                         position, string, true),
                          caret->getCurrentSystemIndex());
    }
}

/// Completely clears the caret's selected positions.
/// For each position, it either removes a barline, or all of the notes at
/// the position.
void PowerTabEditor::clearCurrentPosition()
{
    Caret* caret = getCurrentScoreArea()->getCaret();
    shared_ptr<System> system = caret->getCurrentSystem();
    shared_ptr<Staff> staff = caret->getCurrentStaff();
    const std::vector<Position*> positions = caret->getSelectedPositions();
    const std::vector<shared_ptr<Barline> > bars = caret->getSelectedBarlines();
    const uint32_t voice = caret->getCurrentVoice();

    undoManager->beginMacro(tr("Clear Position"));

    BOOST_FOREACH(Position *pos, positions)
    {
        if (pos)
        {
            undoManager->push(new DeletePosition(staff, pos, voice),
                              caret->getCurrentSystemIndex());
        }
    }

    shared_ptr<const Barline> startBar = system->GetStartBar();
    shared_ptr<const Barline> endBar = system->GetEndBar();

    for (std::vector<shared_ptr<Barline> >::const_iterator bar = bars.begin();
         bar != bars.end(); ++bar)
    {
        // Don't allow the start/end bars to be deleted.
        if (*bar && *bar != startBar && *bar != endBar)
        {
            undoManager->push(new DeleteBarline(caret->getCurrentScore(),
                                                system, *bar),
                              caret->getCurrentSystemIndex());
        }
    }

    undoManager->endMacro();
}

void PowerTabEditor::addGuitar()
{
    Score* score = getCurrentScoreArea()->getCaret()->getCurrentScore();

    AddGuitar* addGuitar = new AddGuitar(score, getCurrentMixer());
    undoManager->push(addGuitar, UndoManager::AFFECTS_ALL_SYSTEMS);
}

void PowerTabEditor::showTuningDictionary()
{
    TuningDictionaryDialog dialog(tuningDictionary, this);
    dialog.exec();
}

/// Edit the time signature at the caret's current location.
void PowerTabEditor::editTimeSignatureFromCaret()
{
    const Caret* caret = getCurrentScoreArea()->getCaret();
    const SystemLocation caretLocation(caret->getCurrentSystemIndex(),
                                       caret->getCurrentPositionIndex());
    editTimeSignature(caretLocation);
}

/// Edit the time signature at the specified location.
void PowerTabEditor::editTimeSignature(const SystemLocation& location)
{
    const Caret* caret = getCurrentScoreArea()->getCaret();
    Score* score = caret->getCurrentScore();
    shared_ptr<Barline> barline = score->GetSystem(location.getSystemIndex())->
            GetBarlineAtPosition(location.getPositionIndex());
    Q_ASSERT(barline);

    const TimeSignature& currentTimeSig = barline->GetTimeSignature();

    TimeSignatureDialog dialog(this, currentTimeSig);

    if (dialog.exec() == QDialog::Accepted)
    {
        EditTimeSignature* action = new EditTimeSignature(score, location,
                                                          dialog.getNewTimeSignature());
        undoManager->push(action, UndoManager::AFFECTS_ALL_SYSTEMS);
    }
}

void PowerTabEditor::editArtificialHarmonic()
{
    Caret* caret = getCurrentScoreArea()->getCaret();
    Note* currentNote = caret->getCurrentNote();

    if (currentNote->HasArtificialHarmonic())
    {
        undoManager->push(new RemoveArtificialHarmonic(currentNote),
                          caret->getCurrentSystemIndex());
    }
    else
    {
        ArtificialHarmonicDialog dialog(this);
        if (dialog.exec() == QDialog::Accepted)
        {
            undoManager->push(new AddArtificialHarmonic(currentNote,
                    dialog.getKey(), dialog.getKeyVariation(),
                    dialog.getOctave()),
                              caret->getCurrentSystemIndex());
        }
        else
        {
            artificialHarmonicAct->setChecked(false);
        }
    }
}

void PowerTabEditor::editRepeatEnding()
{
    const Caret* caret = getCurrentScoreArea()->getCaret();
    Score* currentScore = caret->getCurrentScore();
    shared_ptr<AlternateEnding> altEnding = currentScore->FindAlternateEnding(SystemLocation(caret->getCurrentSystemIndex(),
                                                                                             caret->getCurrentPositionIndex()));

    if (!altEnding) // add an alternate ending
    {
        altEnding = boost::make_shared<AlternateEnding>(caret->getCurrentSystemIndex(),
                                                      caret->getCurrentPositionIndex(), 0);

        AlternateEndingDialog dialog(this, altEnding);
        if (dialog.exec() == QDialog::Accepted)
        {
            undoManager->push(new AddAlternateEnding(currentScore, altEnding),
                              caret->getCurrentSystemIndex());
        }
        else
        {
            repeatEndingAct->setChecked(false);
        }
    }
    else
    {
        undoManager->push(new RemoveAlternateEnding(currentScore, altEnding),
                          caret->getCurrentSystemIndex());
    }
}

// If there is a chord name at the current position, remove it
// If there is no chord name, show the dialog to add a chord name
// Existing chord names are edited by clicking on the chord name
void PowerTabEditor::editChordName()
{
    // Find if there is a chord name at the current position
    Caret* caret = getCurrentScoreArea()->getCaret();
    const quint32 caretPosition = caret->getCurrentPositionIndex();
    shared_ptr<System> currentSystem = caret->getCurrentSystem();

    int chordTextIndex = caret->getCurrentSystem()->FindChordText(caretPosition);
    if (chordTextIndex == -1) // if not found, add a new chord name
    {
        chordTextIndex = 0;
        ChordName chordName;
        ChordNameDialog chordNameDialog(this, &chordName);
        if (chordNameDialog.exec() == QDialog::Accepted)
        {
            shared_ptr<ChordText> chordText = boost::make_shared<ChordText>(caretPosition, chordName);
            undoManager->push(new AddChordText(currentSystem, chordText, chordTextIndex),
                              caret->getCurrentSystemIndex());
        }
    }
    else // if found, remove the chord name
    {
        undoManager->push(new RemoveChordText(currentSystem, chordTextIndex),
                          caret->getCurrentSystemIndex());
    }

}

/// Add or remove a tempo marker.
void PowerTabEditor::editTempoMarker()
{
    const Caret* caret = getCurrentScoreArea()->getCaret();
    Score* currentScore = caret->getCurrentScore();
    Score::TempoMarkerPtr marker = currentScore->FindTempoMarker(
                SystemLocation(caret->getCurrentSystemIndex(),
                               caret->getCurrentPositionIndex()));

    if (!marker) // Add a tempo marker.
    {
        TempoMarkerDialog dialog(this);
        if (dialog.exec() == QDialog::Accepted)
        {
            marker = boost::make_shared<TempoMarker>();
            marker->SetSystem(caret->getCurrentSystemIndex());
            marker->SetPosition(caret->getCurrentPositionIndex());
            marker->SetType(dialog.markerType());
            marker->SetTripletFeelType(dialog.tripletFeelType());

            if (dialog.markerType() == TempoMarker::listesso)
            {
                marker->SetListesso(dialog.beatType(), dialog.listessoBeatType(),
                                    dialog.description());
            }
            else if (dialog.markerType() == TempoMarker::standardMarker)
            {
                marker->SetStandardMarker(dialog.beatType(), dialog.beatsPerMinute(),
                                          dialog.description(), dialog.tripletFeelType());
            }

            undoManager->push(new AddTempoMarker(currentScore, marker),
                              caret->getCurrentSystemIndex());
        }
    }
    else
    {
        undoManager->push(new RemoveTempoMarker(currentScore, marker),
                          caret->getCurrentSystemIndex());
    }
}

void PowerTabEditor::editMusicalDirection()
{
    const Caret* caret = getCurrentScoreArea()->getCaret();
    boost::shared_ptr<System> system(caret->getCurrentSystem());
    boost::shared_ptr<Direction> dir(system->FindDirection(
                                         caret->getCurrentPositionIndex()));

    if (!dir)
    {
        DirectionDialog dialog(this);
        if (dialog.exec() == QDialog::Accepted)
        {
            dir = boost::make_shared<Direction>(dialog.getDirection());
            dir->SetPosition(caret->getCurrentPositionIndex());

            undoManager->push(new AddMusicalDirection(system, dir),
                              caret->getCurrentSystemIndex());
        }
        else
        {
            musicalDirectionAct->setChecked(false);
        }
    }
    else
    {
        undoManager->push(new RemoveMusicalDirection(system, dir),
                          caret->getCurrentSystemIndex());
    }
}

void PowerTabEditor::editHammerPull()
{
    Caret* caret = getCurrentScoreArea()->getCaret();
    Note* note = caret->getCurrentNote();
    const uint32_t voice = caret->getCurrentVoice();

    // TODO - allow editing of hammerons/pulloffs for a group of selected notes??
    std::vector<Note*> notes;
    notes.push_back(note);
    
    // if 'h' is pressed but there is no note do nothing
    if (note == NULL) return;
    
    Position* currentPosition = caret->getCurrentPosition();
    shared_ptr<const Staff> currentStaff = caret->getCurrentStaff();
    
    if (currentStaff->CanHammerOn(currentPosition, note, voice))
    {
        undoManager->push(new EditHammerPull(note, EditHammerPull::hammerOn),
                          caret->getCurrentSystemIndex());
    }
    else if (currentStaff->CanPullOff(currentPosition, note, voice))
    {
        undoManager->push(new EditHammerPull(note, EditHammerPull::pullOff),
                          caret->getCurrentSystemIndex());
    }
    else
    {
        hammerPullAct->setChecked(false);
    }
}

void PowerTabEditor::editTiedNote()
{
    Caret* caret = getCurrentScoreArea()->getCaret();

    std::vector<Note*> notes = getSelectedNotes();

    shared_ptr<const Staff> currentStaff = caret->getCurrentStaff();

    // check if all selected notes are able to be tied
    std::vector<Position*> positions = getSelectedPositions();

    for (size_t i = 0; i < positions.size(); i++)
    {
        Position* currentPosition = positions.at(i);
        for (size_t j = 0; j < currentPosition->GetNoteCount(); j++)
        {
            if (!currentStaff->CanTieNote(currentPosition, currentPosition->GetNote(j),
                                          caret->getCurrentVoice()))
            {
                tiedNoteAct->setChecked(false);
                return;
            }
        }
    }

    undoManager->push(new ToggleProperty<Note>(notes, &Note::SetTied, &Note::IsTied, "Note Tie"),
                      caret->getCurrentSystemIndex());
}

// Updates the given Command to be checked and/or enabled, based on the results of calling
// the predicate member function of the provided object
namespace
{
    template<class T>
    void updatePropertyStatus(Command* action, const T* object, bool (T::*predicate)(void) const)
    {
        if (object == NULL)
        {
            action->setChecked(false);
            action->setEnabled(false);
        }
        else
        {
            action->setEnabled(true);
            const bool propertySet = (object->*predicate)();
            action->setChecked(propertySet);
        }
    }
}

/// Updates whether menu items are checked, etc, whenever the caret moves.
void PowerTabEditor::updateActions()
{
    // Disable editing during playback.
    if (isPlaying)
    {
        updateScoreAreaActions(false);
        playPauseAct->setEnabled(true);
        return;
    }

    Caret* caret = getCurrentScoreArea()->getCaret();
    const Score* currentScore = caret->getCurrentScore();
    const quint32 caretPosition = caret->getCurrentPositionIndex();
    shared_ptr<const System> currentSystem = caret->getCurrentSystem();
    const Position* currentPosition = caret->getCurrentPosition();
    shared_ptr<const Barline> currentBarline = caret->getCurrentBarline();
    const Note* currentNote = caret->getCurrentNote();
    const bool hasSelection = caret->hasSelection();

    const bool onBarline = currentBarline != System::BarlineConstPtr();

    // Check for chord text
    chordNameAct->setChecked(currentSystem->HasChordText(caretPosition));

    // note and rest duration
    uint8_t duration = activeDuration;
    bool isRest = false;

    if (currentPosition != NULL)
    {
        duration = currentPosition->GetDurationType();
        isRest = currentPosition->IsRest();
    }

    if (isRest)
    {
        switch(duration)
        {
        case 1: wholeRestAct->setChecked(true); break;
        case 2: halfRestAct->setChecked(true); break;
        case 4: quarterRestAct->setChecked(true); break;
        case 8: eighthRestAct->setChecked(true); break;
        case 16: sixteenthRestAct->setChecked(true); break;
        case 32: thirtySecondRestAct->setChecked(true); break;
        case 64: sixtyFourthRestAct->setChecked(true); break;
        }

        wholeNoteAct->setChecked(false);
        halfNoteAct->setChecked(false);
        quarterNoteAct->setChecked(false);
        eighthNoteAct->setChecked(false);
        sixteenthNoteAct->setChecked(false);
        thirtySecondNoteAct->setChecked(false);
        sixtyFourthNoteAct->setChecked(false);
    }
    else
    {
        switch(duration)
        {
        case 1: wholeNoteAct->setChecked(true); break;
        case 2: halfNoteAct->setChecked(true); break;
        case 4: quarterNoteAct->setChecked(true); break;
        case 8: eighthNoteAct->setChecked(true); break;
        case 16: sixteenthNoteAct->setChecked(true); break;
        case 32: thirtySecondNoteAct->setChecked(true); break;
        case 64: sixtyFourthNoteAct->setChecked(true); break;
        }

        foreach (QAction* action, restDurationsGroup->actions())
        {
            action->setChecked(false);
        }
    }

    increaseDurationAct->setEnabled(duration != 1);
    decreaseDurationAct->setEnabled(duration != 64);

    const RehearsalSign* currentRehearsalSign = currentBarline ? &currentBarline->GetRehearsalSign() : NULL;
    updatePropertyStatus(rehearsalSignAct, currentRehearsalSign, &RehearsalSign::IsSet);

    const SystemLocation location(caret->getCurrentSystemIndex(),
                                  caret->getCurrentPositionIndex());

    shared_ptr<const TempoMarker> tempoMarker = currentScore->FindTempoMarker(location);
    tempoMarkerAct->setChecked(tempoMarker != shared_ptr<const TempoMarker>());

    shared_ptr<const AlternateEnding> altEnding = currentScore->FindAlternateEnding(location);
    repeatEndingAct->setChecked(altEnding != shared_ptr<const AlternateEnding>());

    musicalDirectionAct->setChecked(currentSystem->FindDirection(caretPosition));

    Score::DynamicPtr dynamic = currentScore->FindDynamic(caret->getCurrentSystemIndex(), caret->getCurrentStaffIndex(),
                                                          caret->getCurrentPositionIndex());
    dynamicAct->setChecked(dynamic != Score::DynamicPtr());

    if (onBarline) // current position is bar
    {
        barlineAct->setText(tr("Edit Barline"));
        barlineAct->setEnabled(true);
    }
    else if (currentPosition == NULL) // current position is empty
    {
        barlineAct->setText(tr("Insert Barline"));
        barlineAct->setEnabled(true);
    }
    else // current position has notes
    {
        barlineAct->setDisabled(true);
        barlineAct->setText(tr("Barline"));
    }

    standardBarlineAct->setEnabled(currentPosition == NULL && !onBarline);

    keySignatureAct->setEnabled(onBarline);
    timeSignatureAct->setEnabled(onBarline);

    updatePropertyStatus(naturalHarmonicAct, currentNote, &Note::IsNaturalHarmonic);
    updatePropertyStatus(artificialHarmonicAct, currentNote, &Note::HasArtificialHarmonic);
    updatePropertyStatus(tappedHarmonicAct, currentNote, &Note::HasTappedHarmonic);
    updatePropertyStatus(noteMutedAct, currentNote, &Note::IsMuted);
    updatePropertyStatus(ghostNoteAct, currentNote, &Note::IsGhostNote);
    updatePropertyStatus(tiedNoteAct, currentNote, &Note::IsTied);
    updatePropertyStatus(staccatoNoteAct, currentPosition, &Position::IsStaccato);
    updatePropertyStatus(hammerPullAct, currentNote, &Note::HasHammerOnOrPulloff);
    updatePropertyStatus(fermataAct, currentPosition, &Position::HasFermata);
    updatePropertyStatus(vibratoAct, currentPosition, &Position::HasVibrato);
    updatePropertyStatus(wideVibratoAct, currentPosition, &Position::HasWideVibrato);
    updatePropertyStatus(palmMuteAct, currentPosition, &Position::HasPalmMuting);
    updatePropertyStatus(letRingAct, currentPosition, &Position::HasLetRing);
    updatePropertyStatus(tremoloPickingAct, currentPosition, &Position::HasTremoloPicking);
    updatePropertyStatus(arpeggioUpAct, currentPosition, &Position::HasArpeggioUp);
    updatePropertyStatus(arpeggioDownAct, currentPosition, &Position::HasArpeggioDown);
    updatePropertyStatus(dottedNoteAct, currentPosition, &Position::IsDotted);
    updatePropertyStatus(doubleDottedNoteAct, currentPosition, &Position::IsDoubleDotted);
    updatePropertyStatus(marcatoAct, currentPosition, &Position::HasMarcato);
    updatePropertyStatus(sforzandoAct, currentPosition, &Position::HasSforzando);
    updatePropertyStatus(graceNoteAct, currentPosition, &Position::IsAcciaccatura);

    updatePropertyStatus(octave8vaAct, currentNote, &Note::IsOctave8va);
    updatePropertyStatus(octave15maAct, currentNote, &Note::IsOctave15ma);
    updatePropertyStatus(octave8vbAct, currentNote, &Note::IsOctave8vb);
    updatePropertyStatus(octave15mbAct, currentNote, &Note::IsOctave15mb);

    updatePropertyStatus(trillAction, currentNote, &Note::HasTrill);
    updatePropertyStatus(tapAct, currentPosition, &Position::HasTap);
    updatePropertyStatus(pickStrokeUpAct, currentPosition, &Position::HasPickStrokeUp);
    updatePropertyStatus(pickStrokeDownAct, currentPosition, &Position::HasPickStrokeDown);

    updatePropertyStatus(shiftSlideAct, currentNote, &Note::HasShiftSlide);
    updatePropertyStatus(legatoSlideAct, currentNote, &Note::HasLegatoSlide);
    updatePropertyStatus(slideOutOfDownwardsAct, currentNote, &Note::HasSlideOutOfDownwards);
    updatePropertyStatus(slideOutOfUpwardsAct, currentNote, &Note::HasSlideOutOfUpwards);
    updatePropertyStatus(slideIntoFromAboveAct, currentNote, &Note::HasSlideIntoFromAbove);
    updatePropertyStatus(slideIntoFromBelowAct, currentNote, &Note::HasSlideIntoFromBelow);

    updatePropertyStatus(volumeSwellAct, currentPosition, &Position::HasVolumeSwell);

    shiftBackwardAct->setEnabled(!currentPosition && (caretPosition == 0 || !currentBarline) &&
                                 !tempoMarker && !altEnding && !dynamic);

    clearNoteAct->setEnabled(currentNote != NULL);

    removeCurrentSystemAct->setEnabled(currentScore->GetSystemCount() > 1);

    clearCurrentPositionAct->setEnabled(currentPosition != NULL ||
            currentBarline || hasSelection);

    shiftTabNumDown->setEnabled(currentNote != NULL);
    shiftTabNumUp->setEnabled(currentNote != NULL);

    // irregular grouping
    tripletAct->setDisabled(true);
    tripletAct->setChecked(false);
    irregularGroupingAct->setDisabled(true);
    irregularGroupingAct->setChecked(false);

    if (currentPosition)
    {
        tripletAct->setEnabled(true);
        irregularGroupingAct->setEnabled(true);

        if (currentPosition->HasIrregularGroupingTiming())
        {
            uint8_t notesPlayed = 0, notesPlayedOver = 0;
            currentPosition->GetIrregularGroupingTiming(notesPlayed, notesPlayedOver);

            if (notesPlayed == 3 && notesPlayedOver == 2)
            {
                tripletAct->setChecked(true);
            }
            else
            {
                irregularGroupingAct->setChecked(true);
            }
        }
    }

    // Set up signal/slot connections for dot add/remove
    addDotAct->disconnect(SIGNAL(triggered()));
    removeDotAct->disconnect(SIGNAL(triggered()));

    if (currentPosition != NULL)
    {
        if (currentPosition->IsDotted())
        {
            connectToggleProperty<Position>(addDotAct, &getSelectedPositions,
                                            &Position::IsDoubleDotted, &Position::SetDoubleDotted);
            connectToggleProperty<Position>(addDotAct, &getSelectedPositions,
                                            &Position::IsDotted, &Position::SetDotted);

            connectToggleProperty<Position>(removeDotAct, &getSelectedPositions,
                                            &Position::IsDotted, &Position::SetDotted);

            addDotAct->setEnabled(true);
            removeDotAct->setEnabled(true);
        }
        else if (currentPosition->IsDoubleDotted())
        {
            connectToggleProperty<Position>(removeDotAct, &getSelectedPositions,
                                            &Position::IsDotted, &Position::SetDotted);
            connectToggleProperty<Position>(removeDotAct, &getSelectedPositions,
                                            &Position::IsDoubleDotted, &Position::SetDoubleDotted);

            addDotAct->setEnabled(false);
            removeDotAct->setEnabled(true);
        }
        else
        {
            connectToggleProperty<Position>(addDotAct, &getSelectedPositions,
                                            &Position::IsDotted, &Position::SetDotted);

            addDotAct->setEnabled(true);
            removeDotAct->setEnabled(false);
        }
    }
}

void PowerTabEditor::updateLocationLabel()
{
    getCurrentPlaybackWidget()->updateLocationLabel(
                getCurrentScoreArea()->getCaret()->toString());
}

void PowerTabEditor::addRest()
{
    editRest(activeDuration);
}

void PowerTabEditor::editRest(uint8_t duration)
{
    const Caret* caret = getCurrentScoreArea()->getCaret();
    Position* currentPosition = caret->getCurrentPosition();
    System::StaffPtr currentStaff = caret->getCurrentStaff();
    const uint32_t currentVoice = caret->getCurrentVoice();
    const uint32_t currentPositionIndex = caret->getCurrentPositionIndex();

    undoManager->push(new EditRest(currentPosition, currentStaff, currentPositionIndex, currentVoice, duration),
                      caret->getCurrentSystemIndex());
}

void PowerTabEditor::editSlideOutOf(uint8_t newSlideType)
{
    Caret* caret = getCurrentScoreArea()->getCaret();
    Note* note = caret->getCurrentNote();
    Position* position = caret->getCurrentPosition();
    const uint32_t voice = caret->getCurrentVoice();

    qint8 newSteps = 0;

    // for shift/legato slides, get the number of steps we will shift
    if (newSlideType == Note::slideOutOfLegatoSlide || newSlideType == Note::slideOutOfShiftSlide)
    {
        shared_ptr<const Staff> staff = caret->getCurrentStaff();

        // if we can't do a slide, uncheck the action that was just pressed and abort
        if (!staff->CanSlideBetweenNotes(position, note, voice))
        {
            if (newSlideType == Note::slideOutOfLegatoSlide)
                legatoSlideAct->setChecked(false);
            else if (newSlideType == Note::slideOutOfShiftSlide)
                shiftSlideAct->setChecked(false);

            return;
        }

        newSteps = caret->getCurrentStaff()->GetSlideSteps(position, note, voice);
    }

    // find what the current slide is set to - if it is the same, remove the slide
    quint8 currentType = 0;
    qint8 steps = 0;
    note->GetSlideOutOf(currentType, steps);
    if (currentType == newSlideType)
    {
        newSlideType = Note::slideOutOfNone;
        newSteps = 0;
    }

    undoManager->push(new EditSlideOut(note, newSlideType, newSteps),
                      caret->getCurrentSystemIndex());
}

void PowerTabEditor::editSlideInto(uint8_t newSlideIntoType)
{
    Q_ASSERT(newSlideIntoType == Note::slideIntoFromBelow ||
             newSlideIntoType == Note::slideIntoFromAbove);

    const Caret* caret = getCurrentScoreArea()->getCaret();
    Note* note = caret->getCurrentNote();

    // get the current slide type - if it is the same as the new type, we remove the slide
    uint8_t currentType = 0;
    note->GetSlideInto(currentType);
    if (currentType == newSlideIntoType)
    {
        newSlideIntoType = Note::slideIntoNone;
    }

    undoManager->push(new EditSlideInto(note, newSlideIntoType),
                      caret->getCurrentSystemIndex());
}

void PowerTabEditor::doPaste()
{
    if (!Clipboard::hasData())
    {
        QMessageBox msg(this);
        msg.setText(QObject::tr("The clipboard does not contain any notes."));
        msg.exec();
        return;
    }

    undoManager->beginMacro(tr("Paste Notes"));

    // If there are any selected notes, delete them before pasting.
    if (getCurrentScoreArea()->getCaret()->hasSelection())
    {
        clearCurrentPosition();
    }

    Clipboard::paste(this, undoManager.get(), getCurrentScoreArea()->getCaret());

    undoManager->endMacro();
}

void PowerTabEditor::copySelectedNotes()
{
    const Caret* caret = getCurrentScoreArea()->getCaret();
    const std::vector<Position*> positions = caret->getSelectedPositions();
    const Tuning& tuning = caret->getCurrentScore()->GetGuitar(
                caret->getCurrentStaffIndex())->GetTuning();

    Clipboard::copySelection(getSelectedPositions(), tuning);
}

void PowerTabEditor::cutSelectedNotes()
{
    undoManager->beginMacro(tr("Cut Notes"));
    copySelectedNotes();
    clearCurrentPosition();
    undoManager->endMacro();
}

void PowerTabEditor::editVolumeSwell()
{
    Caret* caret = getCurrentScoreArea()->getCaret();
    Position* position = caret->getCurrentPosition();

    if (!position->HasVolumeSwell())
    {
        VolumeSwellDialog dialog(this, position);
        if (dialog.exec() == QDialog::Accepted) // add volume swell
        {
            undoManager->push(new AddVolumeSwell(position, dialog.getNewStartVolume(),
                                                 dialog.getNewEndVolume(),
                                                 dialog.getNewDuration()),
                              caret->getCurrentSystemIndex());
        }
    }
    else // remove volume swell
    {
        undoManager->push(new RemoveVolumeSwell(position),
                          caret->getCurrentSystemIndex());
    }
}

void PowerTabEditor::editIrregularGrouping(bool setAsTriplet)
{
    const Caret* caret = getCurrentScoreArea()->getCaret();
    Position* selectedPosition = caret->getCurrentPosition();

    if (selectedPosition->HasIrregularGroupingTiming())
    {
        undoManager->push(new RemoveIrregularGrouping(caret->getCurrentStaff(),
                                                      selectedPosition,
                                                      caret->getCurrentVoice()),
                          caret->getCurrentSystemIndex());
    }
    else
    {
        std::vector<Position*> selectedPositions = getSelectedPositions();

        // check if we are going to be adding or removing an irregular grouping
        // - if one of the selected notes is part of an irregular group, remove the group
        for (size_t i = 0; i < selectedPositions.size(); i++)
        {
            if (selectedPositions[i]->HasIrregularGroupingTiming())
            {
                QMessageBox msgBox;
                msgBox.setText(tr("One or more notes is already part of an irregular group!!!"));
                msgBox.exec();
                return;
            }
        }

        if (setAsTriplet)
        {
            undoManager->push(new AddIrregularGrouping(selectedPositions, 3, 2),
                              caret->getCurrentSystemIndex());
        }
        else
        {
            IrregularGroupingDialog dialog(this);

            if (dialog.exec() == QDialog::Accepted)
            {
                undoManager->push(new AddIrregularGrouping(selectedPositions,
                                                           dialog.notesPlayed(),
                                                           dialog.notesPlayedOver()),
                                  caret->getCurrentSystemIndex());
            }
        }
    }
}

void PowerTabEditor::toggleGuitarVisible(uint32_t trackIndex, bool isVisible)
{
    Caret* caret = getCurrentScoreArea()->getCaret();
    Score* score = caret->getCurrentScore();
    Mixer* mixer = getCurrentMixer();

    // There must always be at least one visible guitar.
    bool canToggle = false;
    for (size_t i = 0; i < score->GetGuitarCount(); ++i)
    {
        if (score->GetGuitar(i)->IsShown() && i != trackIndex)
        {
            canToggle = true;
        }
    }

    if (canToggle)
    {
        EditTrackShown* action = new EditTrackShown(score, mixer, trackIndex,
                                                    isVisible);
        undoManager->push(action, UndoManager::AFFECTS_ALL_SYSTEMS);
    }
    else
    {
        // Update the state of the checkboxes if we didn't show/hide the guitar.
        mixer->update();
    }

}

void PowerTabEditor::openFileInformation()
{
    boost::shared_ptr<PowerTabDocument> doc = documentManager->getCurrentDocument();
    FileInformationDialog dialog(this, doc);

    if (dialog.exec() == QDialog::Accepted)
    {
        const PowerTabFileHeader newHeader = dialog.getNewFileHeader();
        if (newHeader != doc->GetHeader())
        {
            undoManager->push(new EditFileInformation(doc, newHeader),
                              UndoManager::AFFECTS_ALL_SYSTEMS);
        }
    }
}

#endif
