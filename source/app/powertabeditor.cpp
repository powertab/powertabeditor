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

#include <app/caret.h>
#include <app/command.h>
#include <app/documentmanager.h>
#include <app/recentfiles.h>
#include <app/scorearea.h>
#include <app/settings.h>
#include <boost/bind.hpp>
#include <boost/foreach.hpp>
#include <boost/timer.hpp>
#include <dialogs/keyboardsettingsdialog.h>
#include <formats/fileformatmanager.h>
#include <QCoreApplication>
#include <QDebug>
#include <QFileDialog>
#include <QFontDatabase>
#include <QMenuBar>
#include <QMessageBox>
#include <QSettings>
#include <sigfwd/sigfwd.hpp>

#if 0
#include <QFileDialog>
#include <QAction>
#include <QMenuBar>
#include <QMenu>
#include <QToolBox>
#include <QTabWidget>
#include <QSettings>
#include <QFileInfo>
#include <QSplitter>
#include <QScrollArea>
#include <QStackedWidget>
#include <QActionGroup>
#include <QKeyEvent>
#include <QEvent>

#include <boost/make_shared.hpp>

#include <app/scorearea.h>
#include <app/skinmanager.h>
#include <audio/midiplayer.h>
#include <app/settings.h>
#include <app/documentmanager.h>
#include <app/command.h>
#include <app/clipboard.h>
#include <app/pubsub/settingspubsub.h>
#include <app/pubsub/systemlocationpubsub.h>
#include <app/tuningdictionary.h>

#include <dialogs/preferencesdialog.h>
#include <dialogs/chordnamedialog.h>
#include <dialogs/rehearsalsigndialog.h>
#include <dialogs/trilldialog.h>
#include <dialogs/barlinedialog.h>
#include <dialogs/alternateendingdialog.h>
#include <dialogs/tappedharmonicdialog.h>
#include <dialogs/keysignaturedialog.h>
#include <dialogs/timesignaturedialog.h>
#include <dialogs/keyboardsettingsdialog.h>
#include <dialogs/dynamicdialog.h>
#include <dialogs/volumeswelldialog.h>
#include <dialogs/irregulargroupingdialog.h>
#include <dialogs/fileinformationdialog.h>
#include <dialogs/gotorehearsalsigndialog.h>
#include <dialogs/tempomarkerdialog.h>
#include <dialogs/gotobarlinedialog.h>
#include <dialogs/tuningdictionarydialog.h>
#include <dialogs/artificialharmonicdialog.h>
#include <dialogs/directiondialog.h>

#include <powertabdocument/powertabdocument.h>
#include <powertabdocument/guitar.h>
#include <powertabdocument/system.h>
#include <powertabdocument/chordtext.h>
#include <powertabdocument/score.h>
#include <powertabdocument/note.h>
#include <powertabdocument/staff.h>
#include <powertabdocument/barline.h>
#include <powertabdocument/systemlocation.h>
#include <powertabdocument/alternateending.h>
#include <powertabdocument/dynamic.h>

#include <widgets/mixer/mixer.h>
#include <widgets/toolbox/toolbox.h>
#include <widgets/playback/playbackwidget.h>

#include <painters/caret.h>

#include <actions/undomanager.h>
#include <actions/removechordtext.h>
#include <actions/addchordtext.h>
#include <actions/updatenoteduration.h>
#include <actions/editrehearsalsign.h>
#include <actions/toggleproperty.h>
#include <actions/shifttabnumber.h>
#include <actions/changepositionspacing.h>
#include <actions/removetrill.h>
#include <actions/addtrill.h>
#include <actions/editslideout.h>
#include <actions/updatetabnumber.h>
#include <actions/positionshift.h>
#include <actions/changebarlinetype.h>
#include <actions/addbarline.h>
#include <actions/deletebarline.h>
#include <actions/deleteposition.h>
#include <actions/removesystem.h>
#include <actions/addsystem.h>
#include <actions/addguitar.h>
#include <actions/addnote.h>
#include <actions/removealternateending.h>
#include <actions/addalternateending.h>
#include <actions/editslideinto.h>
#include <actions/removetappedharmonic.h>
#include <actions/addtappedharmonic.h>
#include <actions/editrest.h>
#include <actions/editkeysignature.h>
#include <actions/edittimesignature.h>
#include <actions/adddynamic.h>
#include <actions/removedynamic.h>
#include <actions/removevolumeswell.h>
#include <actions/addvolumeswell.h>
#include <actions/addirregulargrouping.h>
#include <actions/removeirregulargrouping.h>
#include <actions/edittrackshown.h>
#include <actions/deletenote.h>
#include <actions/editfileinformation.h>
#include <actions/removetempomarker.h>
#include <actions/addtempomarker.h>
#include <actions/edithammerpull.h>
#include <actions/addartificialharmonic.h>
#include <actions/removeartificialharmonic.h>
#include <actions/addmusicaldirection.h>
#include <actions/removemusicaldirection.h>

boost::scoped_ptr<UndoManager> PowerTabEditor::undoManager(new UndoManager);

#endif

PowerTabEditor::PowerTabEditor() :
    QMainWindow(0),
    myDocumentManager(new DocumentManager()),
    myFileFormatManager(new FileFormatManager()),
    myPreviousDirectory(QSettings().value(Settings::APP_PREVIOUS_DIRECTORY,
                                          QDir::homePath()).toString()),
    myRecentFiles(NULL),
    myTabWidget(NULL)
#if 0
    activeDuration(Position::DEFAULT_DURATION_TYPE),
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

    connect(undoManager.get(), SIGNAL(redrawNeeded(int)), this, SLOT(redrawSystem(int)));
    connect(undoManager.get(), SIGNAL(fullRedrawNeeded()), this, SLOT(performFullRedraw()));
    connect(undoManager.get(), SIGNAL(cleanChanged(bool)), this, SLOT(updateModified(bool)));
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
    myDocumentManager->addDocument();
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
    undoManager->setActiveStackIndex(index);
#endif

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

    undoManager->removeStack(index);
#endif
    myDocumentManager->removeDocument(index);
    delete myTabWidget->widget(index);
#if 0
    mixerList->removeWidget(mixerList->widget(index));
    playbackToolbarList->removeWidget(playbackToolbarList->widget(index));
#endif
    // Get the index of the tab that we will now switch to.
    const int currentIndex = myTabWidget->currentIndex();

#if 0
    undoManager->setActiveStackIndex(currentIndex);
    mixerList->setCurrentIndex(currentIndex);
    playbackToolbarList->setCurrentIndex(currentIndex);
#endif
    myDocumentManager->setCurrentDocumentIndex(currentIndex);

#if 0
    if (currentIndex == -1) // disable score-related actions if no documents are open
    {
        updateScoreAreaActions(false);
    }
#endif

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

void PowerTabEditor::moveCaretRight()
{
    getCaret().moveHorizontal(1);
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
#if 0

    // Redo / Undo actions
    undoAct = undoManager->createUndoAction(this, tr("&Undo"));
    undoAct->setShortcuts(QKeySequence::Undo);

    redoAct = undoManager->createRedoAction(this, tr("&Redo"));
    redoAct->setShortcuts(QKeySequence::Redo);

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

    // Section navigation actions
    firstSectionAct = new Command(tr("First Section"), "Section.FirstSection",
                                  QKeySequence::MoveToStartOfDocument, this);
    connect(firstSectionAct, SIGNAL(triggered()), this, SLOT(moveCaretToFirstSection()));

    nextSectionAct = new Command(tr("Next Section"), "Section.NextSection",
                                 QKeySequence::MoveToNextPage, this);
    connect(nextSectionAct, SIGNAL(triggered()), this, SLOT(moveCaretToNextSection()));

    prevSectionAct = new Command(tr("Previous Section"), "Section.PreviousSection",
                                 QKeySequence::MoveToPreviousPage, this);
    connect(prevSectionAct, SIGNAL(triggered()), this, SLOT(moveCaretToPrevSection()));

    lastSectionAct = new Command(tr("Last Section"), "Section.LastSection",
                                 QKeySequence::MoveToEndOfDocument, this);
    connect(lastSectionAct, SIGNAL(triggered()), this, SLOT(moveCaretToLastSection()));

    shiftForwardAct = new Command(tr("Shift Forward"), "Position.ShiftForward",
                                  QKeySequence(Qt::Key_Insert), this);
    connect(shiftForwardAct, SIGNAL(triggered()), this, SLOT(shiftForward()));

    shiftBackwardAct = new Command(tr("Shift Backward"), "Position.ShiftBackward",
                                   QKeySequence(Qt::SHIFT + Qt::Key_Insert), this);
    connect(shiftBackwardAct, SIGNAL(triggered()), this, SLOT(shiftBackward()));

    // Position-related actions
    startPositionAct = new Command(tr("Move to &Start"), "Staff.MoveToStart",
                                   QKeySequence::MoveToStartOfLine, this);
    connect(startPositionAct, SIGNAL(triggered()), this, SLOT(moveCaretToStart()));

#endif
    myNextPositionCommand = new Command(tr("&Next Position"),
                                        "Staff.NextPosition",
                                        QKeySequence::MoveToNextChar, this);
    connect(myNextPositionCommand, SIGNAL(triggered()), this,
            SLOT(moveCaretRight()));

#if 0
    prevPositionAct = new Command(tr("&Previous Position"), "Staff.PreviousPosition",
                                  QKeySequence::MoveToPreviousChar, this);
    connect(prevPositionAct, SIGNAL(triggered()), this, SLOT(moveCaretLeft()));

    nextStringAct = new Command(tr("Next String"), "Staff.NextString",
                                QKeySequence::MoveToNextLine, this);
    connect(nextStringAct, SIGNAL(triggered()), this, SLOT(moveCaretDown()));

    prevStringAct = new Command(tr("Previous String"), "Staff.PreviousString",
                                QKeySequence::MoveToPreviousLine, this);
    connect(prevStringAct, SIGNAL(triggered()), this, SLOT(moveCaretUp()));

    lastPositionAct = new Command(tr("Move to &End"), "Staff.MoveToEnd",
                                  QKeySequence::MoveToEndOfLine, this);
    connect(lastPositionAct, SIGNAL(triggered()), this, SLOT(moveCaretToEnd()));

    nextStaffAct = new Command(tr("Next Staff"), "Staff.NextStaff",
                               Qt::ALT + Qt::Key_Down, this);
    connect(nextStaffAct, SIGNAL(triggered()), this, SLOT(moveCaretToNextStaff()));

    prevStaffAct = new Command(tr("Previous Staff"), "Staff.PreviousStaff",
                               Qt::ALT + Qt::Key_Up, this);
    connect(prevStaffAct, SIGNAL(triggered()), this, SLOT(moveCaretToPrevStaff()));

    // the shortcuts for these two won't do anything since the tabs get
    // sucked up by our event filter first
    nextBarAct = new Command(tr("Next Bar"), "Staff.NextBar", Qt::Key_Tab, this);
    connect(nextBarAct, SIGNAL(triggered()), this, SLOT(moveCaretToNextBar()));

    prevBarAct = new Command(tr("Previous Bar"), "Staff.PreviousBar", Qt::SHIFT + Qt::Key_Tab, this);
    connect(prevBarAct, SIGNAL(triggered()), this, SLOT(moveCaretToPrevBar()));

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

    gotoBarlineAct = new Command(tr("Go To Barline..."), "Position.GoToBarline",
                                 Qt::CTRL + Qt::Key_G, this);
    connect(gotoBarlineAct, SIGNAL(triggered()), this, SLOT(gotoBarline()));

    gotoRehearsalSignAct = new Command(tr("Go To Rehearsal Sign..."),
                                       "Position.GoToRehearsalSign",
                                       Qt::CTRL + Qt::Key_H, this);
    connect(gotoRehearsalSignAct, SIGNAL(triggered()),
            this, SLOT(gotoRehearsalSign()));

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

    removeCurrentSystemAct = new Command(tr("Remove Current System"),
                    "Section.RemoveCurrentSystem",
                    QKeySequence(Qt::CTRL + Qt::SHIFT + Qt::Key_N), this);
    connect(removeCurrentSystemAct, SIGNAL(triggered()), this, SLOT(removeCurrentSystem()));

    insertSystemAtEndAct = new Command(tr("Insert System At End"),
                    "Section.InsertSystemAtEnd", Qt::Key_N, this);
    connect(insertSystemAtEndAct, SIGNAL(triggered()), this, SLOT(insertSystemAtEnd()));

    insertSystemBeforeAct = new Command(tr("Insert System Before"),
                    "Section.InsertSystemBefore",
                    QKeySequence(Qt::ALT + Qt::SHIFT + Qt::Key_N), this);
    connect(insertSystemBeforeAct, SIGNAL(triggered()), this, SLOT(insertSystemBefore()));

    insertSystemAfterAct = new Command(tr("Insert System After"),
                    "Section.InsertSystemAfter",
                    QKeySequence(Qt::SHIFT + Qt::Key_N), this);
    connect(insertSystemAfterAct, SIGNAL(triggered()), this, SLOT(insertSystemAfter()));

    // Note-related actions
    noteDurationActGroup = new QActionGroup(this);

    wholeNoteAct = new Command(tr("Whole"), "Note.WholeNote", QKeySequence(), this);
    wholeNoteAct->setCheckable(true);
    sigfwd::connect(wholeNoteAct, SIGNAL(triggered()),
                    boost::bind(&PowerTabEditor::updateNoteDuration, this, 1));
    noteDurationActGroup->addAction(wholeNoteAct);

    halfNoteAct = new Command(tr("Half"), "Note.HalfNote", QKeySequence(), this);
    halfNoteAct->setCheckable(true);
    sigfwd::connect(halfNoteAct, SIGNAL(triggered()),
                    boost::bind(&PowerTabEditor::updateNoteDuration, this, 2));
    noteDurationActGroup->addAction(halfNoteAct);

    quarterNoteAct = new Command(tr("Quarter"), "Note.QuarterNote", QKeySequence(), this);
    quarterNoteAct->setCheckable(true);
    sigfwd::connect(quarterNoteAct, SIGNAL(triggered()),
                    boost::bind(&PowerTabEditor::updateNoteDuration, this, 4));
    noteDurationActGroup->addAction(quarterNoteAct);

    eighthNoteAct = new Command(tr("8th"), "Note.EighthNote", QKeySequence(), this);
    eighthNoteAct->setCheckable(true);
    sigfwd::connect(eighthNoteAct, SIGNAL(triggered()),
                    boost::bind(&PowerTabEditor::updateNoteDuration, this, 8));
    noteDurationActGroup->addAction(eighthNoteAct);

    sixteenthNoteAct = new Command(tr("16th"), "Note.SixteenthNote", QKeySequence(), this);
    sixteenthNoteAct->setCheckable(true);
    sigfwd::connect(sixteenthNoteAct, SIGNAL(triggered()),
                    boost::bind(&PowerTabEditor::updateNoteDuration, this, 16));
    noteDurationActGroup->addAction(sixteenthNoteAct);

    thirtySecondNoteAct = new Command(tr("32nd"), "Note.ThirtySecondNote", QKeySequence(), this);
    thirtySecondNoteAct->setCheckable(true);
    sigfwd::connect(thirtySecondNoteAct, SIGNAL(triggered()),
                    boost::bind(&PowerTabEditor::updateNoteDuration, this, 32));
    noteDurationActGroup->addAction(thirtySecondNoteAct);

    sixtyFourthNoteAct = new Command(tr("64th"), "Note.SixtyFourthNote", QKeySequence(), this);
    sixtyFourthNoteAct->setCheckable(true);
    sigfwd::connect(sixtyFourthNoteAct, SIGNAL(triggered()),
                    boost::bind(&PowerTabEditor::updateNoteDuration, this, 64));
    noteDurationActGroup->addAction(sixtyFourthNoteAct);

    increaseDurationAct = new Command(tr("Increase Duration"),
                                         "Note.IncreaseDuration", Qt::SHIFT + Qt::Key_Up, this);
    sigfwd::connect(increaseDurationAct, SIGNAL(triggered()),
                    boost::bind(&PowerTabEditor::changeNoteDuration, this, true));

    decreaseDurationAct = new Command(tr("Decrease Duration"),
                                         "Note.DecreaseDuration", Qt::SHIFT + Qt::Key_Down, this);
    sigfwd::connect(decreaseDurationAct, SIGNAL(triggered()),
                    boost::bind(&PowerTabEditor::changeNoteDuration, this, false));

    dottedNoteAct = new Command(tr("Dotted"), "Note.Dotted", QKeySequence(), this);
    dottedNoteAct->setCheckable(true);
    connectToggleProperty<Position>(dottedNoteAct, &getSelectedPositions,
                                    &Position::IsDotted, &Position::SetDotted);

    doubleDottedNoteAct = new Command(tr("Double Dotted"), "Note.DoubleDotted", QKeySequence(), this);
    doubleDottedNoteAct->setCheckable(true);
    connectToggleProperty<Position>(doubleDottedNoteAct, &getSelectedPositions,
                                    &Position::IsDoubleDotted, &Position::SetDoubleDotted);

    addDotAct = new Command(tr("Add Dot"), "Note.AddDot", Qt::SHIFT + Qt::Key_Right, this);
    removeDotAct = new Command(tr("Remove Dot"), "Note.RemoveDot", Qt::SHIFT + Qt::Key_Left, this);

    tiedNoteAct = new Command(tr("Tied"), "Note.Tied", Qt::Key_Y, this);
    tiedNoteAct->setCheckable(true);
    connect(tiedNoteAct, SIGNAL(triggered()), this, SLOT(editTiedNote()));

    noteMutedAct = new Command(tr("Muted"), "Note.Muted",
                               QKeySequence(Qt::Key_X), this);
    noteMutedAct->setCheckable(true);
    connectToggleProperty<Note>(noteMutedAct, &getSelectedNotes,
                                &Note::IsMuted, &Note::SetMuted);

    ghostNoteAct = new Command(tr("Ghost Note"), "Note.GhostNote", Qt::Key_G, this);
    ghostNoteAct->setCheckable(true);
    connectToggleProperty<Note>(ghostNoteAct, &getSelectedNotes,
                                &Note::IsGhostNote, &Note::SetGhostNote);

    fermataAct = new Command(tr("Fermata"), "Note.Fermata", Qt::Key_F, this);
    fermataAct->setCheckable(true);
    connectToggleProperty<Position>(fermataAct, &getSelectedPositions,
                                    &Position::HasFermata, &Position::SetFermata);

    letRingAct = new Command(tr("Let Ring"), "Note.LetRing", QKeySequence(), this);
    letRingAct->setCheckable(true);
    connectToggleProperty<Position>(letRingAct, &getSelectedPositions,
                                    &Position::HasLetRing, &Position::SetLetRing);

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

    // Octave actions
    octave8vaAct = new Command(tr("8va"), "Note.Octave8va", QKeySequence(), this);
    octave8vaAct->setCheckable(true);
    connectToggleProperty<Note>(octave8vaAct, &getSelectedNotes,
                                &Note::IsOctave8va, &Note::SetOctave8va);

    octave15maAct = new Command(tr("15ma"), "Note.Octave15ma", QKeySequence(), this);
    octave15maAct->setCheckable(true);
    connectToggleProperty<Note>(octave15maAct, &getSelectedNotes,
                                &Note::IsOctave15ma, &Note::SetOctave15ma);

    octave8vbAct = new Command(tr("8vb"), "Note.Octave8vb", QKeySequence(), this);
    octave8vbAct->setCheckable(true);
    connectToggleProperty<Note>(octave8vbAct, &getSelectedNotes,
                                &Note::IsOctave8vb, &Note::SetOctave8vb);

    octave15mbAct = new Command(tr("15mb"), "Note.Octave15mb", QKeySequence(), this);
    octave15mbAct->setCheckable(true);
    connectToggleProperty<Note>(octave15mbAct, &getSelectedNotes,
                                &Note::IsOctave15mb, &Note::SetOctave15mb);

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

    // Music Symbol Actions
    rehearsalSignAct = new Command(tr("Rehearsal Sign..."), "MusicSymbols.EditRehearsalSign",
                                   Qt::SHIFT + Qt::Key_R, this);
    rehearsalSignAct->setCheckable(true);
    connect(rehearsalSignAct, SIGNAL(triggered()), this, SLOT(editRehearsalSign()));

    tempoMarkerAct = new Command(tr("Tempo Marker..."),"MusicSymbols.EditTempoMarker",
                                 Qt::Key_O, this);
    tempoMarkerAct->setCheckable(true);
    connect(tempoMarkerAct, SIGNAL(triggered()), this, SLOT(editTempoMarker()));

    keySignatureAct = new Command(tr("Edit Key Signature..."), "MusicSymbols.EditKeySignature",
                                  Qt::Key_K, this);
    connect(keySignatureAct, SIGNAL(triggered()), this,
            SLOT(editKeySignatureFromCaret()));

    timeSignatureAct = new Command(tr("Edit Time Signature..."), "MusicSymbols.EditTimeSignature",
                                   Qt::Key_T, this);
    connect(timeSignatureAct, SIGNAL(triggered()), this,
            SLOT(editTimeSignatureFromCaret()));

    barlineAct = new Command(tr("Barline..."), "MusicSymbols.EditBarline",
            Qt::SHIFT + Qt::Key_B, this);
    connect(barlineAct, SIGNAL(triggered()), this, SLOT(editBarlineFromCaret()));

    standardBarlineAct = new Command(tr("Insert Standard Barline"),
            "MusicSymbols.InsertStandardBarline", Qt::Key_B, this);
    connect(standardBarlineAct, SIGNAL(triggered()), this,
            SLOT(insertStandardBarline()));

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

    dynamicAct = new Command(tr("Dynamic..."), "MusicSymbols.EditDynamic", Qt::Key_D, this);
    dynamicAct->setCheckable(true);
    connect(dynamicAct, SIGNAL(triggered()), this, SLOT(editDynamic()));

    volumeSwellAct = new Command(tr("Volume Swell ..."), "MusicSymbols.VolumeSwell",
                                    QKeySequence(), this);
    volumeSwellAct->setCheckable(true);
    connect(volumeSwellAct, SIGNAL(triggered()), this, SLOT(editVolumeSwell()));

    // Tab Symbol Actions
    hammerPullAct = new Command(tr("Hammer On/Pull Off"), "TabSymbols.HammerPull", Qt::Key_H, this);
    hammerPullAct->setCheckable(true);
    connect(hammerPullAct, SIGNAL(triggered()), this, SLOT(editHammerPull()));

    naturalHarmonicAct = new Command(tr("Natural Harmonic"), "TabSymbols.NaturalHarmonic",
                                     QKeySequence(), this);
    naturalHarmonicAct->setCheckable(true);
    connectToggleProperty<Note>(naturalHarmonicAct, &getSelectedNotes,
                                &Note::IsNaturalHarmonic, &Note::SetNaturalHarmonic);

    artificialHarmonicAct = new Command(tr("Artificial Harmonic..."),
            "TabSymbols.ArtificialHarmonic", QKeySequence(), this);
    artificialHarmonicAct->setCheckable(true);
    connect(artificialHarmonicAct, SIGNAL(triggered()),
            this, SLOT(editArtificialHarmonic()));

    tappedHarmonicAct = new Command(tr("Tapped Harmonic..."),
            "TabSymbols.TappedHarmonic", QKeySequence(), this);
    tappedHarmonicAct->setCheckable(true);
    connect(tappedHarmonicAct, SIGNAL(triggered()),
            this, SLOT(editTappedHarmonic()));

    shiftSlideAct = new Command(tr("Shift Slide"), "TabSymbols.ShiftSlide", Qt::Key_S, this);
    shiftSlideAct->setCheckable(true);
    sigfwd::connect(shiftSlideAct, SIGNAL(triggered()),
                    boost::bind(&PowerTabEditor::editSlideOutOf, this, Note::slideOutOfShiftSlide));

    legatoSlideAct = new Command(tr("Legato Slide"), "TabSymbols.LegatoSlide", Qt::Key_L, this);
    legatoSlideAct->setCheckable(true);
    sigfwd::connect(legatoSlideAct, SIGNAL(triggered()),
                    boost::bind(&PowerTabEditor::editSlideOutOf, this, Note::slideOutOfLegatoSlide));

    vibratoAct = new Command(tr("Vibrato"), "TabSymbols.Vibrato", Qt::Key_V, this);
    vibratoAct->setCheckable(true);
    connectToggleProperty<Position>(vibratoAct, &getSelectedPositions,
                                    &Position::HasVibrato, &Position::SetVibrato);

    wideVibratoAct = new Command(tr("Wide Vibrato"), "TabSymbols.WideVibrato", Qt::Key_W, this);
    wideVibratoAct->setCheckable(true);
    connectToggleProperty<Position>(wideVibratoAct, &getSelectedPositions,
                                    &Position::HasWideVibrato, &Position::SetWideVibrato);

    palmMuteAct = new Command(tr("Palm Mute"), "TabSymbols.PalmMute", Qt::Key_M, this);
    palmMuteAct->setCheckable(true);
    connectToggleProperty<Position>(palmMuteAct, &getSelectedPositions,
                                    &Position::HasPalmMuting, &Position::SetPalmMuting);

    tremoloPickingAct = new Command(tr("Tremolo Picking"), "TabSymbols.TremoloPicking", QKeySequence(), this);
    tremoloPickingAct->setCheckable(true);
    connectToggleProperty<Position>(tremoloPickingAct, &getSelectedPositions,
                                    &Position::HasTremoloPicking, &Position::SetTremoloPicking);

    arpeggioUpAct = new Command(tr("Arpeggio Up"), "TabSymbols.ArpeggioUp", QKeySequence(), this);
    arpeggioUpAct->setCheckable(true);
    connectToggleProperty<Position>(arpeggioUpAct, &getSelectedPositions,
                                    &Position::HasArpeggioUp, &Position::SetArpeggioUp);

    arpeggioDownAct = new Command(tr("Arpeggio Down"), "TabSymbols.ArpeggioDown", QKeySequence(), this);
    arpeggioDownAct->setCheckable(true);
    connectToggleProperty<Position>(arpeggioDownAct, &getSelectedPositions,
                                    &Position::HasArpeggioDown, &Position::SetArpeggioDown);

    tapAct = new Command(tr("Tap"), "TabSymbols.Tap", Qt::Key_P, this);
    tapAct->setCheckable(true);
    connectToggleProperty<Position>(tapAct, &getSelectedPositions,
                                    &Position::HasTap, &Position::SetTap);

    trillAction = new Command(tr("Trill..."), "TabSymbols.Trill",
                              QKeySequence(), this);
    trillAction->setCheckable(true);
    connect(trillAction, SIGNAL(triggered()), this, SLOT(editTrill()));

    pickStrokeUpAct = new Command(tr("Pickstroke Up"), "TabSymbols.PickstrokeUp", QKeySequence(), this);
    pickStrokeUpAct->setCheckable(true);
    connectToggleProperty<Position>(pickStrokeUpAct, &getSelectedPositions,
                                    &Position::HasPickStrokeUp, &Position::SetPickStrokeUp);

    pickStrokeDownAct = new Command(tr("Pickstroke Down"), "TabSymbols.PickStrokeDown",
                                    QKeySequence(), this);
    pickStrokeDownAct->setCheckable(true);
    connectToggleProperty<Position>(pickStrokeDownAct, &getSelectedPositions,
                                    &Position::HasPickStrokeDown, &Position::SetPickStrokeDown);

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
#if 0
    // Edit Menu
    editMenu = menuBar()->addMenu(tr("&Edit"));
    editMenu->addAction(undoAct);
    editMenu->addAction(redoAct);
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
#if 0
    positionSectionMenu = positionMenu->addMenu(tr("&Section"));
    positionSectionMenu->addAction(firstSectionAct);
    positionSectionMenu->addAction(nextSectionAct);
    positionSectionMenu->addAction(prevSectionAct);
    positionSectionMenu->addAction(lastSectionAct);

#endif
    myPositionStaffMenu = myPositionMenu->addMenu(tr("&Staff"));
#if 0
    myPositionStaffMenu->addAction(startPositionAct);
#endif
    myPositionStaffMenu->addAction(myNextPositionCommand);
#if 0
    myPositionStaffMenu->addAction(prevPositionAct);
    myPositionStaffMenu->addAction(nextStringAct);
    myPositionStaffMenu->addAction(prevStringAct);
    myPositionStaffMenu->addAction(lastPositionAct);
    myPositionStaffMenu->addAction(nextStaffAct);
    myPositionStaffMenu->addAction(prevStaffAct);
    myPositionStaffMenu->addAction(nextBarAct);
    myPositionStaffMenu->addAction(prevBarAct);

    positionMenu->addSeparator();
    positionMenu->addAction(shiftTabNumUp);
    positionMenu->addAction(shiftTabNumDown);
    positionMenu->addSeparator();
    positionMenu->addAction(shiftForwardAct);
    positionMenu->addAction(shiftBackwardAct);
    positionMenu->addSeparator();
    positionMenu->addAction(clearNoteAct);
    positionMenu->addAction(clearCurrentPositionAct);
    positionMenu->addSeparator();
    positionMenu->addAction(gotoBarlineAct);
    positionMenu->addAction(gotoRehearsalSignAct);

    // Text Menu
    textMenu = menuBar()->addMenu(tr("&Text"));
    textMenu->addAction(chordNameAct);

    // Section Menu
    sectionMenu = menuBar()->addMenu(tr("&Section"));
    sectionMenu->addAction(increasePositionSpacingAct);
    sectionMenu->addAction(decreasePositionSpacingAct);
    sectionMenu->addSeparator();
    sectionMenu->addAction(insertSystemAtEndAct);
    sectionMenu->addAction(insertSystemBeforeAct);
    sectionMenu->addAction(insertSystemAfterAct);
    sectionMenu->addSeparator();
    sectionMenu->addAction(removeCurrentSystemAct);

    // Note Menu
    notesMenu = menuBar()->addMenu(tr("&Notes"));
    notesMenu->addAction(wholeNoteAct);
    notesMenu->addAction(halfNoteAct);
    notesMenu->addAction(quarterNoteAct);
    notesMenu->addAction(eighthNoteAct);
    notesMenu->addAction(sixteenthNoteAct);
    notesMenu->addAction(thirtySecondNoteAct);
    notesMenu->addAction(sixtyFourthNoteAct);
    notesMenu->addSeparator();
    notesMenu->addAction(increaseDurationAct);
    notesMenu->addAction(decreaseDurationAct);
    notesMenu->addSeparator();
    notesMenu->addAction(dottedNoteAct);
    notesMenu->addAction(doubleDottedNoteAct);
    notesMenu->addSeparator();
    notesMenu->addAction(addDotAct);
    notesMenu->addAction(removeDotAct);
    notesMenu->addSeparator();
    notesMenu->addAction(tiedNoteAct);
    notesMenu->addSeparator();
    notesMenu->addAction(noteMutedAct);
    notesMenu->addSeparator();
    notesMenu->addAction(ghostNoteAct);
    notesMenu->addSeparator();
    notesMenu->addAction(letRingAct);
    notesMenu->addAction(fermataAct);
    notesMenu->addSeparator();
    notesMenu->addActions(QList<QAction*>() << graceNoteAct << staccatoNoteAct <<
                          marcatoAct << sforzandoAct);

    octaveMenu = notesMenu->addMenu(tr("Octave"));
    octaveMenu->addActions(QList<QAction*>() << octave8vaAct << octave15maAct
                           << octave8vbAct << octave15mbAct);

    notesMenu->addAction(tripletAct);
    notesMenu->addAction(irregularGroupingAct);

    // Rests Menu
    restsMenu = menuBar()->addMenu(tr("Rests"));
    restsMenu->addActions(QList<QAction*>() << wholeRestAct << halfRestAct <<
                          quarterRestAct << eighthRestAct << sixteenthRestAct <<
                          thirtySecondRestAct << sixtyFourthRestAct);
    restsMenu->addSeparator();
    restsMenu->addAction(addRestAct);

    // Music Symbols Menu
    musicSymbolsMenu = menuBar()->addMenu(tr("&Music Symbols"));
    musicSymbolsMenu->addAction(rehearsalSignAct);
    musicSymbolsMenu->addAction(tempoMarkerAct);
    musicSymbolsMenu->addAction(keySignatureAct);
    musicSymbolsMenu->addAction(timeSignatureAct);
    musicSymbolsMenu->addAction(standardBarlineAct);
    musicSymbolsMenu->addAction(barlineAct);
    musicSymbolsMenu->addAction(musicalDirectionAct);
    musicSymbolsMenu->addAction(repeatEndingAct);
    musicSymbolsMenu->addAction(dynamicAct);
    musicSymbolsMenu->addAction(volumeSwellAct);

    // Tab Symbols Menu
    tabSymbolsMenu = menuBar()->addMenu(tr("&Tab Symbols"));
    tabSymbolsMenu->addAction(hammerPullAct);
    tabSymbolsMenu->addAction(naturalHarmonicAct);
    tabSymbolsMenu->addAction(artificialHarmonicAct);
    tabSymbolsMenu->addAction(tappedHarmonicAct);
    tabSymbolsMenu->addSeparator();

    slideIntoMenu = tabSymbolsMenu->addMenu(tr("Slide Into"));
    slideIntoMenu->addAction(slideIntoFromBelowAct);
    slideIntoMenu->addAction(slideIntoFromAboveAct);

    tabSymbolsMenu->addAction(shiftSlideAct);
    tabSymbolsMenu->addAction(legatoSlideAct);

    slideOutOfMenu = tabSymbolsMenu->addMenu(tr("Slide Out Of"));
    slideOutOfMenu->addAction(slideOutOfDownwardsAct);
    slideOutOfMenu->addAction(slideOutOfUpwardsAct);

    guitarMenu = menuBar()->addMenu(tr("&Guitar"));
    guitarMenu->addAction(addGuitarAct);
    guitarMenu->addSeparator();
    guitarMenu->addAction(tuningDictionaryAct);

    tabSymbolsMenu->addSeparator();
    tabSymbolsMenu->addAction(vibratoAct);
    tabSymbolsMenu->addAction(wideVibratoAct);
    tabSymbolsMenu->addSeparator();
    tabSymbolsMenu->addAction(palmMuteAct);
    tabSymbolsMenu->addAction(tremoloPickingAct);
    tabSymbolsMenu->addAction(trillAction);
    tabSymbolsMenu->addAction(tapAct);
    tabSymbolsMenu->addSeparator();
    tabSymbolsMenu->addAction(arpeggioUpAct);
    tabSymbolsMenu->addAction(arpeggioDownAct);
    tabSymbolsMenu->addSeparator();
    tabSymbolsMenu->addAction(pickStrokeUpAct);
    tabSymbolsMenu->addAction(pickStrokeDownAct);

#endif
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
#if 0
    updateScoreAreaActions(false);
#endif
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

    ScoreArea *scorearea = new ScoreArea(this);
    scorearea->renderDocument(doc, Staff::GuitarView);
#if 0
    score->installEventFilter(this);
    score->renderDocument(doc);

    score->timeSignaturePubSub()->subscribe(
                boost::bind(&PowerTabEditor::editTimeSignature, this, _1));
    score->keySignaturePubSub()->subscribe(
                boost::bind(&PowerTabEditor::editKeySignature, this, _1));
    score->barlinePubSub()->subscribe(
                boost::bind(&PowerTabEditor::editBarline, this, _1));

    undoManager->addNewUndoStack();
#endif

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

#if 0
    // if this is the only open document, enable score area actions
    if (documentManager->getCurrentDocumentIndex() == 0)
    {
        updateScoreAreaActions(true);
    }

    updateActions(); // update available actions for the current position

    getCurrentScoreArea()->setFocus();
#endif
    qDebug() << "Tab opened in" << timer.elapsed() << "seconds";
}

ScoreArea *PowerTabEditor::getScoreArea()
{
    return dynamic_cast<ScoreArea *>(myTabWidget->currentWidget());
}

Caret &PowerTabEditor::getCaret()
{
    return myDocumentManager->getCurrentDocument().getCaret();
}

#if 0
/// Redraws the specified system.
void PowerTabEditor::redrawSystem(int index)
{
    Caret* caret = getCurrentScoreArea()->getCaret();
    caret->adjustToValidLocation();
    Score* score = caret->getCurrentScore();

    // Update the data model.
    caret->getCurrentScore()->UpdateSystemHeight(score->GetSystem(index));

    // Update the score area.
    getCurrentScoreArea()->updateSystem(index);

    // Update the status of all actions
    updateActions();
}

/// Redraw the entire score.
void PowerTabEditor::performFullRedraw()
{
    getCurrentScoreArea()->requestFullRedraw();
    redrawSystem(0); // Trigger a redraw - the system index doesn't matter.
}

// this is a reimplementation of QObject's eventFilter function
// it returns true to tell Qt to stop propagating this event
// this is necessary to intercept tab key presses before they are
// used to cycle focus to different widgets
bool PowerTabEditor::eventFilter(QObject *object, QEvent *event)
{
    ScoreArea* currentDoc = getCurrentScoreArea();

    if (currentDoc && event->type() == QEvent::KeyPress)
    {
        QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);
        if ((keyEvent->key() >= Qt::Key_0) &&
                 (keyEvent->key() <= Qt::Key_9))
        {
            // this arithmetic and this condition assume that Key_0 ... Key_9
            // are assigned ascending continuous numbers as they are in Qt 4.7
            int typedNumber = keyEvent->key() - Qt::Key_0;
            Caret *caret = getCurrentScoreArea()->getCaret();
            Note *currentNote = caret->getCurrentNote();
            Position *currentPosition = caret->getCurrentPosition();
            shared_ptr<Staff> currentStaff = caret->getCurrentStaff();
            shared_ptr<const Barline> currentBarline = caret->getCurrentBarline();
            const int system = caret->getCurrentSystemIndex();
            const uint32_t voice = caret->getCurrentVoice();

            // Don't allow inserting notes on top of bars (except for the first
            // bar in the system, which has position 0).
            if (!currentBarline || caret->getCurrentPositionIndex() == 0)
            {
                if (currentNote != NULL)
                {
                    // if there is already a number here update it
                    undoManager->push(new UpdateTabNumber(typedNumber, currentNote,
                                                          currentPosition, voice, currentStaff),
                                      system);
                }
                else
                {
                    undoManager->push(new AddNote(caret->getCurrentStringIndex(), typedNumber,
                                                  caret->getCurrentPositionIndex(), caret->getCurrentVoice(),
                                                  currentStaff, activeDuration),
                                      system);
                }

                return true;
            }
        }
    }

    return QMainWindow::eventFilter(object, event);
}

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

bool PowerTabEditor::moveCaretLeft()
{
    return getCurrentScoreArea()->getCaret()->moveCaretHorizontal(-1);
}

void PowerTabEditor::moveCaretDown()
{
    getCurrentScoreArea()->getCaret()->moveCaretVertical(1);
}

void PowerTabEditor::moveCaretUp()
{
    getCurrentScoreArea()->getCaret()->moveCaretVertical(-1);
}

void PowerTabEditor::moveCaretToStart()
{
    getCurrentScoreArea()->getCaret()->moveCaretToStart();
}

void PowerTabEditor::moveCaretToEnd()
{
    getCurrentScoreArea()->getCaret()->moveCaretToEnd();
}

void PowerTabEditor::moveCaretToFirstSection()
{
    getCurrentScoreArea()->getCaret()->moveCaretToFirstSection();
}

bool PowerTabEditor::moveCaretToNextSection()
{
    return getCurrentScoreArea()->getCaret()->moveCaretSection(1);;
}

bool PowerTabEditor::moveCaretToPrevSection()
{
    return getCurrentScoreArea()->getCaret()->moveCaretSection(-1);
}

void PowerTabEditor::moveCaretToLastSection()
{
    getCurrentScoreArea()->getCaret()->moveCaretToLastSection();
}

bool PowerTabEditor::moveCaretToNextStaff()
{
    return getCurrentScoreArea()->getCaret()->moveCaretStaff(1);;
}

bool PowerTabEditor::moveCaretToPrevStaff()
{
    return getCurrentScoreArea()->getCaret()->moveCaretStaff(-1);
}

void PowerTabEditor::moveCaretToNextBar()
{
    getCurrentScoreArea()->getCaret()->moveCaretToNextBar();
}

void PowerTabEditor::moveCaretToPrevBar()
{
    getCurrentScoreArea()->getCaret()->moveCaretToPrevBar();
}

/// Allows the user to jump to a specific bar in the score.
void PowerTabEditor::gotoBarline()
{
    Caret* caret = getCurrentScoreArea()->getCaret();
    GoToBarlineDialog dialog(this, caret->getCurrentScore());

    if (dialog.exec() == QDialog::Accepted)
    {
        SystemLocation location = dialog.getLocation();
        caret->setCurrentSystemIndex(location.getSystemIndex());
        caret->setCurrentPositionIndex(location.getPositionIndex());
    }
}

/// Allows the user to jump to a specific rehearsal sign.
void PowerTabEditor::gotoRehearsalSign()
{
    Caret* caret = getCurrentScoreArea()->getCaret();
    GoToRehearsalSignDialog dialog(this, caret->getCurrentScore());

    if (dialog.exec() == QDialog::Accepted)
    {
        SystemLocation location = dialog.getLocation();
        caret->setCurrentSystemIndex(location.getSystemIndex());
        caret->setCurrentPositionIndex(location.getPositionIndex());
    }
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

void PowerTabEditor::removeCurrentSystem()
{
    Caret* caret = getCurrentScoreArea()->getCaret();

    RemoveSystem* removeSystemAct = new RemoveSystem(caret->getCurrentScore(), caret->getCurrentSystemIndex());
    undoManager->push(removeSystemAct, UndoManager::AFFECTS_ALL_SYSTEMS);
}

void PowerTabEditor::insertSystemAfter()
{
    const size_t index = getCurrentScoreArea()->getCaret()->getCurrentSystemIndex() + 1;
    performSystemInsert(index);
}

void PowerTabEditor::insertSystemBefore()
{
    const size_t currentIndex = getCurrentScoreArea()->getCaret()->getCurrentSystemIndex();
    performSystemInsert(currentIndex);
}

void PowerTabEditor::insertSystemAtEnd()
{
    const size_t index = getCurrentScoreArea()->getCaret()->getCurrentScore()->GetSystemCount();
    performSystemInsert(index);
}

/// Inserts a system at the specified index.
void PowerTabEditor::performSystemInsert(size_t index)
{
    Caret* caret = getCurrentScoreArea()->getCaret();
    Score* score = caret->getCurrentScore();

    AddSystem* addSystemAct = new AddSystem(score, index);
    undoManager->push(addSystemAct, UndoManager::AFFECTS_ALL_SYSTEMS);
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

/// Edit the key signature at the caret's current location.
void PowerTabEditor::editKeySignatureFromCaret()
{
    const Caret* caret = getCurrentScoreArea()->getCaret();
    const SystemLocation caretLocation(caret->getCurrentSystemIndex(),
                                       caret->getCurrentPositionIndex());
    editKeySignature(caretLocation);
}

/// Edit the key signature at the specified location.
void PowerTabEditor::editKeySignature(const SystemLocation& location)
{
    const Caret* caret = getCurrentScoreArea()->getCaret();
    Score* score = caret->getCurrentScore();
    shared_ptr<Barline> barline = score->GetSystem(location.getSystemIndex())->
            GetBarlineAtPosition(location.getPositionIndex());
    Q_ASSERT(barline);

    const KeySignature& keySignature = barline->GetKeySignature();

    KeySignatureDialog dialog(this, keySignature);
    if (dialog.exec() == QDialog::Accepted)
    {
        const KeySignature newKey = dialog.getNewKey();

        EditKeySignature* action = new EditKeySignature(score, location,
                                                        newKey.GetKeyType(),
                                                        newKey.GetKeyAccidentals(),
                                                        newKey.IsShown());

        undoManager->push(action, UndoManager::AFFECTS_ALL_SYSTEMS);
    }
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

/// Edit the barline at the caret's current location.
void PowerTabEditor::editBarlineFromCaret()
{
    const Caret* caret = getCurrentScoreArea()->getCaret();
    const SystemLocation caretLocation(caret->getCurrentSystemIndex(),
                                       caret->getCurrentPositionIndex());
    editBarline(caretLocation);
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

/// Edits or creates a barline at the specified position.
void PowerTabEditor::editBarline(const SystemLocation& location)
{
    Caret* caret = getCurrentScoreArea()->getCaret();
    Score* score = caret->getCurrentScore();
    shared_ptr<System> system = score->GetSystem(location.getSystemIndex());
    shared_ptr<Barline> barline = system->GetBarlineAtPosition(
                location.getPositionIndex());

    const bool startBar = barline == system->GetStartBar();
    const bool endBar = barline == system->GetEndBar();

    // Move the cursor to the barline. This is important if a barline in another
    // system was clicked, since otherwise it might not be redrawn properly.
    caret->setCurrentSystemIndex(location.getSystemIndex());
    caret->setCurrentPositionIndex(location.getPositionIndex());

    if (barline) // edit existing barline
    {
        quint8 type = barline->GetType(), repeats = barline->GetRepeatCount();

        BarlineDialog dialog(this, type, repeats, startBar, endBar);
        if (dialog.exec() == QDialog::Accepted)
        {
            undoManager->push(new ChangeBarLineType(system, barline,
                                                    dialog.barlineType(),
                                                    dialog.repeatCount()),
                              caret->getCurrentSystemIndex());
        }
    }
    else // create new barline
    {
        quint8 type = Barline::bar, repeats = Barline::MIN_REPEAT_COUNT;

        BarlineDialog dialog(this, type, repeats, startBar, endBar);
        if (dialog.exec() == QDialog::Accepted)
        {
            undoManager->push(new AddBarline(caret->getCurrentSystem(),
                                             caret->getCurrentPositionIndex(),
                                             dialog.barlineType(),
                                             dialog.repeatCount()),
                              caret->getCurrentSystemIndex());
        }
    }
}

/// Inserts a standard barline at the current position.
void PowerTabEditor::insertStandardBarline()
{
    const Caret* caret = getCurrentScoreArea()->getCaret();
    undoManager->push(new AddBarline(caret->getCurrentSystem(),
                                     caret->getCurrentPositionIndex(),
                                     Barline::bar, 0),
                      caret->getCurrentSystemIndex());
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

// Add/Remove the trill at the current position
void PowerTabEditor::editTrill()
{
    Caret* caret = getCurrentScoreArea()->getCaret();
    Note* currentNote = caret->getCurrentNote();

    if (currentNote->HasTrill())
    {
        undoManager->push(new RemoveTrill(currentNote), caret->getCurrentSystemIndex());
    }
    else // add a new trill
    {
        TrillDialog trillDialog(this, currentNote);
        if (trillDialog.exec() == QDialog::Accepted)
        {
            undoManager->push(new AddTrill(currentNote, trillDialog.getTrill()),
                              caret->getCurrentSystemIndex());
        }
        else
        {
            trillAction->setChecked(false);
        }
    }
}

/// Add/Remove a tapped harmonic at the current note
void PowerTabEditor::editTappedHarmonic()
{
    Caret* caret = getCurrentScoreArea()->getCaret();
    Note* currentNote = caret->getCurrentNote();

    if (currentNote->HasTappedHarmonic())
    {
        undoManager->push(new RemoveTappedHarmonic(currentNote), caret->getCurrentSystemIndex());
    }
    else // add a tapped harmonic
    {
        uint8_t tappedFret = 0;
        TappedHarmonicDialog dialog(this, currentNote, tappedFret);

        if (dialog.exec() == QDialog::Accepted)
        {
            undoManager->push(new AddTappedHarmonic(currentNote, tappedFret),
                                                caret->getCurrentSystemIndex());
        }
        else
        {
            tappedHarmonicAct->setChecked(false);
        }
    }
}

// If there is a rehearsal sign at the barline, remove it
// If there is no rehearsal sign, show the dialog to add one
void PowerTabEditor::editRehearsalSign()
{
    // Find if there is a rehearsal sign at the current position
    const Caret* caret = getCurrentScoreArea()->getCaret();
    Score* score = caret->getCurrentScore();
    System::BarlinePtr currentBarline = caret->getCurrentBarline();

    // the rehearsal sign action should not be available unless there is a barline at the current position
    Q_ASSERT(currentBarline);

    RehearsalSign& rehearsalSign = currentBarline->GetRehearsalSign();

    if (rehearsalSign.IsSet())
    {
        EditRehearsalSign* action = new EditRehearsalSign(score, rehearsalSign, false);
        undoManager->push(action, UndoManager::AFFECTS_ALL_SYSTEMS);
    }
    else
    {
        RehearsalSignDialog dialog(this);

        if (dialog.exec() == QDialog::Accepted)
        {
            EditRehearsalSign* action = new EditRehearsalSign(score, rehearsalSign, true,
                                                              dialog.description());
            undoManager->push(action, UndoManager::AFFECTS_ALL_SYSTEMS);
        }
        else
        {
            rehearsalSignAct->setChecked(false);
        }
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

// Enables/disables actions that should only be available when a score is opened
void PowerTabEditor::updateScoreAreaActions(bool enable)
{
    QList<QMenu*> menuList;
    menuList << playbackMenu << positionMenu << textMenu << notesMenu << musicSymbolsMenu << tabSymbolsMenu << windowMenu;
    menuList << positionSectionMenu << myPositionStaffMenu << sectionMenu << octaveMenu << slideIntoMenu;
    menuList << slideOutOfMenu << restsMenu << editMenu;

    foreach(QMenu* menu, menuList)
    {
        foreach(QAction* action, menu->actions())
        {
            action->setEnabled(enable);
        }
    }

    myCloseTabCommand->setEnabled(enable);
    mySaveAsCommand->setEnabled(enable);
    addGuitarAct->setEnabled(enable);
}

void PowerTabEditor::changeNoteDuration(bool increase)
{
// TODO shift+up on a selection should increase the duration of each note
//    const std::vector<Position*> selectedPositions = getSelectedPositions();
//    if (!selectedPositions.empty())
//    {
//        uint duration = selectedPositions.at(0)->GetDuration();
//        updateNoteDuration(increase ? duration >> 1 : duration << 1);
//    }

    Caret* caret = getCurrentScoreArea()->getCaret();
    const Position* currentPosition = caret->getCurrentPosition();

    if (currentPosition != NULL)
    {
        uint8_t duration = currentPosition->GetDurationType();
        updateNoteDuration(increase ? duration >> 1 : duration << 1);
    }
    else
    {
        updateNoteDuration(increase ? activeDuration >> 1 : activeDuration << 1);
    }

    updateActions();
}
/// Updates the active note duration (for inserting future notes), and
/// updates the duration of the selected note(s) if possible
void PowerTabEditor::updateNoteDuration(uint8_t duration)
{
    // first and foremost set activeDuration for future notes
    activeDuration = duration;

    const std::vector<Position*> selectedPositions = getSelectedPositions();

    if (!selectedPositions.empty())
    {
        undoManager->push(new UpdateNoteDuration(selectedPositions, duration),
                          getCurrentScoreArea()->getCaret()->getCurrentSystemIndex());
    }
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

void PowerTabEditor::editDynamic()
{
    const Caret* caret = getCurrentScoreArea()->getCaret();
    Score* currentScore = caret->getCurrentScore();
    Score::DynamicPtr dynamic = currentScore->FindDynamic(caret->getCurrentSystemIndex(), caret->getCurrentStaffIndex(),
                                                          caret->getCurrentPositionIndex());

    if (!dynamic) // add a dynamic
    {
        DynamicDialog dialog(this);
        if (dialog.exec() == QDialog::Accepted)
        {
            dynamic = boost::make_shared<Dynamic>(caret->getCurrentSystemIndex(), caret->getCurrentStaffIndex(),
                                                caret->getCurrentPositionIndex(), dialog.selectedVolumeLevel(),
                                                Dynamic::notSet);

            undoManager->push(new AddDynamic(currentScore, dynamic),
                              caret->getCurrentSystemIndex());
        }
        else
        {
            dynamicAct->setChecked(false);
        }
    }
    else
    {
        undoManager->push(new RemoveDynamic(currentScore, dynamic),
                          caret->getCurrentSystemIndex());
    }
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
