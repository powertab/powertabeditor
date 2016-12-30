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

#include <actions/addalternateending.h>
#include <actions/addbarline.h>
#include <actions/addchordtext.h>
#include <actions/adddirection.h>
#include <actions/adddynamic.h>
#include <actions/addinstrument.h>
#include <actions/addirregulargrouping.h>
#include <actions/addmultibarrest.h>
#include <actions/addnote.h>
#include <actions/addnoteproperty.h>
#include <actions/addplayer.h>
#include <actions/addplayerchange.h>
#include <actions/addpositionproperty.h>
#include <actions/addrehearsalsign.h>
#include <actions/addrest.h>
#include <actions/addspecialnoteproperty.h>
#include <actions/addstaff.h>
#include <actions/addsystem.h>
#include <actions/addtempomarker.h>
#include <actions/addtextitem.h>
#include <actions/adjustlinespacing.h>
#include <actions/editbarline.h>
#include <actions/editfileinformation.h>
#include <actions/editinstrument.h>
#include <actions/editkeysignature.h>
#include <actions/editnoteduration.h>
#include <actions/editplayer.h>
#include <actions/editstaff.h>
#include <actions/edittabnumber.h>
#include <actions/edittimesignature.h>
#include <actions/editviewfilters.h>
#include <actions/polishscore.h>
#include <actions/polishsystem.h>
#include <actions/removealternateending.h>
#include <actions/removebarline.h>
#include <actions/removechordtext.h>
#include <actions/removedirection.h>
#include <actions/removedynamic.h>
#include <actions/removeinstrument.h>
#include <actions/removeirregulargrouping.h>
#include <actions/removenote.h>
#include <actions/removenoteproperty.h>
#include <actions/removeplayer.h>
#include <actions/removeplayerchange.h>
#include <actions/removeposition.h>
#include <actions/removepositionproperty.h>
#include <actions/removerehearsalsign.h>
#include <actions/removespecialnoteproperty.h>
#include <actions/removestaff.h>
#include <actions/removesystem.h>
#include <actions/removetempomarker.h>
#include <actions/removetextitem.h>
#include <actions/shiftpositions.h>
#include <actions/undomanager.h>

#include <app/appinfo.h>
#include <app/caret.h>
#include <app/clipboard.h>
#include <app/command.h>
#include <app/documentmanager.h>
#include <app/paths.h>
#include <app/pubsub/clickpubsub.h>
#include <app/recentfiles.h>
#include <app/scorearea.h>
#include <app/settings.h>
#include <app/settingsmanager.h>
#include <app/tuningdictionary.h>

#include <audio/midiplayer.h>
#include <audio/settings.h>

#include <boost/lexical_cast.hpp>
#include <boost/range/algorithm/transform.hpp>
#include <chrono>

#include <dialogs/alterationofpacedialog.h>
#include <dialogs/alternateendingdialog.h>
#include <dialogs/artificialharmonicdialog.h>
#include <dialogs/barlinedialog.h>
#include <dialogs/benddialog.h>
#include <dialogs/chordnamedialog.h>
#include <dialogs/directiondialog.h>
#include <dialogs/dynamicdialog.h>
#include <dialogs/fileinformationdialog.h>
#include <dialogs/gotobarlinedialog.h>
#include <dialogs/gotorehearsalsigndialog.h>
#include <dialogs/irregulargroupingdialog.h>
#include <dialogs/keyboardsettingsdialog.h>
#include <dialogs/keysignaturedialog.h>
#include <dialogs/multibarrestdialog.h>
#include <dialogs/playerchangedialog.h>
#include <dialogs/preferencesdialog.h>
#include <dialogs/rehearsalsigndialog.h>
#include <dialogs/staffdialog.h>
#include <dialogs/tappedharmonicdialog.h>
#include <dialogs/tempomarkerdialog.h>
#include <dialogs/textitemdialog.h>
#include <dialogs/timesignaturedialog.h>
#include <dialogs/trilldialog.h>
#include <dialogs/tuningdictionarydialog.h>
#include <dialogs/viewfilterdialog.h>

#include <formats/fileformatmanager.h>

#include <QCoreApplication>
#include <QDebug>
#include <QDesktopServices>
#include <QDockWidget>
#include <QFileDialog>
#include <QFontDatabase>
#include <QKeyEvent>
#include <QMenuBar>
#include <QMessageBox>
#include <QMimeData>
#include <QPrinter>
#include <QPrintDialog>
#include <QPrintPreviewDialog>
#include <QScrollArea>
#include <QTabBar>
#include <QUrl>
#include <QVBoxLayout>

#include <score/utils.h>
#include <score/voiceutils.h>

#include <widgets/instruments/instrumentpanel.h>
#include <widgets/mixer/mixer.h>
#include <widgets/playback/playbackwidget.h>

PowerTabEditor::PowerTabEditor()
    : QMainWindow(nullptr),
      mySettingsManager(new SettingsManager()),
      myDocumentManager(new DocumentManager()),
      myFileFormatManager(new FileFormatManager(*mySettingsManager)),
      myUndoManager(new UndoManager()),
      myTuningDictionary(new TuningDictionary()),
      myIsPlaying(false),
      myRecentFiles(nullptr),
      myActiveDurationType(Position::EighthNote),
      myTabWidget(nullptr),
      myMixer(nullptr),
      myMixerDockWidget(nullptr),
      myInstrumentPanel(nullptr),
      myInstrumentDockWidget(nullptr),
      myPlaybackWidget(nullptr),
      myPlaybackArea(nullptr)
{
    this->setWindowIcon(QIcon(":icons/app_icon.png"));

    setAcceptDrops(true);

    // Load the music notation font.
    QFontDatabase::addApplicationFont(":fonts/emmentaler-13.otf");
    // Load the tab note font.
    QFontDatabase::addApplicationFont(":fonts/LiberationSans-Regular.ttf");
    QFontDatabase::addApplicationFont(":fonts/LiberationSerif-Regular.ttf");

    connect(myUndoManager.get(), SIGNAL(redrawNeeded(int)), this,
            SLOT(redrawSystem(int)));
    connect(myUndoManager.get(), SIGNAL(fullRedrawNeeded()), this,
            SLOT(redrawScore()));
    connect(myUndoManager.get(), SIGNAL(cleanChanged(bool)), this,
            SLOT(updateModified(bool)));

    myTuningDictionary->loadInBackground();
    mySettingsManager->load(Paths::getConfigDir());

    createMixer();
    createInstrumentPanel();
    createCommands();
    loadKeyboardShortcuts();
    createMenus();

    // Set up the recent files menu.
    myRecentFiles =
        new RecentFiles(*mySettingsManager, myRecentFilesMenu, this);
    connect(myRecentFiles, SIGNAL(fileSelected(QString)), this,
            SLOT(openFile(QString)));

    createTabArea();

    auto settings = mySettingsManager->getReadHandle();
    myPreviousDirectory =
        QString::fromStdString(settings->get(Settings::PreviousDirectory));

    // Restore the state of any dock widgets.
    restoreState(settings->get(Settings::WindowState));

    setCentralWidget(myPlaybackArea);
    setMinimumSize(800, 600);
    setWindowState(Qt::WindowMaximized);
    setWindowTitle(getApplicationName());
}

PowerTabEditor::~PowerTabEditor()
{
}

void PowerTabEditor::openFiles(const QStringList &files)
{
    for (auto &filename : files)
        openFile(filename);
}

void PowerTabEditor::createNewDocument()
{
    myDocumentManager->addDefaultDocument(*mySettingsManager);
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

    int validationResult = myDocumentManager->findDocument(filename.toStdString());
    if (validationResult > -1)
    {
        qDebug() << "File: " << filename << " is already open";
        myTabWidget->setCurrentIndex(validationResult);
        return;
    }

    auto start = std::chrono::high_resolution_clock::now();

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

    try
    {
        Document &doc = myDocumentManager->addDocument();
        myFileFormatManager->importFile(doc.getScore(), filename.toStdString(),
                                        *format);
        auto end = std::chrono::high_resolution_clock::now();
        qDebug() << "File loaded in"
                 << std::chrono::duration_cast<std::chrono::milliseconds>(end - start) .count()
                 << "ms";

        doc.setFilename(filename.toStdString());
        setPreviousDirectory(filename);
        myRecentFiles->add(filename);
        setupNewTab();
    }
    catch (const std::exception &e)
    {
        myDocumentManager->removeDocument(
            myDocumentManager->getCurrentDocumentIndex());

        QMessageBox::warning(
            this, tr("Error Opening File"),
            tr("Error opening file: %1").arg(QString(e.what())));
    }
}

void PowerTabEditor::switchTab(int index)
{
    myDocumentManager->setCurrentDocumentIndex(index);

    if (index != -1)
    {
        const Document &doc = myDocumentManager->getCurrentDocument();
        myMixer->reset(doc.getScore());
        myInstrumentPanel->reset(doc.getScore());
        myPlaybackWidget->reset(doc);
        updateLocationLabel();
    }
    else
    {
        myMixer->clear();
        myInstrumentPanel->clear();
    }

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
            if (!saveFile())
                return false;
        }
        else if (ret == QMessageBox::Cancel)
            return false;
    }

    if (myDocumentManager->getDocument(index).getCaret().isInPlaybackMode())
        startStopPlayback();

    myUndoManager->removeStack(index);
    myDocumentManager->removeDocument(index);
    delete myTabWidget->widget(index);

    // Get the index of the tab that we will now switch to.
    const int currentIndex = myTabWidget->currentIndex();

    myUndoManager->setActiveStackIndex(currentIndex);
    myDocumentManager->setCurrentDocumentIndex(currentIndex);

    enableEditing(currentIndex != -1);
    myPlaybackWidget->setEnabled(currentIndex != -1);

    return true;
}

bool PowerTabEditor::closeCurrentTab()
{
    return closeTab(myDocumentManager->getCurrentDocumentIndex());
}

bool PowerTabEditor::saveFile()
{
    const Document &doc = myDocumentManager->getCurrentDocument();
    if (!doc.hasFilename())
        return saveFileAs();

    const QString filename = QString::fromStdString(doc.getFilename());
    return QFileInfo(filename).suffix() == "pt2" ? saveFile(filename)
                                                 : saveFileAs();
}

bool PowerTabEditor::saveFile(QString path)
{
    QFileInfo info(path);
    QString extension = info.suffix();
    Q_ASSERT(!extension.isEmpty());

    boost::optional<FileFormat> format =
        myFileFormatManager->findFormat(extension.toStdString());
    if (!format)
    {
        QMessageBox::warning(this, tr("Error Saving File"),
                             tr("Unsupported file type."));
        return false;
    }

    const std::string path_str = path.toStdString();
    Document &doc = myDocumentManager->getCurrentDocument();

    try
    {
        myFileFormatManager->exportFile(doc.getScore(), path_str, *format);
    }
    catch (const std::exception &e)
    {
        QMessageBox::warning(
            this, tr("Error Saving File"),
            tr("Error saving file: %1").arg(QString(e.what())));

        return false;
    }

    if (extension == "pt2")
    {
        doc.setFilename(path_str);

        // Update window title and tab bar.
        updateWindowTitle();
        const QString filename = info.fileName();
        myTabWidget->setTabText(myTabWidget->currentIndex(), filename);
        myTabWidget->setTabToolTip(myTabWidget->currentIndex(), filename);

        // Add to the recent files list and update the last used directory.
        myRecentFiles->add(path);
        setPreviousDirectory(path);

        // Mark the file as being in an unmodified state.
        myUndoManager->setClean();
    }

    return true;
}

bool PowerTabEditor::saveFileAs()
{
    const QString filter =
        QString::fromStdString(myFileFormatManager->exportFileFilter());

    QFileDialog dialog(this, tr("Save As"), myPreviousDirectory, filter);
    dialog.setAcceptMode(QFileDialog::AcceptSave);

    if (dialog.exec() == QDialog::Accepted)
    {
        QString path = dialog.selectedFiles().first();
        if (path.isEmpty())
            return false;

        // Add a suitable file extension if necessary.
        QFileInfo info(path);
        if (info.suffix().isEmpty())
        {
            const QString file_filter = dialog.selectedNameFilter();
            QRegExp regex("\\*.(\\w+)");
            if (regex.indexIn(file_filter, 0) < 0)
                return false;

            const QString extension = regex.cap(1);
            path += ".";
            path += extension;
        }

        return saveFile(path);
    }
    else
        return false;
}

void PowerTabEditor::updateModified(bool clean)
{
    setWindowModified(!clean);
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
    KeyboardSettingsDialog dialog(this, getCommands());
    dialog.exec();

    saveKeyboardShortcuts();
}

void PowerTabEditor::editPreferences()
{
    PreferencesDialog dialog(this, *mySettingsManager, *myTuningDictionary);
    dialog.exec();
}

void PowerTabEditor::printDocument()
{
    QPrinter printer(QPrinter::HighResolution);

    QPrintDialog dialog(&printer, this);
    if (dialog.exec() != QDialog::Accepted)
        return;

    getScoreArea()->print(printer);
}

void PowerTabEditor::printPreview()
{
    // Set the window flags to Qt::Window so that the dialog can be maximized.
    QPrintPreviewDialog dialog(this, Qt::Window);

    connect(&dialog, &QPrintPreviewDialog::paintRequested, this,
            [=](QPrinter *printer) {
        getScoreArea()->print(*printer);
    });

    dialog.exec();
}

void PowerTabEditor::cutSelectedNotes()
{
    myUndoManager->beginMacro(tr("Cut Notes"));
    copySelectedNotes();
    removeSelectedPositions();
    myUndoManager->endMacro();
}

void PowerTabEditor::copySelectedNotes()
{
    Clipboard::copySelection(getLocation());
}

void PowerTabEditor::pasteNotes()
{
    if (!Clipboard::hasData())
    {
        QMessageBox msg(this);
        msg.setText(QObject::tr("The clipboard does not contain any notes."));
        msg.exec();
        return;
    }

    myUndoManager->beginMacro(tr("Paste Notes"));

    // If there are any selected notes, delete them before pasting.
    if (getLocation().hasSelection())
        removeSelectedPositions();

    Clipboard::paste(this, *myUndoManager, getLocation());
    myUndoManager->endMacro();
}

void PowerTabEditor::polishScore()
{
    myUndoManager->push(new PolishScore(getLocation().getScore()),
                        UndoManager::AFFECTS_ALL_SYSTEMS);
}

void PowerTabEditor::polishSystem()
{
    myUndoManager->push(new PolishSystem(getLocation()),
                        getLocation().getSystemIndex());
}

void PowerTabEditor::editFileInformation()
{
    FileInformationDialog dialog(this, myDocumentManager->getCurrentDocument());
    if (dialog.exec() == QDialog::Accepted)
    {
        myUndoManager->push(
            new EditFileInformation(getLocation(), dialog.getScoreInfo()),
            UndoManager::AFFECTS_ALL_SYSTEMS);
    }
}

void PowerTabEditor::startStopPlayback(bool from_measure_start)
{
    myIsPlaying = !myIsPlaying;

    if (myIsPlaying)
    {
        // Start up the midi player.
        myPlayPauseCommand->setText(tr("Pause"));

        // Move the caret to the start of the current bar if necessary.
        if (from_measure_start)
        {
            moveCaretToPrevBar();
            moveCaretToNextBar();
        }

        getCaret().setIsInPlaybackMode(true);
        myPlaybackWidget->setPlaybackMode(true);
        enableEditing(false);

        const ScoreLocation &location = getLocation();
        myMidiPlayer.reset(
            new MidiPlayer(*mySettingsManager, location,
                           myPlaybackWidget->getPlaybackSpeed()));

        connect(myMidiPlayer.get(), SIGNAL(playbackSystemChanged(int)), this,
                SLOT(moveCaretToSystem(int)));
        connect(myMidiPlayer.get(), SIGNAL(playbackPositionChanged(int)), this,
                SLOT(moveCaretToPosition(int)));
        connect(myMidiPlayer.get(), SIGNAL(finished()), this,
                SLOT(startStopPlayback()));
        connect(myPlaybackWidget, &PlaybackWidget::playbackSpeedChanged,
                myMidiPlayer.get(), &MidiPlayer::changePlaybackSpeed);

        connect(myMidiPlayer.get(), &MidiPlayer::error, this, [=](const QString &msg) {
            QMessageBox::critical(this, tr("Midi Error"), msg);
        });

        myMidiPlayer->start();
    }
    else
    {
        // If we manually stop playback, tell the midi thread to finish.
        if (myMidiPlayer && myMidiPlayer->isRunning())
        {
            // Avoid recursion from the finished() signal being called.
            myMidiPlayer->disconnect(this);
            myMidiPlayer.reset();
        }

        myPlayPauseCommand->setText(tr("Play"));
        getCaret().setIsInPlaybackMode(false);
        myPlaybackWidget->setPlaybackMode(false);

        enableEditing(true);
        updateCommands();
    }
}

void PowerTabEditor::redrawSystem(int index)
{
    getCaret().moveToValidPosition();
    getScoreArea()->redrawSystem(index);
    updateCommands();
}

void PowerTabEditor::redrawScore()
{
    Document &doc = myDocumentManager->getCurrentDocument();
    doc.validateViewOptions();
    getCaret().moveToValidPosition();
    getScoreArea()->renderDocument(doc);
    updateCommands();

    myMixer->reset(doc.getScore());
    myInstrumentPanel->reset(doc.getScore());
    myPlaybackWidget->reset(doc);
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

void PowerTabEditor::moveCaretToPosition(int position)
{
    getCaret().moveToPosition(position);
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

void PowerTabEditor::moveCaretToSystem(int system)
{
    getCaret().moveToSystem(system, true);
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
    // Move to the first position after the barline.
    if (getCaret().getLocation().getPositionIndex() != 0)
        getCaret().moveHorizontal(1);
}

void PowerTabEditor::moveCaretToPrevBar()
{
    getCaret().moveToPrevBar();
    // Move to the first position after the barline.
    if (getCaret().getLocation().getPositionIndex() != 0)
        getCaret().moveHorizontal(1);
}

void PowerTabEditor::shiftForward()
{
    ScoreLocation &location = getLocation();
    myUndoManager->push(new ShiftPositions(location, ShiftPositions::Forward),
                        location.getSystemIndex());
}

void PowerTabEditor::shiftBackward()
{
    ScoreLocation &location = getLocation();
    myUndoManager->push(new ShiftPositions(location, ShiftPositions::Backward),
                        location.getSystemIndex());
}

void PowerTabEditor::removeNote()
{
    myUndoManager->push(new RemoveNote(getLocation()),
                        getLocation().getSystemIndex());
}

void PowerTabEditor::removeSelectedPositions()
{
    ScoreLocation location(getLocation());

    const std::vector<Position *> selectedPositions(
        location.getSelectedPositions());
    const std::vector<Barline *> bars = location.getSelectedBarlines();

    myUndoManager->beginMacro(tr("Remove Position"));

    // The Position pointers will become invalid once we start creating
    // RemovePosition actions. So, we build a list of their position indices
    // beforehand and use that instead.
    std::vector<int> positions;
    std::transform(selectedPositions.begin(), selectedPositions.end(),
                   std::back_inserter(positions),
                   std::mem_fun(&Position::getPosition));

    // Remove each of the selected positions.
    for (int position : positions)
    {
        location.setPositionIndex(position);
        myUndoManager->push(new RemovePosition(location),
                            location.getSystemIndex());
    }

    std::vector<int> barPositions;
    std::transform(bars.begin(), bars.end(), std::back_inserter(barPositions),
                   std::mem_fun(&Barline::getPosition));

    // Remove each of the selected barlines.
    for (int position : barPositions)
    {
        location.setPositionIndex(position);
        myUndoManager->push(new RemoveBarline(location),
                            location.getSystemIndex());
    }

    myUndoManager->endMacro();
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

void PowerTabEditor::editChordName()
{
    ScoreLocation &location(getLocation());

    if (!ScoreUtils::findByPosition(location.getSystem().getChords(),
                                    location.getPositionIndex()))
    {
        ChordNameDialog dialog(this);
        if (dialog.exec() == QDialog::Accepted)
        {
            ChordText text(location.getPositionIndex(),
                           dialog.getChordName());

            myUndoManager->push(new AddChordText(location, text),
                                location.getSystemIndex());
        }
        else
            myChordNameCommand->setChecked(false);
    }
    else
    {
        myUndoManager->push(new RemoveChordText(location),
                            location.getSystemIndex());
    }
}

void PowerTabEditor::editTextItem()
{
    ScoreLocation &location(getLocation());

    if (!ScoreUtils::findByPosition(location.getSystem().getTextItems(),
                                    location.getPositionIndex()))
    {
        TextItemDialog dialog(this);
        if (dialog.exec() == QDialog::Accepted)
        {
            TextItem text(location.getPositionIndex(), dialog.getContents());

            myUndoManager->push(new AddTextItem(location, text),
                                location.getSystemIndex());
        }
        else
            myTextCommand->setChecked(false);
    }
    else
    {
        myUndoManager->push(new RemoveTextItem(location),
                            location.getSystemIndex());
    }
}

void PowerTabEditor::insertSystemAtEnd()
{
    insertSystem(
        static_cast<int>(getLocation().getScore().getSystems().size()));
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
    myUndoManager->push(
        new RemoveSystem(location.getScore(), location.getSystemIndex()),
        UndoManager::AFFECTS_ALL_SYSTEMS);
}

void PowerTabEditor::insertStaffBefore()
{
    insertStaff(getLocation().getStaffIndex());
}

void PowerTabEditor::insertStaffAfter()
{
    insertStaff(getLocation().getStaffIndex() + 1);
}

void PowerTabEditor::removeCurrentStaff()
{
    const ScoreLocation &location = getLocation();
    myUndoManager->push(new RemoveStaff(location), location.getSystemIndex());
}

void PowerTabEditor::updateNoteDuration(Position::DurationType duration)
{
    // Set the duration for future notes that are added.
    myActiveDurationType = duration;

    if (!getLocation().getSelectedPositions().empty())
    {
        myUndoManager->push(
            new EditNoteDuration(getLocation(), duration, false),
            getLocation().getSystemIndex());
    }
    else
        updateCommands();
}

static Position::DurationType changeDuration(Position::DurationType duration,
                                             bool increase)
{
    if ((increase && duration == Position::WholeNote) ||
        (!increase && duration == Position::SixtyFourthNote))
    {
        return duration;
    }

    return static_cast<Position::DurationType>(increase ? duration / 2
                                                        : duration * 2);
}

void PowerTabEditor::changeNoteDuration(bool increase)
{
    std::vector<Position *> selected_positions =
        getLocation().getSelectedPositions();

    if (selected_positions.empty())
    {
        if (increase && myActiveDurationType == Position::WholeNote)
            return;
        if (!increase && myActiveDurationType == Position::SixtyFourthNote)
            return;

        updateNoteDuration(changeDuration(myActiveDurationType, increase));
    }
    else
    {
        myUndoManager->beginMacro(tr("Edit Note Duration"));

        // Increase the duration of each selected position.
        for (const Position *pos : selected_positions)
        {
            ScoreLocation location(getLocation());
            location.setPositionIndex(pos->getPosition());
            location.setSelectionStart(pos->getPosition());

            Position::DurationType new_duration =
                changeDuration(pos->getDurationType(), increase);

            if (new_duration == pos->getDurationType())
                continue;

            // Update the active note duration to match the last selected note.
            if (pos == selected_positions.back())
                updateNoteDuration(new_duration);

            myUndoManager->push(
                new EditNoteDuration(location, new_duration, false),
                location.getSystemIndex());
        }

        myUndoManager->endMacro();
    }
}

void PowerTabEditor::addDot()
{
    ScoreLocation &location = getLocation();
    const Position *pos = location.getPosition();
    Q_ASSERT(pos);

    if (pos->hasProperty(Position::Dotted))
    {
        myUndoManager->push(new AddPositionProperty(
                                location, Position::DoubleDotted,
                                myDoubleDottedCommand->text()),
                            location.getSystemIndex());
    }
    else
    {
        myUndoManager->push(new AddPositionProperty(
                                location, Position::Dotted,
                                myDottedCommand->text()),
                            location.getSystemIndex());
    }
}

void PowerTabEditor::removeDot()
{
    ScoreLocation &location = getLocation();
    const Position *pos = location.getPosition();
    Q_ASSERT(pos);

    if (pos->hasProperty(Position::DoubleDotted))
    {
        myUndoManager->push(new AddPositionProperty(
                                location, Position::Dotted,
                                myDottedCommand->text()),
                            location.getSystemIndex());
    }
    else
    {
        myUndoManager->push(new RemovePositionProperty(
                                location, Position::Dotted,
                                myDottedCommand->text()),
                            location.getSystemIndex());
    }
}

void PowerTabEditor::editTiedNote()
{
    ScoreLocation &location = getLocation();
    const Voice &voice = location.getVoice();

    // If at an empty position, try to insert a new note that's tied to the
    // previous note.
    if (!location.hasSelection() && !location.getNote())
    {
        const int string = location.getString();
        const Note *prevNote = VoiceUtils::getPreviousNote(
            voice, location.getPositionIndex(), string);
        if (prevNote)
        {
            Note newNote(*prevNote);
            newNote.setProperty(Note::Tied);
            myUndoManager->push(
                new AddNote(location, newNote, myActiveDurationType),
                location.getSystemIndex());
        }
        else
            myTieCommand->setChecked(false);
    }
    else
    {
        if (!location.hasSelection())
        {
            const Note *note = location.getNote();
            if (!VoiceUtils::canTieNote(voice, location.getPositionIndex(),
                                        *note))
            {
                myTieCommand->setChecked(false);
                return;
            }
        }
        else
        {
            std::vector<Position *> positions = location.getSelectedPositions();
            // Check that all selected notes can be tied.
            for (const Position *pos : positions)
            {
                for (const Note &note : pos->getNotes())
                {
                    if (!VoiceUtils::canTieNote(voice, pos->getPosition(),
                                                note))
                    {
                        myTieCommand->setChecked(false);
                        return;
                    }
                }
            }
        }

        // Now, we can go ahead and add/remove a tie.
        editSimpleNoteProperty(myTieCommand, Note::Tied);
    }
}

void PowerTabEditor::editIrregularGrouping(bool setAsTriplet)
{
    ScoreLocation &location = getLocation();
    std::vector<Position *> selectedPositions = location.getSelectedPositions();
    Q_ASSERT(!selectedPositions.empty());

    if (selectedPositions.size() == 1)
    {
        // Remove an irregular group from this position.
        const int position = selectedPositions[0]->getPosition();
        auto groups = VoiceUtils::getIrregularGroupsInRange(location.getVoice(),
                                                            position, position);
        if (!groups.empty())
        {
            myUndoManager->push(
                new RemoveIrregularGrouping(location, *groups.back()),
                location.getSystemIndex());
        }
        return;
    }
    else
    {
        IrregularGrouping group(selectedPositions.front()->getPosition(),
                                static_cast<int>(selectedPositions.size()), 3,
                                2);

        if (setAsTriplet)
        {
            myUndoManager->push(new AddIrregularGrouping(location, group),
                                location.getSystemIndex());
        }
        else
        {
            IrregularGroupingDialog dialog(this);

            if (dialog.exec() == QDialog::Accepted)
            {
                group.setNotesPlayed(dialog.getNotesPlayed());
                group.setNotesPlayedOver(dialog.getNotesPlayedOver());
                myUndoManager->push(new AddIrregularGrouping(location, group),
                                    location.getSystemIndex());
            }
        }
    }
}

void PowerTabEditor::addRest()
{
    ScoreLocation &location = getLocation();
    const Position *pos = location.getPosition();
    const Position::DurationType duration =
        pos ? pos->getDurationType() : myActiveDurationType;

    myUndoManager->push(new AddRest(location, duration),
                        location.getSystemIndex());
}

void PowerTabEditor::editMultiBarRest()
{
    const ScoreLocation &location = getLocation();

    if (location.getPosition() && location.getPosition()->hasMultiBarRest())
    {
        myUndoManager->push(
            new RemovePosition(location, tr("Remove Multi-Bar Rest")),
            location.getSystemIndex());
    }
    else
    {
        // Verify that the bar is empty.
        {
            const System &system = location.getSystem();
            const Barline *prevBar =
                system.getPreviousBarline(location.getPositionIndex());
            if (!prevBar)
                prevBar = &system.getBarlines().front();
            const Barline *nextBar =
                system.getNextBarline(location.getPositionIndex());

            if (!ScoreUtils::findInRange(location.getVoice().getPositions(),
                                         prevBar->getPosition(),
                                         nextBar->getPosition()).empty())
            {
                QMessageBox message(this);
                message.setText(
                    tr("Cannot add a multi-bar rest to a non-empty measure."));
                message.exec();

                myMultibarRestCommand->setChecked(false);
                return;
            }
        }

        MultiBarRestDialog dialog(this);

        if (dialog.exec() == QDialog::Accepted)
        {
            myUndoManager->push(
                new AddMultiBarRest(location, dialog.getBarCount()),
                location.getSystemIndex());
        }
        else
            myMultibarRestCommand->setChecked(false);
    }
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

void PowerTabEditor::editTempoMarker()
{
    const ScoreLocation &location = getLocation();
    const TempoMarker *marker = ScoreUtils::findByPosition(
                location.getSystem().getTempoMarkers(),
                location.getPositionIndex());

    if (marker)
    {
        myUndoManager->push(new RemoveTempoMarker(location),
                            location.getSystemIndex());
    }
    else
    {
        TempoMarkerDialog dialog(this);
        if (dialog.exec() == QDialog::Accepted)
        {
            TempoMarker marker(dialog.getTempoMarker());
            marker.setPosition(location.getPositionIndex());

            myUndoManager->push(new AddTempoMarker(location, marker),
                                location.getSystemIndex());
        }
        else
            myTempoMarkerCommand->setChecked(false);
    }
}

void PowerTabEditor::editAlterationOfPace()
{
    const ScoreLocation &location = getLocation();
    const TempoMarker *marker = ScoreUtils::findByPosition(
        location.getSystem().getTempoMarkers(), location.getPositionIndex());

    if (marker)
    {
        Q_ASSERT(marker->getMarkerType() == TempoMarker::AlterationOfPace);
        myUndoManager->push(new RemoveTempoMarker(location),
                            location.getSystemIndex());
    }
    else
    {
        AlterationOfPaceDialog dialog(this);
        if (dialog.exec() == QDialog::Accepted)
        {
            TempoMarker marker(location.getPositionIndex());
            marker.setMarkerType(TempoMarker::AlterationOfPace);
            marker.setAlterationOfPace(dialog.getAlterationOfPaceType());

            myUndoManager->push(new AddTempoMarker(location, marker),
                                location.getSystemIndex());
        }
        else
            myAlterationOfPaceCommand->setChecked(false);
    }
}

void PowerTabEditor::editKeySignatureFromCaret()
{
    editKeySignature(getLocation());
}

void PowerTabEditor::editTimeSignatureFromCaret()
{
    editTimeSignature(getLocation());
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

void PowerTabEditor::editMusicalDirection()
{
    const ScoreLocation &location = getLocation();
    const Direction *direction = ScoreUtils::findByPosition(
                location.getSystem().getDirections(),
                location.getPositionIndex());

    if (direction)
    {
        myUndoManager->push(new RemoveDirection(location),
                            location.getSystemIndex());
    }
    else
    {
        DirectionDialog dialog(this);
        if (dialog.exec() == QDialog::Accepted)
        {
            Direction direction(dialog.getDirection());
            direction.setPosition(location.getPositionIndex());

            myUndoManager->push(new AddDirection(location, direction),
                                location.getSystemIndex());
        }
        else
            myDirectionCommand->setChecked(false);
    }
}

void PowerTabEditor::editRepeatEnding()
{
    ScoreLocation &location(getLocation());
    AlternateEnding *ending = ScoreUtils::findByPosition(
                location.getSystem().getAlternateEndings(),
                location.getPositionIndex());

    if (!ending)
    {
        AlternateEndingDialog dialog(this);
        if (dialog.exec() == QDialog::Accepted)
        {
            AlternateEnding altending(dialog.getAlternateEnding());
            altending.setPosition(location.getPositionIndex());
            myUndoManager->push(new AddAlternateEnding(location, altending),
                                location.getSystemIndex());
        }
        else
            myRepeatEndingCommand->setChecked(false);
    }
    else
    {
        myUndoManager->push(new RemoveAlternateEnding(location),
                            location.getSystemIndex());
    }
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

void PowerTabEditor::editHammerPull()
{
    ScoreLocation &location = getLocation();
    const Voice &voice = location.getVoice();
    const int position = location.getPositionIndex();
    const Note *note = location.getNote();
    if (!note)
        return;

    // TODO - support editing groups of notes.
    if (VoiceUtils::canHammerOnOrPullOff(voice, position, *note))
        editSimpleNoteProperty(myHammerPullCommand, Note::HammerOnOrPullOff);
    else
        myHammerPullCommand->setChecked(false);
}

void PowerTabEditor::editArtificialHarmonic()
{
    auto &location = getLocation();

    if (!location.getNote()->hasArtificialHarmonic())
    {
        ArtificialHarmonicDialog dialog(this);

        if (dialog.exec() == QDialog::Accepted)
        {
            myUndoManager->push(
                new AddArtificialHarmonic(location, dialog.getHarmonic()),
                location.getSystemIndex());
        }
        else
            myArtificialHarmonicCommand->setChecked(false);
    }
    else
    {
        myUndoManager->push(new RemoveArtificialHarmonic(location),
                            location.getSystemIndex());
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

void PowerTabEditor::editBend()
{
    const ScoreLocation &location = getLocation();
    const Note *note = location.getNote();
    Q_ASSERT(note);

    if (note->hasBend())
    {
        myUndoManager->push(new RemoveBend(location),
                            location.getSystemIndex());
    }
    else
    {
        BendDialog dialog(this);
        if (dialog.exec() == QDialog::Accepted)
        {
            myUndoManager->push(new AddBend(location, dialog.getBend()),
                                location.getSystemIndex());
        }
        else
            myBendCommand->setChecked(false);
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

void PowerTabEditor::addPlayer()
{
    ScoreLocation &location = getLocation();
    Score &score = location.getScore();
    Player player;

    // Create a unique name for the player.
    {
        std::vector<std::string> names;
        boost::range::transform(score.getPlayers(), std::back_inserter(names),
                                [](const Player &player) {
            return player.getDescription();
        });

        size_t i = score.getPlayers().size() + 1;
        while (true)
        {
            const std::string name = "Player " + std::to_string(i);

            if (std::find(names.begin(), names.end(), name) == names.end())
            {
                player.setDescription(name);
                break;
            }
            else
                ++i;
        }
    }

    auto settings = mySettingsManager->getReadHandle();
    player.setTuning(settings->get(Settings::DefaultTuning));

    myUndoManager->push(new AddPlayer(score, player),
                        UndoManager::AFFECTS_ALL_SYSTEMS);
}

void PowerTabEditor::addInstrument()
{
    ScoreLocation &location = getLocation();
    Score &score = location.getScore();
    Instrument instrument;

    auto settings = mySettingsManager->getReadHandle();

    // Create a unique name for the instrument.
    {
        std::vector<std::string> names;
        boost::range::transform(score.getInstruments(),
                                std::back_inserter(names),
                                [](const Instrument &instrument) {
            return instrument.getDescription();
        });

        const std::string default_name =
            settings->get(Settings::DefaultInstrumentName);

        size_t i = score.getInstruments().size() + 1;
        while (true)
        {
            const std::string name = default_name + " " + std::to_string(i);

            if (std::find(names.begin(), names.end(), name) == names.end())
            {
                instrument.setDescription(name);
                break;
            }
            else
                ++i;
        }
    }

    instrument.setMidiPreset(settings->get(Settings::DefaultInstrumentPreset));

    myUndoManager->push(new AddInstrument(location.getScore(), instrument),
                        UndoManager::AFFECTS_ALL_SYSTEMS);
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

void PowerTabEditor::editPlayer(int playerIndex, const Player &player,
                                bool undoable)
{
    ScoreLocation &location = getLocation();

    if (!undoable)
        location.getScore().getPlayers()[playerIndex] = player;
    else
    {
        myUndoManager->push(
            new EditPlayer(location.getScore(), playerIndex, player),
            UndoManager::AFFECTS_ALL_SYSTEMS);
    }
}

void PowerTabEditor::removePlayer(int index)
{
    ScoreLocation &location = getLocation();

    myUndoManager->push(new RemovePlayer(location.getScore(), index),
                        UndoManager::AFFECTS_ALL_SYSTEMS);
}

void PowerTabEditor::editInstrument(int index, const Instrument &instrument)
{
    ScoreLocation &location = getLocation();

    myUndoManager->push(
        new EditInstrument(location.getScore(), index, instrument),
        UndoManager::AFFECTS_ALL_SYSTEMS);
}

void PowerTabEditor::removeInstrument(int index)
{
    myUndoManager->push(new RemoveInstrument(getLocation().getScore(), index),
                        UndoManager::AFFECTS_ALL_SYSTEMS);
}

void PowerTabEditor::showTuningDictionary()
{
    TuningDictionaryDialog dialog(this, *myTuningDictionary);
    dialog.exec();
}

void PowerTabEditor::editViewFilters()
{
    ViewFilterDialog dialog(this);
    ViewFilterPresenter presenter(dialog, getLocation().getScore());
    if (presenter.exec())
    {
        myUndoManager->push(new EditViewFilters(getLocation().getScore(),
                                                presenter.getFilters()),
                            UndoManager::AFFECTS_ALL_SYSTEMS);
    }
}

bool PowerTabEditor::eventFilter(QObject *object, QEvent *event)
{
    // Don't handle key presses during playback.
    if (myIsPlaying)
        return QMainWindow::eventFilter(object, event);

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

void PowerTabEditor::closeEvent(QCloseEvent *event)
{
    while (myTabWidget->currentIndex() != -1)
    {
        if (!closeCurrentTab())
        {
            // Don't close if the user pressed Cancel.
            event->ignore();
            return;
        }
    }

    myTuningDictionary->save();

    {
        auto settings = mySettingsManager->getWriteHandle();
        settings->set(Settings::WindowState, saveState());
    }

    mySettingsManager->save(Paths::getConfigDir());

    QMainWindow::closeEvent(event);
}

void PowerTabEditor::dragEnterEvent(QDragEnterEvent *event)
{
    if (event->mimeData()->hasUrls())
    {
        for (const QUrl &url : event->mimeData()->urls())
        {
            if (!url.isLocalFile())
                return;
        }

        event->acceptProposedAction();
    }
}

void PowerTabEditor::dropEvent(QDropEvent *event)
{
    Q_ASSERT(event->mimeData()->hasUrls());
    for (const QUrl &url : event->mimeData()->urls())
        openFile(url.toLocalFile());
}

QString PowerTabEditor::getApplicationName() const
{
    QString name = QString("%1 %2 Beta").arg(
                AppInfo::APPLICATION_NAME,
                AppInfo::APPLICATION_VERSION);

#ifdef VERSION
    name += QString(" (v") + BOOST_STRINGIZE(VERSION) + ")";
#endif

    return name;
}

void PowerTabEditor::updateWindowTitle()
{
    QString name;

    if (myDocumentManager->hasOpenDocuments())
    {
        if (myDocumentManager->getCurrentDocument().hasFilename())
        {
            const Document &doc = myDocumentManager->getCurrentDocument();
            const QString path = QString::fromStdString(doc.getFilename());
            name = QFileInfo(path).fileName();
        }
        else
            name = tr("Untitled");

        // Need the [*] for using setWindowModified().
        name += "[*] - ";
    }

    name += getApplicationName();
    setWindowTitle(name);
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

    mySaveCommand = new Command(tr("Save"), "File.Save",
                                QKeySequence::Save, this);
    connect(mySaveCommand, SIGNAL(triggered()), this, SLOT(saveFile()));

    mySaveAsCommand = new Command(tr("Save As..."), "File.SaveAs",
                                  QKeySequence::SaveAs, this);
    connect(mySaveAsCommand, SIGNAL(triggered()), this, SLOT(saveFileAs()));

    myPrintCommand = new Command(tr("Print..."), "File.Print",
                                 QKeySequence::Print, this);
    connect(myPrintCommand, SIGNAL(triggered()), this, SLOT(printDocument()));

    myPrintPreviewCommand = new Command(
        tr("Print Preview..."), "File.PrintPreview", QKeySequence(), this);
    connect(myPrintPreviewCommand, SIGNAL(triggered()), this,
            SLOT(printPreview()));

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

    // Copy/Paste actions.
    myCutCommand = new Command(tr("Cut"), "Edit.Cut", QKeySequence::Cut, this);
    connect(myCutCommand, SIGNAL(triggered()), this, SLOT(cutSelectedNotes()));

    myCopyCommand = new Command(tr("Copy"), "Edit.Copy", QKeySequence::Copy,
                                this);
    connect(myCopyCommand, SIGNAL(triggered()), this,
            SLOT(copySelectedNotes()));

    myPasteCommand = new Command(tr("Paste"), "Edit.Paste",
                                 QKeySequence::Paste, this);
    connect(myPasteCommand, SIGNAL(triggered()), this, SLOT(pasteNotes()));

    myPolishCommand = new Command(tr("Polish Score"), "Edit.PolishScore",
                                  QKeySequence(Qt::SHIFT + Qt::Key_J), this);
    connect(myPolishCommand, &QAction::triggered, this,
            &PowerTabEditor::polishScore);

    myPolishSystemCommand = new Command(tr("Polish System"), "Edit.PolishSystem",
                                        QKeySequence(Qt::Key_J), this);
    connect(myPolishSystemCommand, &QAction::triggered, this,
            &PowerTabEditor::polishSystem);

    // File Information
    myFileInfoCommand =
        new Command(tr("File Information..."), "Edit.FileInformation",
                    QKeySequence(), this);
    connect(myFileInfoCommand, SIGNAL(triggered()), this,
            SLOT(editFileInformation()));

    // Playback-related actions.
    myPlayPauseCommand = new Command(tr("Play"), "Playback.PlayPause",
                                     Qt::Key_Space, this);
    connect(myPlayPauseCommand, SIGNAL(triggered()), this,
            SLOT(startStopPlayback()));

#ifdef Q_OS_MAC
    // Command-Space is used by Spotlight.
    QKeySequence play_start_seq = Qt::META + Qt::Key_Space;
#else
    QKeySequence play_start_seq = Qt::CTRL + Qt::Key_Space;
#endif
    myPlayFromStartOfMeasureCommand = new Command(
        tr("Play From Start Of Measure"), "Playback.PlayFromStartOfMeasure",
        play_start_seq, this);
    connect(myPlayFromStartOfMeasureCommand, &QAction::triggered, [this]() {
        startStopPlayback(/* from_measure_start */ true);
    });

    myStopCommand =
        new Command(tr("Stop"), "Playback.Stop", Qt::ALT + Qt::Key_Space, this);
    connect(myStopCommand, &QAction::triggered, this,
            &PowerTabEditor::stopPlayback);

    myRewindCommand = new Command(tr("Rewind"), "Playback.Rewind",
                                  Qt::ALT + Qt::Key_Left, this);
    connect(myRewindCommand, &QAction::triggered, this,
            &PowerTabEditor::rewindPlaybackToStart);

    myMetronomeCommand = new Command(tr("Metronome"), "Playback.Metronome",
                                     QKeySequence(), this);
    myMetronomeCommand->setCheckable(true);
    connect(myMetronomeCommand, &QAction::triggered, this,
            &PowerTabEditor::toggleMetronome);

    // Section navigation actions.
    myFirstSectionCommand =
        new Command(tr("First Section"), "Position.Section.FirstSection",
                    QKeySequence::MoveToStartOfDocument, this);
    connect(myFirstSectionCommand, SIGNAL(triggered()), this,
            SLOT(moveCaretToFirstSection()));

    myNextSectionCommand =
        new Command(tr("Next Section"), "Position.Section.NextSection",
                    QKeySequence::MoveToNextPage, this);
    connect(myNextSectionCommand, SIGNAL(triggered()), this,
            SLOT(moveCaretToNextSection()));

    myPrevSectionCommand =
        new Command(tr("Previous Section"), "Position.Section.PreviousSection",
                    QKeySequence::MoveToPreviousPage, this);
    connect(myPrevSectionCommand, SIGNAL(triggered()), this,
            SLOT(moveCaretToPrevSection()));

    myLastSectionCommand =
        new Command(tr("Last Section"), "Position.Section.LastSection",
                    QKeySequence::MoveToEndOfDocument, this);
    connect(myLastSectionCommand, SIGNAL(triggered()), this,
            SLOT(moveCaretToLastSection()));

    myShiftForwardCommand = new Command(tr("Shift Forward"),
                                        "Position.ShiftForward",
                                        QKeySequence(Qt::Key_Insert), this);
    connect(myShiftForwardCommand, SIGNAL(triggered()), this,
            SLOT(shiftForward()));

    myShiftBackwardCommand = new Command(tr("Shift Backward"),
                                         "Position.ShiftBackward",
                                         QKeySequence(Qt::SHIFT + Qt::Key_Insert),
                                         this);
    connect(myShiftBackwardCommand, SIGNAL(triggered()), this,
            SLOT(shiftBackward()));

    // Position-related actions.
#ifdef Q_OS_MAC
    // Use Command-Left instead of Control-Left, which is used for changing
    // desktops.
    QKeySequence move_start_seq = Qt::CTRL + Qt::Key_Left;
    QKeySequence move_end_seq = Qt::CTRL + Qt::Key_Right;
#else
    QKeySequence move_start_seq = QKeySequence::MoveToStartOfLine;
    QKeySequence move_end_seq = QKeySequence::MoveToEndOfLine;
#endif
    myStartPositionCommand =
        new Command(tr("Move to &Start"), "Position.Staff.MoveToStart",
                    move_start_seq, this);
    connect(myStartPositionCommand, SIGNAL(triggered()), this,
            SLOT(moveCaretToStart()));

    myNextPositionCommand =
        new Command(tr("&Next Position"), "Position.Staff.NextPosition",
                    QKeySequence::MoveToNextChar, this);
    connect(myNextPositionCommand, SIGNAL(triggered()), this,
            SLOT(moveCaretRight()));

    myPrevPositionCommand =
        new Command(tr("&Previous Position"), "Position.Staff.PreviousPosition",
                    QKeySequence::MoveToPreviousChar, this);
    connect(myPrevPositionCommand, SIGNAL(triggered()), this,
            SLOT(moveCaretLeft()));

    myNextStringCommand =
        new Command(tr("Next String"), "Position.Staff.NextString",
                    QKeySequence::MoveToNextLine, this);
    connect(myNextStringCommand, SIGNAL(triggered()), this,
            SLOT(moveCaretDown()));

    myPrevStringCommand =
        new Command(tr("Previous String"), "Position.Staff.PreviousString",
                    QKeySequence::MoveToPreviousLine, this);
    connect(myPrevStringCommand, SIGNAL(triggered()), this,
            SLOT(moveCaretUp()));

    myLastPositionCommand =
        new Command(tr("Move to &End"), "Position.Staff.MoveToEnd",
                    move_end_seq, this);
    connect(myLastPositionCommand, SIGNAL(triggered()), this,
            SLOT(moveCaretToEnd()));

    myNextStaffCommand =
        new Command(tr("Next Staff"), "Position.Staff.NextStaff",
                    Qt::ALT + Qt::Key_Down, this);
    connect(myNextStaffCommand, SIGNAL(triggered()), this,
            SLOT(moveCaretToNextStaff()));

    myPrevStaffCommand =
        new Command(tr("Previous Staff"), "Position.Staff.PreviousStaff",
                    Qt::ALT + Qt::Key_Up, this);
    connect(myPrevStaffCommand, SIGNAL(triggered()), this,
            SLOT(moveCaretToPrevStaff()));

    myNextBarCommand = new Command(tr("Next Bar"), "Position.Staff.NextBar",
                                   Qt::Key_Tab, this);
    connect(myNextBarCommand, SIGNAL(triggered()), this,
            SLOT(moveCaretToNextBar()));

    myPrevBarCommand =
        new Command(tr("Previous Bar"), "Position.Staff.PreviousBar",
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
#endif
    myRemoveNoteCommand = new Command(tr("Remove Note"), "Position.RemoveNote",
                                      QKeySequence::Delete, this);
    connect(myRemoveNoteCommand, SIGNAL(triggered()), this, SLOT(removeNote()));

    myRemovePositionCommand = new Command(tr("Remove Position"),
                                          "Position.RemovePosition",
                                          QKeySequence::DeleteEndOfWord, this);
    connect(myRemovePositionCommand, SIGNAL(triggered()), this,
            SLOT(removeSelectedPositions()));

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

    // Text-related actions.
    myChordNameCommand = new Command(tr("Chord Name..."), "Text.ChordName",
                                     Qt::Key_C, this);
    myChordNameCommand->setCheckable(true);
    connect(myChordNameCommand, SIGNAL(triggered()), this,
            SLOT(editChordName()));

    myTextCommand = new Command(tr("Text..."), "Text.TextItem",
                                QKeySequence(), this);
    myTextCommand->setCheckable(true);
    connect(myTextCommand, &QAction::triggered, this,
            &PowerTabEditor::editTextItem);

#if 0
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

    myInsertStaffBeforeCommand =
        new Command(tr("Insert Staff Before"), "Section.InsertStaffBefore",
                    QKeySequence(), this);
    connect(myInsertStaffBeforeCommand, &QAction::triggered, this,
            &PowerTabEditor::insertStaffBefore);

    myInsertStaffAfterCommand =
        new Command(tr("Insert Staff After"), "Section.InsertStaffAfter",
                    QKeySequence(), this);
    connect(myInsertStaffAfterCommand, &QAction::triggered, this,
            &PowerTabEditor::insertStaffAfter);

    myRemoveCurrentStaffCommand =
        new Command(tr("Remove Current Staff"), "Section.RemoveCurrentStaff",
                    QKeySequence(), this);
    connect(myRemoveCurrentStaffCommand, &QAction::triggered, this,
            &PowerTabEditor::removeCurrentStaff);

    myIncreaseLineSpacingCommand = new Command(tr("Increase"),
                                               "Section.LineSpacing.Increase",
                                               QKeySequence(), this);
    connect(myIncreaseLineSpacingCommand, &QAction::triggered, [=]() {
        adjustLineSpacing(1);
    });

    myDecreaseLineSpacingCommand = new Command(tr("Decrease"),
                                               "Section.LineSpacing.Decrease",
                                               QKeySequence(), this);
    connect(myDecreaseLineSpacingCommand, &QAction::triggered, [=]() {
        adjustLineSpacing(-1);
    });

    // Note-related actions.
    myNoteDurationGroup = new QActionGroup(this);
    createNoteDurationCommand(myWholeNoteCommand, tr("Whole"), "Notes.WholeNote",
                              Position::WholeNote);
    createNoteDurationCommand(myHalfNoteCommand, tr("Half"), "Notes.HalfNote",
                              Position::HalfNote);
    createNoteDurationCommand(myQuarterNoteCommand, tr("Quarter"),
                              "Notes.QuarterNote", Position::QuarterNote);
    createNoteDurationCommand(myEighthNoteCommand, tr("8th"),
                              "Notes.EighthNote", Position::EighthNote);
    createNoteDurationCommand(mySixteenthNoteCommand, tr("16th"),
                              "Notes.SixteenthNote", Position::SixteenthNote);
    createNoteDurationCommand(myThirtySecondNoteCommand, tr("32nd"),
                              "Notes.ThirtySecondNote",
                              Position::ThirtySecondNote);
    createNoteDurationCommand(mySixtyFourthNoteCommand, tr("64th"),
                              "Notes.SixtyFourthNote",
                              Position::SixtyFourthNote);

    myIncreaseDurationCommand = new Command(tr("Increase Duration"),
                                            "Notes.Duration.Increase",
                                            Qt::SHIFT + Qt::Key_Up, this);
    connect(myIncreaseDurationCommand, &QAction::triggered, [=]() {
        changeNoteDuration(true);
    });

    myDecreaseDurationCommand = new Command(tr("Decrease Duration"),
                                            "Notes.Duration.Decrease",
                                            Qt::SHIFT + Qt::Key_Down, this);
    connect(myDecreaseDurationCommand, &QAction::triggered, [=]() {
        changeNoteDuration(false);
    });

    createPositionPropertyCommand(myDottedCommand, tr("Dotted"), "Notes.Dotted",
                                  QKeySequence(), Position::Dotted);

    createPositionPropertyCommand(myDoubleDottedCommand, tr("Double Dotted"),
                                  "Notes.DoubleDotted", QKeySequence(),
                                  Position::DoubleDotted);

    myAddDotCommand = new Command(tr("Add Dot"), "Notes.Dot.Add",
                            Qt::SHIFT + Qt::Key_Right, this);
    connect(myAddDotCommand, SIGNAL(triggered()), this, SLOT(addDot()));

    myRemoveDotCommand = new Command(tr("Remove Dot"), "Notes.Dot.Remove",
                               Qt::SHIFT + Qt::Key_Left, this);
    connect(myRemoveDotCommand, SIGNAL(triggered()), this, SLOT(removeDot()));

    myTieCommand = new Command(tr("Tied"), "Notes.Tied", Qt::Key_Y, this);
    myTieCommand->setCheckable(true);
    connect(myTieCommand, SIGNAL(triggered()), this, SLOT(editTiedNote()));

    createNotePropertyCommand(myMutedCommand, tr("Muted"), "Notes.Muted",
                              Qt::Key_X, Note::Muted);
    createNotePropertyCommand(myGhostNoteCommand, tr("Ghost Note"),
                              "Notes.GhostNote", Qt::Key_G, Note::GhostNote);

    createPositionPropertyCommand(myFermataCommand, tr("Fermata"),
                                  "Notes.Fermata", Qt::Key_F, Position::Fermata);

    createPositionPropertyCommand(myLetRingCommand, tr("Let Ring"),
                                  "Notes.LetRing", QKeySequence(),
                                  Position::LetRing);

    createPositionPropertyCommand(myGraceNoteCommand, tr("Grace Note"),
                                  "Notes.GraceNote", QKeySequence(),
                                  Position::Acciaccatura);

    createPositionPropertyCommand(myStaccatoCommand, tr("Staccato"),
                                  "Notes.Staccato", Qt::Key_Z,
                                  Position::Staccato);

    createPositionPropertyCommand(myMarcatoCommand, tr("Accent"), "Notes.Accent",
                                  Qt::Key_A, Position::Marcato);

    createPositionPropertyCommand(mySforzandoCommand, tr("Heavy Accent"),
                                  "Notes.HeavyAccent", QKeySequence(),
                                  Position::Sforzando);

    // Octave actions
    createNotePropertyCommand(myOctave8vaCommand, tr("8va"), "Notes.Octave.8va",
                              QKeySequence(), Note::Octave8va);
    createNotePropertyCommand(myOctave15maCommand, tr("15ma"), "Notes.Octave.15ma",
                              QKeySequence(), Note::Octave15ma);
    createNotePropertyCommand(myOctave8vbCommand, tr("8vb"), "Notes.Octave.8vb",
                              QKeySequence(), Note::Octave8vb);
    createNotePropertyCommand(myOctave15mbCommand, tr("15mb"),
                              "Notes.Octave.15mb", QKeySequence(),
                              Note::Octave15mb);

    myTripletCommand = new Command(tr("Triplet"), "Notes.Triplet", Qt::Key_E, this);
    connect(myTripletCommand, &QAction::triggered, [=]() {
        editIrregularGrouping(true);
    });

    myIrregularGroupingCommand = new Command(
        tr("Irregular Grouping"), "Notes.IrregularGrouping", Qt::Key_I, this);
    connect(myIrregularGroupingCommand, &QAction::triggered, [=]() {
        editIrregularGrouping(false);
    });

    // Rest Actions.
    myRestDurationGroup = new QActionGroup(this);

    createRestDurationCommand(myWholeRestCommand, tr("Whole"), "Rests.Whole",
                              Position::WholeNote);
    createRestDurationCommand(myHalfRestCommand, tr("Half"), "Rests.Half",
                              Position::HalfNote);
    createRestDurationCommand(myQuarterRestCommand, tr("Quarter"),
                              "Rests.Quarter", Position::QuarterNote);
    createRestDurationCommand(myEighthRestCommand, tr("8th"),
                              "Rests.Eighth", Position::EighthNote);
    createRestDurationCommand(mySixteenthRestCommand, tr("16th"),
                              "Rests.Sixteenth", Position::SixteenthNote);
    createRestDurationCommand(myThirtySecondRestCommand, tr("32nd"),
                              "Rests.ThirtySecond",
                              Position::ThirtySecondNote);
    createRestDurationCommand(mySixtyFourthRestCommand, tr("64th"),
                              "Rests.SixtyFourth",
                              Position::SixtyFourthNote);

    myAddRestCommand = new Command(tr("Add Rest"), "Rests.AddRest", Qt::Key_R,
                                   this);
    connect(myAddRestCommand, SIGNAL(triggered()), this, SLOT(addRest()));

    myMultibarRestCommand = new Command(
        tr("Multi-Bar Rest..."), "Rests.MultibarRest", QKeySequence(), this);
    myMultibarRestCommand->setCheckable(true);
    connect(myMultibarRestCommand, SIGNAL(triggered()), this,
            SLOT(editMultiBarRest()));

    // Music Symbol Actions
    myRehearsalSignCommand =
        new Command(tr("Rehearsal Sign..."), "MusicSymbols.RehearsalSign",
                    Qt::SHIFT + Qt::Key_R, this);
    myRehearsalSignCommand->setCheckable(true);
    connect(myRehearsalSignCommand, SIGNAL(triggered()), this,
            SLOT(editRehearsalSign()));

    myTempoMarkerCommand = new Command(
        tr("Tempo Marker..."), "MusicSymbols.TempoMarker", Qt::Key_O, this);
    myTempoMarkerCommand->setCheckable(true);
    connect(myTempoMarkerCommand, SIGNAL(triggered()), this,
            SLOT(editTempoMarker()));

    myAlterationOfPaceCommand =
        new Command(tr("Alteration of Pace..."),
                    "MusicSymbols.AlterationOfPace", QKeySequence(), this);
    myAlterationOfPaceCommand->setCheckable(true);
    connect(myAlterationOfPaceCommand, SIGNAL(triggered()), this,
            SLOT(editAlterationOfPace()));

    myKeySignatureCommand = new Command(tr("Edit Key Signature..."),
                                        "MusicSymbols.EditKeySignature",
                                        Qt::Key_K, this);
    connect(myKeySignatureCommand, SIGNAL(triggered()), this,
            SLOT(editKeySignatureFromCaret()));

    myTimeSignatureCommand = new Command(tr("Edit Time Signature..."),
                                   "MusicSymbols.EditTimeSignature",
                                   Qt::Key_T, this);
    connect(myTimeSignatureCommand, SIGNAL(triggered()), this,
            SLOT(editTimeSignatureFromCaret()));

    myStandardBarlineCommand = new Command(tr("Insert Standard Barline"),
                                     "MusicSymbols.InsertStandardBarline",
                                     Qt::Key_B, this);
    connect(myStandardBarlineCommand, SIGNAL(triggered()), this,
            SLOT(insertStandardBarline()));

    myBarlineCommand = new Command(tr("Barline..."), "MusicSymbols.Barline",
                                   Qt::SHIFT + Qt::Key_B, this);
    connect(myBarlineCommand, SIGNAL(triggered()), this,
            SLOT(editBarlineFromCaret()));

    myDirectionCommand =
        new Command(tr("Musical Direction..."), "MusicSymbols.MusicalDirection",
                    Qt::SHIFT + Qt::Key_D, this);
    myDirectionCommand->setCheckable(true);
    connect(myDirectionCommand, SIGNAL(triggered()), this,
            SLOT(editMusicalDirection()));

    myRepeatEndingCommand =
        new Command(tr("Repeat Ending..."), "MusicSymbols.RepeatEnding",
                    Qt::SHIFT + Qt::Key_E, this);
    myRepeatEndingCommand->setCheckable(true);
    connect(myRepeatEndingCommand, SIGNAL(triggered()), this,
            SLOT(editRepeatEnding()));

    myDynamicCommand =
        new Command(tr("Dynamic..."), "MusicSymbols.Dynamic", Qt::Key_D, this);
    myDynamicCommand->setCheckable(true);
    connect(myDynamicCommand, SIGNAL(triggered()), this, SLOT(editDynamic()));
#if 0
    volumeSwellAct = new Command(tr("Volume Swell ..."), "MusicSymbols.VolumeSwell",
                                    QKeySequence(), this);
    volumeSwellAct->setCheckable(true);
    connect(volumeSwellAct, SIGNAL(triggered()), this, SLOT(editVolumeSwell()));
#endif

    // Tab Symbol Actions.
    myHammerPullCommand = new Command(tr("Hammer On/Pull Off"),
                                      "TabSymbols.HammerPull", Qt::Key_H, this);
    myHammerPullCommand->setCheckable(true);
    connect(myHammerPullCommand, SIGNAL(triggered()), this,
            SLOT(editHammerPull()));

    createNotePropertyCommand(myHammerOnFromNowhereCommand,
                              tr("Hammer On From Nowhere"),
                              "TabSymbols.HammerOnFromNowhere", QKeySequence(),
                              Note::HammerOnFromNowhere);

    createNotePropertyCommand(myPullOffToNowhereCommand,
                              tr("Pull Off To Nowhere"),
                              "TabSymbols.PullOffToNowhere", QKeySequence(),
                              Note::PullOffToNowhere);

    createNotePropertyCommand(myNaturalHarmonicCommand, tr("Natural Harmonic"),
                              "TabSymbols.NaturalHarmonic", QKeySequence(),
                              Note::NaturalHarmonic);

    myArtificialHarmonicCommand =
        new Command(tr("Artificial Harmonic..."),
                    "TabSymbols.ArtificialHarmonic", QKeySequence(), this);
    myArtificialHarmonicCommand->setCheckable(true);
    connect(myArtificialHarmonicCommand, SIGNAL(triggered()), this,
            SLOT(editArtificialHarmonic()));

    myTappedHarmonicCommand = new Command(tr("Tapped Harmonic..."),
                                          "TabSymbols.TappedHarmonic",
                                          QKeySequence(), this);
    myTappedHarmonicCommand->setCheckable(true);
    connect(myTappedHarmonicCommand, SIGNAL(triggered()), this,
            SLOT(editTappedHarmonic()));

    myBendCommand =
        new Command(tr("Bend..."), "TabSymbols.Bend", QKeySequence(), this);
    myBendCommand->setCheckable(true);
    connect(myBendCommand, &QAction::triggered, this,
            &PowerTabEditor::editBend);

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

    createNotePropertyCommand(mySlideIntoFromAboveCommand,
                              tr("Slide Into From Above"),
                              "TabSymbols.SlideInto.FromAbove", QKeySequence(),
                              Note::SlideIntoFromAbove);
    createNotePropertyCommand(mySlideIntoFromBelowCommand,
                              tr("Slide Into From Below"),
                              "TabSymbols.SlideInto.FromBelow", QKeySequence(),
                              Note::SlideIntoFromBelow);

    createNotePropertyCommand(myShiftSlideCommand, tr("Shift Slide"),
                              "TabSymbols.ShiftSlide", Qt::Key_S,
                              Note::ShiftSlide);
    createNotePropertyCommand(myLegatoSlideCommand, tr("Legato Slide"),
                              "TabSymbols.LegatoSlide", Qt::Key_L,
                              Note::LegatoSlide);

    createNotePropertyCommand(mySlideOutOfDownwardsCommand,
                              tr("Slide Out Of Downwards"),
                              "TabSymbols.SlideOutOf.Downwards", QKeySequence(),
                              Note::SlideOutOfDownwards);
    createNotePropertyCommand(mySlideOutOfUpwardsCommand,
                              tr("Slide Out Of Upwards"),
                              "TabSymbols.SlideOutOf.Upwards", QKeySequence(),
                              Note::SlideOutOfUpwards);

    // Player menu.
    myAddPlayerCommand =
        new Command(tr("Add Player"), "Player.AddPlayer", QKeySequence(), this);
    connect(myAddPlayerCommand, &QAction::triggered, this,
            &PowerTabEditor::addPlayer);

    myAddInstrumentCommand = new Command(
        tr("Add Instrument"), "Player.AddInstrument", QKeySequence(), this);
    connect(myAddInstrumentCommand, &QAction::triggered, this,
            &PowerTabEditor::addInstrument);

    myPlayerChangeCommand = new Command(tr("Player Change..."),
                                        "Player.PlayerChange", QKeySequence(),
                                        this);
    myPlayerChangeCommand->setCheckable(true);
    connect(myPlayerChangeCommand, SIGNAL(triggered()), this,
            SLOT(editPlayerChange()));

    myShowTuningDictionaryCommand = new Command(tr("Tuning Dictionary..."),
                                                "Player.TuningDictionary",
                                                QKeySequence(), this);
    connect(myShowTuningDictionaryCommand, SIGNAL(triggered()),
            this, SLOT(showTuningDictionary()));

    myEditViewFiltersCommand =
        new Command(tr("Edit View Filters..."), "Player.EditViewFilters",
                    QKeySequence(), this);
    connect(myEditViewFiltersCommand, &QAction::triggered, this,
            &PowerTabEditor::editViewFilters);

    // Window Menu commands.
    
#ifdef Q_OS_MAC
    // NextChild is Command-{ on OS X, so use the more conventional Control-Tab
    // to match Safari, Finder, etc.
    QKeySequence next_tab_seq = Qt::META + Qt::Key_Tab;
    QKeySequence prev_tab_seq = Qt::META + Qt::SHIFT + Qt::Key_Tab;
#else
    QKeySequence next_tab_seq = QKeySequence::NextChild;
    QKeySequence prev_tab_seq = QKeySequence::PreviousChild;
#endif
    
    myNextTabCommand = new Command(tr("Next Tab"), "Window.NextTab",
                                   next_tab_seq, this);
    connect(myNextTabCommand, &QAction::triggered, [=]() {
        cycleTab(1);
    });

    myPrevTabCommand = new Command(tr("Previous Tab"), "Window.PreviousTab",
                                   prev_tab_seq, this);
    connect(myPrevTabCommand, &QAction::triggered, [=]() {
        cycleTab(-1);
    });

    // Help menu commands.
    myReportBugCommand = new Command(tr("Report Bug..."), "Help.ReportBug",
                                     QKeySequence(), this);
    connect(myReportBugCommand, &QAction::triggered, [=]() {
        QDesktopServices::openUrl(QUrl(AppInfo::BUG_TRACKER_URL));
    });

    myMixerDockWidgetCommand =
        createCommandWrapper(myMixerDockWidget->toggleViewAction(),
                             "Window.Mixer", QKeySequence(), this);
    myInstrumentDockWidgetCommand =
        createCommandWrapper(myInstrumentDockWidget->toggleViewAction(),
                             "Window.Instruments", QKeySequence(), this);
}

void PowerTabEditor::loadKeyboardShortcuts()
{
    auto settings = mySettingsManager->getReadHandle();
    for (auto command : getCommands())
        command->load(*settings);
}

void PowerTabEditor::saveKeyboardShortcuts() const
{
    auto settings = mySettingsManager->getWriteHandle();
    for (auto command : getCommands())
        command->save(*settings);
}

std::vector<const Command *> PowerTabEditor::getCommands() const
{
    return findChildren<const Command *>().toVector().toStdVector();
}

std::vector<Command *> PowerTabEditor::getCommands()
{
    return findChildren<Command *>().toVector().toStdVector();
}

void PowerTabEditor::createMixer()
{
    myMixerDockWidget = new QDockWidget(tr("Mixer"), this);
    myMixerDockWidget->setAllowedAreas(Qt::BottomDockWidgetArea);
    myMixerDockWidget->setFeatures(QDockWidget::DockWidgetClosable);
    // The object name is used by QMainWindow::saveState().
    myMixerDockWidget->setObjectName("Mixer");

    QScrollArea *scroll = new QScrollArea(this);
    scroll->setMinimumSize(0, 150);

    myMixer = new Mixer(scroll, *myTuningDictionary, myPlayerEditPubSub,
                        myPlayerRemovePubSub);

    scroll->setWidget(myMixer);
    myMixerDockWidget->setWidget(scroll);
    addDockWidget(Qt::BottomDockWidgetArea, myMixerDockWidget);

    myPlayerEditPubSub.subscribe([=](int index, const Player & player,
                                 bool undoable) {
        editPlayer(index, player, undoable);
    });
    myPlayerRemovePubSub.subscribe([=](int index) {
        removePlayer(index);
    });
}

void PowerTabEditor::createInstrumentPanel()
{
    myInstrumentDockWidget = new QDockWidget(tr("Instruments"), this);
    myInstrumentDockWidget->setAllowedAreas(Qt::BottomDockWidgetArea);
    myInstrumentDockWidget->setFeatures(QDockWidget::DockWidgetClosable);
    myInstrumentDockWidget->setObjectName("Instruments");

    QScrollArea *scroll = new QScrollArea(this);
    scroll->setMinimumSize(0, 150);

    myInstrumentPanel = new InstrumentPanel(scroll, myInstrumentEditPubSub,
                                            myInstrumentRemovePubSub);

    scroll->setWidget(myInstrumentPanel);
    myInstrumentDockWidget->setWidget(scroll);
    addDockWidget(Qt::BottomDockWidgetArea, myInstrumentDockWidget);

    myInstrumentEditPubSub.subscribe([=](int index, const Instrument &instrument) {
        editInstrument(index, instrument);
    });
    myInstrumentRemovePubSub.subscribe([=](int index) {
        removeInstrument(index);
    });
}

Command *PowerTabEditor::createCommandWrapper(
    QAction *action, const QString &id, const QKeySequence &defaultShortcut,
    QObject *parent)
{
    Command *command = new Command(action->text(), id, defaultShortcut, parent);
    command->setCheckable(action->isCheckable());
    command->setChecked(action->isChecked());

    // Keep the two actions in sync with each other.
    connect(command, &QAction::triggered, action, &QAction::triggered);
    connect(action, &QAction::toggled, command, &QAction::setChecked);

    return command;
}

void PowerTabEditor::createNoteDurationCommand(
        Command *&command, const QString &menuName, const QString &commandName,
        Position::DurationType durationType)
{
    command = new Command(menuName, commandName, QKeySequence(), this);
    command->setCheckable(true);
    connect(command, &QAction::triggered, [=]() {
        updateNoteDuration(durationType);
    });
    myNoteDurationGroup->addAction(command);
}

void PowerTabEditor::createRestDurationCommand(
        Command *&command, const QString &menuName, const QString &commandName,
        Position::DurationType durationType)
{
    command = new Command(menuName, commandName, QKeySequence(), this);
    command->setCheckable(true);
    connect(command, &QAction::triggered, [=]() {
        editRest(durationType);
    });
    myRestDurationGroup->addAction(command);
}

void PowerTabEditor::createNotePropertyCommand(
        Command *&command, const QString &menuName, const QString &commandName,
        const QKeySequence &shortcut, Note::SimpleProperty property)
{
    command = new Command(menuName, commandName, shortcut, this);
    command->setCheckable(true);
    connect(command, &QAction::triggered, [=]() {
        editSimpleNoteProperty(command, property);
    });
}

void PowerTabEditor::createPositionPropertyCommand(
        Command *&command, const QString &menuName, const QString &commandName,
        const QKeySequence &shortcut, Position::SimpleProperty property)
{
    command = new Command(menuName, commandName, shortcut, this);
    command->setCheckable(true);
    connect(command, &QAction::triggered, [=]() {
        editSimplePositionProperty(command, property);
    });
}

void PowerTabEditor::createMenus()
{
    // File Menu.
    myFileMenu = menuBar()->addMenu(tr("&File"));
    myFileMenu->addAction(myNewDocumentCommand);
    myFileMenu->addAction(myOpenFileCommand);
    myFileMenu->addAction(myCloseTabCommand);
    myFileMenu->addSeparator();
    myFileMenu->addAction(mySaveCommand);
    myFileMenu->addAction(mySaveAsCommand);
    myFileMenu->addSeparator();
    myFileMenu->addAction(myPrintCommand);
    myFileMenu->addAction(myPrintPreviewCommand);
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
    myEditMenu->addSeparator();
    myEditMenu->addAction(myCutCommand);
    myEditMenu->addAction(myCopyCommand);
    myEditMenu->addAction(myPasteCommand);
    myEditMenu->addSeparator();
    myEditMenu->addAction(myRemoveNoteCommand);
    myEditMenu->addAction(myRemovePositionCommand);
    myEditMenu->addSeparator();
    myEditMenu->addAction(myPolishCommand);
    myEditMenu->addAction(myPolishSystemCommand);
    myEditMenu->addSeparator();
    myEditMenu->addAction(myFileInfoCommand);
    // Playback Menu.
    myPlaybackMenu = menuBar()->addMenu(tr("Play&back"));
    myPlaybackMenu->addAction(myPlayPauseCommand);
    myPlaybackMenu->addAction(myPlayFromStartOfMeasureCommand);
    myPlaybackMenu->addAction(myStopCommand);
    myPlaybackMenu->addAction(myRewindCommand);
    myPlaybackMenu->addAction(myMetronomeCommand);

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
#endif
    myPositionMenu->addSeparator();
    myPositionMenu->addAction(myShiftForwardCommand);
    myPositionMenu->addAction(myShiftBackwardCommand);
    myPositionMenu->addSeparator();
    myPositionMenu->addAction(myRemoveNoteCommand);
    myPositionMenu->addAction(myRemovePositionCommand);
    myPositionMenu->addSeparator();
    myPositionMenu->addAction(myGoToBarlineCommand);
    myPositionMenu->addAction(myGoToRehearsalSignCommand);

    // Text Menu.
    myTextMenu = menuBar()->addMenu(tr("&Text"));
    myTextMenu->addAction(myChordNameCommand);
    myTextMenu->addAction(myTextCommand);

    // Section Menu.
    mySectionMenu = menuBar()->addMenu(tr("&Section"));
    mySectionMenu->addAction(myInsertSystemAtEndCommand);
    mySectionMenu->addAction(myInsertSystemBeforeCommand);
    mySectionMenu->addAction(myInsertSystemAfterCommand);
    mySectionMenu->addAction(myRemoveCurrentSystemCommand);
    mySectionMenu->addSeparator();
    mySectionMenu->addAction(myInsertStaffBeforeCommand);
    mySectionMenu->addAction(myInsertStaffAfterCommand);
    mySectionMenu->addAction(myRemoveCurrentStaffCommand);
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
    myNotesMenu->addAction(myAddDotCommand);
    myNotesMenu->addAction(myRemoveDotCommand);
    myNotesMenu->addSeparator();
    myNotesMenu->addAction(myTieCommand);
    myNotesMenu->addAction(myMutedCommand);
    myNotesMenu->addAction(myGhostNoteCommand);
    myNotesMenu->addSeparator();
    myNotesMenu->addAction(myLetRingCommand);
    myNotesMenu->addAction(myFermataCommand);
    myNotesMenu->addAction(myGraceNoteCommand);
    myNotesMenu->addAction(myStaccatoCommand);
    myNotesMenu->addAction(myMarcatoCommand);
    myNotesMenu->addAction(mySforzandoCommand);
    myNotesMenu->addSeparator();

    myOctaveMenu = myNotesMenu->addMenu(tr("Octave"));
    myOctaveMenu->addAction(myOctave8vaCommand);
    myOctaveMenu->addAction(myOctave15maCommand);
    myOctaveMenu->addAction(myOctave8vbCommand);
    myOctaveMenu->addAction(myOctave15mbCommand);

    myNotesMenu->addAction(myTripletCommand);
    myNotesMenu->addAction(myIrregularGroupingCommand);

    // Rests Menu.
    myRestsMenu = menuBar()->addMenu(tr("Rests"));
    myRestsMenu->addAction(myWholeRestCommand);
    myRestsMenu->addAction(myHalfRestCommand);
    myRestsMenu->addAction(myQuarterRestCommand);
    myRestsMenu->addAction(myEighthRestCommand);
    myRestsMenu->addAction(mySixteenthRestCommand);
    myRestsMenu->addAction(myThirtySecondRestCommand);
    myRestsMenu->addAction(mySixtyFourthRestCommand);
    myRestsMenu->addSeparator();
    myRestsMenu->addAction(myAddRestCommand);
    myRestsMenu->addAction(myMultibarRestCommand);

    // Music Symbols Menu.
    myMusicSymbolsMenu = menuBar()->addMenu(tr("&Music Symbols"));
    myMusicSymbolsMenu->addAction(myRehearsalSignCommand);
    myMusicSymbolsMenu->addAction(myTempoMarkerCommand);
    myMusicSymbolsMenu->addAction(myAlterationOfPaceCommand);
    myMusicSymbolsMenu->addAction(myKeySignatureCommand);
    myMusicSymbolsMenu->addAction(myTimeSignatureCommand);
    myMusicSymbolsMenu->addAction(myStandardBarlineCommand);
    myMusicSymbolsMenu->addAction(myBarlineCommand);
    myMusicSymbolsMenu->addAction(myDirectionCommand);
    myMusicSymbolsMenu->addAction(myRepeatEndingCommand);
    myMusicSymbolsMenu->addAction(myDynamicCommand);
#if 0
    myMusicSymbolsMenu->addAction(volumeSwellAct);

#endif
    // Tab Symbols Menu
    myTabSymbolsMenu = menuBar()->addMenu(tr("&Tab Symbols"));
    myHammerOnMenu = myTabSymbolsMenu->addMenu(tr("&Hammer Ons/Pull Offs"));
    myHammerOnMenu->addAction(myHammerPullCommand);
    myHammerOnMenu->addAction(myHammerOnFromNowhereCommand);
    myHammerOnMenu->addAction(myPullOffToNowhereCommand);

    myTabSymbolsMenu->addAction(myNaturalHarmonicCommand);
    myTabSymbolsMenu->addAction(myArtificialHarmonicCommand);
    myTabSymbolsMenu->addAction(myTappedHarmonicCommand);
    myTabSymbolsMenu->addSeparator();

	myTabSymbolsMenu->addAction(myBendCommand);
	myTabSymbolsMenu->addSeparator();

    mySlideIntoMenu = myTabSymbolsMenu->addMenu(tr("Slide Into"));
    mySlideIntoMenu->addAction(mySlideIntoFromBelowCommand);
    mySlideIntoMenu->addAction(mySlideIntoFromAboveCommand);

    myTabSymbolsMenu->addAction(myShiftSlideCommand);
    myTabSymbolsMenu->addAction(myLegatoSlideCommand);

    mySlideOutOfMenu = myTabSymbolsMenu->addMenu(tr("Slide Out Of"));
    mySlideOutOfMenu->addAction(mySlideOutOfDownwardsCommand);
    mySlideOutOfMenu->addAction(mySlideOutOfUpwardsCommand);

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

    // Player Menu.
    myPlayerMenu = menuBar()->addMenu(tr("&Player"));
    myPlayerMenu->addAction(myAddPlayerCommand);
    myPlayerMenu->addAction(myAddInstrumentCommand);
    myPlayerMenu->addAction(myPlayerChangeCommand);
    myPlayerMenu->addSeparator();
    myPlayerMenu->addAction(myShowTuningDictionaryCommand);
    myPlayerMenu->addAction(myEditViewFiltersCommand);

    // Window Menu.
    myWindowMenu = menuBar()->addMenu(tr("&Window"));
    myWindowMenu->addAction(myNextTabCommand);
    myWindowMenu->addAction(myPrevTabCommand);
    myWindowMenu->addSeparator();
    myWindowMenu->addAction(myMixerDockWidgetCommand);
    myWindowMenu->addAction(myInstrumentDockWidgetCommand);

    // Help menu.
    myHelpMenu = menuBar()->addMenu(tr("&Help"));
    myHelpMenu->addAction(myReportBugCommand);
}

void PowerTabEditor::createTabArea()
{
    myTabWidget = new QTabWidget(this);
    myTabWidget->setDocumentMode(true);
    myTabWidget->setTabsClosable(true);

    connect(myTabWidget, SIGNAL(tabCloseRequested(int)), this,
            SLOT(closeTab(int)));
    connect(myTabWidget, SIGNAL(currentChanged(int)), this,
            SLOT(switchTab(int)));

    myPlaybackWidget =
        new PlaybackWidget(*myPlayPauseCommand, *myRewindCommand,
                           *myStopCommand, *myMetronomeCommand, this);

    connect(myPlaybackWidget, &PlaybackWidget::activeVoiceChanged, this,
            &PowerTabEditor::updateActiveVoice);

    connect(myPlaybackWidget, &PlaybackWidget::activeFilterChanged, this,
            &PowerTabEditor::updateActiveFilter);

    connect(myPlaybackWidget, &PlaybackWidget::zoomChanged, this,
            &PowerTabEditor::updateZoom);

    auto update_metronome_state = [&]() {
        auto settings = mySettingsManager->getReadHandle();
        myMetronomeCommand->setChecked(
            settings->get(Settings::MetronomeEnabled));
    };

    update_metronome_state();
    mySettingsManager->subscribeToChanges(update_metronome_state);

    myPlaybackArea = new QWidget(this);
    QVBoxLayout *layout = new QVBoxLayout(myPlaybackArea);
    layout->addWidget(myTabWidget);
    layout->addWidget(myPlaybackWidget, 0, Qt::AlignHCenter);
    layout->setMargin(0);
    layout->setSpacing(0);

    enableEditing(false);
    myPlaybackWidget->setEnabled(false);
}

void PowerTabEditor::setPreviousDirectory(const QString &fileName)
{
    QFileInfo fileInfo(fileName);
    myPreviousDirectory = fileInfo.absolutePath();

    auto settings = mySettingsManager->getWriteHandle();
    settings->set(Settings::PreviousDirectory,
                  myPreviousDirectory.toStdString());
}

void PowerTabEditor::setupNewTab()
{
    auto start = std::chrono::high_resolution_clock::now();
    qDebug() << "Tab creation started ...";

    Q_ASSERT(myDocumentManager->hasOpenDocuments());
    Document &doc = myDocumentManager->getCurrentDocument();

    doc.getCaret().subscribeToChanges([=]() {
        updateCommands();
        updateLocationLabel();
    });

    auto scorearea = new ScoreArea(this);
    scorearea->renderDocument(doc);
    scorearea->installEventFilter(this);

    // Connect the signals for mouse clicks on time signatures, barlines, etc.
    // to the appropriate event handlers.
    scorearea->getClickPubSub()->subscribe([=](ClickType type,
                                               const ScoreLocation &location) {
        switch (type)
        {
            case ClickType::Barline:
                editBarline(location);
                break;
            case ClickType::TimeSignature:
                editTimeSignature(location);
                break;
            case ClickType::KeySignature:
                editKeySignature(location);
                break;
            case ClickType::TabClef:
                editStaff(location.getSystemIndex(), location.getStaffIndex());
                break;
            case ClickType::Clef:
                editClef(location.getSystemIndex(), location.getStaffIndex());
                break;
            case ClickType::Selection:
                if (!getCaret().isInPlaybackMode())
                    getCaret().moveToLocation(location);
                break;
            default:
                Q_ASSERT(false);
                break;
        }
    });

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

    myMixer->reset(doc.getScore());
    myInstrumentPanel->reset(doc.getScore());
    myPlaybackWidget->reset(doc);

    // Switch to the new document.
    myTabWidget->setCurrentIndex(myDocumentManager->getCurrentDocumentIndex());
    myPlaybackWidget->setEnabled(true);

    enableEditing(true);
    updateCommands();
    scorearea->setFocus();

    auto end = std::chrono::high_resolution_clock::now();
    qDebug() << "Tab opened in"
             << std::chrono::duration_cast<std::chrono::milliseconds>(
                    end - start).count() << "ms";
}

namespace
{
inline void updatePositionProperty(Command *command, const Position *pos,
                                   Position::SimpleProperty property)
{
    command->setEnabled(pos != nullptr);
    command->setChecked(pos && pos->hasProperty(property));
}

inline void updateNoteProperty(Command *command, const Note *note,
                               Note::SimpleProperty property)
{
    command->setEnabled(note != nullptr);
    command->setChecked(note && note->hasProperty(property));
}
}

void PowerTabEditor::updateCommands()
{
    if (myIsPlaying)
        return;

    ScoreLocation location = getLocation();
    const Score &score = location.getScore();
    if (score.getSystems().empty())
        return;

    const System &system = location.getSystem();
    if (system.getStaves().empty())
        return;

    const Staff &staff = location.getStaff();
    const Position *pos = location.getPosition();
    const int position = location.getPositionIndex();
    const Note *note = location.getNote();
    const Barline *barline = location.getBarline();
    const TempoMarker *tempoMarker =
        ScoreUtils::findByPosition(system.getTempoMarkers(), position);
    const AlternateEnding *altEnding =
        ScoreUtils::findByPosition(system.getAlternateEndings(), position);
    const Dynamic *dynamic =
        ScoreUtils::findByPosition(staff.getDynamics(), position);
    const bool hasSelection = !location.getSelectedPositions().empty();

    myRemoveCurrentSystemCommand->setEnabled(score.getSystems().size() > 1);
    myRemoveCurrentStaffCommand->setEnabled(system.getStaves().size() > 1);
    myIncreaseLineSpacingCommand->setEnabled(score.getLineSpacing() <
                                             Score::MAX_LINE_SPACING);
    myDecreaseLineSpacingCommand->setEnabled(score.getLineSpacing() >
                                             Score::MIN_LINE_SPACING);
    myShiftBackwardCommand->setEnabled(!pos && (position == 0 || !barline) &&
                                       !tempoMarker && !altEnding && !dynamic);
    myRemoveNoteCommand->setEnabled(note != nullptr);
    myRemovePositionCommand->setEnabled(pos || barline || hasSelection);

    myChordNameCommand->setChecked(
        ScoreUtils::findByPosition(system.getChords(), position) != nullptr);
    myTextCommand->setChecked(
        ScoreUtils::findByPosition(system.getTextItems(), position) != nullptr);

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
    myDecreaseDurationCommand->setEnabled(durationType !=
                                          Position::SixtyFourthNote);

    updatePositionProperty(myDottedCommand, pos, Position::Dotted);
    updatePositionProperty(myDoubleDottedCommand, pos, Position::DoubleDotted);
    myAddDotCommand->setEnabled(pos &&
                                !pos->hasProperty(Position::DoubleDotted));
    myRemoveDotCommand->setEnabled(pos &&
                                   (pos->hasProperty(Position::Dotted) ||
                                    pos->hasProperty(Position::DoubleDotted)));

    if (note)
    {
        myTieCommand->setText(tr("Tied"));
        myTieCommand->setChecked(note->hasProperty(Note::Tied));
        myTieCommand->setEnabled(true);
    }
    else if (!barline)
    {
        myTieCommand->setText(tr("Insert Tied Note"));
        myTieCommand->setChecked(false);
        myTieCommand->setEnabled(true);
    }
    else
        myTieCommand->setEnabled(false);

    updateNoteProperty(myMutedCommand, note, Note::Muted);
    updateNoteProperty(myGhostNoteCommand, note, Note::GhostNote);
    updatePositionProperty(myLetRingCommand, pos, Position::LetRing);
    updatePositionProperty(myFermataCommand, pos, Position::Fermata);
    updatePositionProperty(myGraceNoteCommand, pos, Position::Acciaccatura);
    updatePositionProperty(myStaccatoCommand, pos, Position::Staccato);
    updatePositionProperty(myMarcatoCommand, pos, Position::Marcato);
    updatePositionProperty(mySforzandoCommand, pos, Position::Sforzando);

    updateNoteProperty(myOctave8vaCommand, note, Note::Octave8va);
    updateNoteProperty(myOctave8vbCommand, note, Note::Octave8vb);
    updateNoteProperty(myOctave15maCommand, note, Note::Octave15ma);
    updateNoteProperty(myOctave15mbCommand, note, Note::Octave15mb);

    myAddRestCommand->setEnabled(!pos || !pos->isRest());

    myTripletCommand->setEnabled(pos != nullptr);
    myIrregularGroupingCommand->setEnabled(pos != nullptr);

    myMultibarRestCommand->setEnabled(!barline || position == 0);
    myMultibarRestCommand->setChecked(pos && pos->hasMultiBarRest());

    myRehearsalSignCommand->setEnabled(barline != nullptr);
    myRehearsalSignCommand->setChecked(barline && barline->hasRehearsalSign());

    const bool isAlterationOfPace =
        (tempoMarker &&
         tempoMarker->getMarkerType() == TempoMarker::AlterationOfPace);
    myTempoMarkerCommand->setEnabled(!tempoMarker || !isAlterationOfPace);
    myTempoMarkerCommand->setChecked(tempoMarker && !isAlterationOfPace);
    myAlterationOfPaceCommand->setEnabled(!tempoMarker || isAlterationOfPace);
    myAlterationOfPaceCommand->setChecked(isAlterationOfPace);

    myKeySignatureCommand->setEnabled(barline != nullptr);
    myTimeSignatureCommand->setEnabled(barline != nullptr);
    myStandardBarlineCommand->setEnabled(!pos && !barline);
    myDirectionCommand->setChecked(
        ScoreUtils::findByPosition(system.getDirections(), position) !=
        nullptr);
    myRepeatEndingCommand->setChecked(altEnding != nullptr);
    myDynamicCommand->setChecked(dynamic != nullptr);

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

    myHammerPullCommand->setEnabled(note != nullptr);
    myHammerPullCommand->setChecked(note &&
                                    note->hasProperty(Note::HammerOnOrPullOff));

    updateNoteProperty(myHammerOnFromNowhereCommand, note,
                       Note::HammerOnFromNowhere);
    updateNoteProperty(myPullOffToNowhereCommand, note, Note::PullOffToNowhere);
    updateNoteProperty(myNaturalHarmonicCommand, note, Note::NaturalHarmonic);
    myArtificialHarmonicCommand->setEnabled(note != nullptr);
    myArtificialHarmonicCommand->setChecked(note &&
                                            note->hasArtificialHarmonic());
    myTappedHarmonicCommand->setEnabled(note != nullptr);
    myTappedHarmonicCommand->setChecked(note && note->hasTappedHarmonic());

    myBendCommand->setEnabled(note != nullptr);
    myBendCommand->setChecked(note && note->hasBend());

    updateNoteProperty(mySlideIntoFromAboveCommand, note,
                       Note::SlideIntoFromAbove);
    updateNoteProperty(mySlideIntoFromBelowCommand, note,
                       Note::SlideIntoFromBelow);
    updateNoteProperty(myShiftSlideCommand, note, Note::ShiftSlide);
    updateNoteProperty(myLegatoSlideCommand, note, Note::LegatoSlide);
    updateNoteProperty(mySlideOutOfDownwardsCommand, note,
                       Note::SlideOutOfDownwards);
    updateNoteProperty(mySlideOutOfUpwardsCommand, note,
                       Note::SlideOutOfUpwards);

    updatePositionProperty(myVibratoCommand, pos, Position::Vibrato);
    updatePositionProperty(myWideVibratoCommand, pos, Position::WideVibrato);
    updatePositionProperty(myPalmMuteCommand, pos, Position::PalmMuting);
    updatePositionProperty(myTremoloPickingCommand, pos,
                           Position::TremoloPicking);
    myTrillCommand->setEnabled(note != nullptr);
    myTrillCommand->setChecked(note && note->hasTrill());
    updatePositionProperty(myTapCommand, pos, Position::Tap);
    updatePositionProperty(myArpeggioUpCommand, pos, Position::ArpeggioUp);
    updatePositionProperty(myArpeggioDownCommand, pos, Position::ArpeggioDown);
    updatePositionProperty(myPickStrokeUpCommand, pos, Position::PickStrokeUp);
    updatePositionProperty(myPickStrokeDownCommand, pos,
                           Position::PickStrokeDown);

    myPlayerChangeCommand->setChecked(
        ScoreUtils::findByPosition(system.getPlayerChanges(), position) !=
        nullptr);
}

void PowerTabEditor::enableEditing(bool enable)
{
    QList<QMenu *> menuList;
    menuList << myPositionMenu << myPositionSectionMenu << myPositionStaffMenu
             << myTextMenu << mySectionMenu << myLineSpacingMenu << myNotesMenu
             << myOctaveMenu << myRestsMenu << myMusicSymbolsMenu
             << myTabSymbolsMenu << myPlaybackMenu << myEditMenu;

    for (QMenu *menu : menuList)
    {
        for (QAction *action : menu->actions())
            action->setEnabled(enable);
    }

    myCloseTabCommand->setEnabled(enable);
    mySaveCommand->setEnabled(enable);
    mySaveAsCommand->setEnabled(enable);
    myPrintCommand->setEnabled(enable);
    myPrintPreviewCommand->setEnabled(enable);
    myPlayFromStartOfMeasureCommand->setEnabled(enable);
    myAddPlayerCommand->setEnabled(enable);
    myAddInstrumentCommand->setEnabled(enable);
    myPlayerChangeCommand->setEnabled(enable);
    myEditViewFiltersCommand->setEnabled(enable);
    myNextTabCommand->setEnabled(enable);
    myPrevTabCommand->setEnabled(enable);

    // MIDI commands are always enabled if documents are open.
    if (myDocumentManager->hasOpenDocuments())
    {
        myPlayPauseCommand->setEnabled(true);
        myRewindCommand->setEnabled(true);
        myMetronomeCommand->setEnabled(true);
        myStopCommand->setEnabled(myIsPlaying);
    }

    // Prevent the user from changing tabs during playback.
    myTabWidget->tabBar()->setEnabled(enable);
}

void PowerTabEditor::editRest(Position::DurationType duration)
{
    ScoreLocation &location = getLocation();
    const Position *pos = location.getPosition();

    if (pos && pos->isRest())
    {
        if (pos->getDurationType() == duration)
        {
            // TODO - delete rest using the regular command for deleting positions.
        }
        else
        {
            myUndoManager->push(new EditNoteDuration(location, duration, true),
                                location.getSystemIndex());
        }
    }
    else
    {
        myUndoManager->push(new AddRest(location, duration),
                            location.getSystemIndex());

    }
}

void PowerTabEditor::rewindPlaybackToStart()
{
    const bool wasPlaying = myIsPlaying;

    if (wasPlaying)
        startStopPlayback();

    moveCaretToFirstSection();
    moveCaretToStart();

    if (wasPlaying)
        startStopPlayback();
}

void PowerTabEditor::stopPlayback()
{
    assert(myIsPlaying);
    const ScoreLocation start_location = myMidiPlayer->getStartLocation();

    startStopPlayback();
    getCaret().moveToLocation(start_location);
}

void PowerTabEditor::toggleMetronome()
{
    auto settings = mySettingsManager->getWriteHandle();
    settings->set(Settings::MetronomeEnabled, myMetronomeCommand->isChecked());
}

void PowerTabEditor::updateActiveVoice(int voice)
{
    getLocation().setVoiceIndex(voice);
    updateCommands();
}

void PowerTabEditor::updateActiveFilter(int filter)
{
    myDocumentManager->getCurrentDocument().getViewOptions().setFilter(filter);
    redrawScore();
}

void PowerTabEditor::updateZoom(double percent)
{
    myDocumentManager->getCurrentDocument().getViewOptions().setZoom(percent);
    getScoreArea()->refreshZoom();
}

void PowerTabEditor::updateLocationLabel()
{
    myPlaybackWidget->updateLocationLabel(
        boost::lexical_cast<std::string>(getCaret().getLocation()));
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

void PowerTabEditor::editTimeSignature(const ScoreLocation &timeLocation)
{
    ScoreLocation location(getLocation());
    location.setSystemIndex(timeLocation.getSystemIndex());
    location.setPositionIndex(timeLocation.getPositionIndex());

    const Barline *barline = location.getBarline();
    Q_ASSERT(barline);

    TimeSignatureDialog dialog(this, barline->getTimeSignature());
    if (dialog.exec() == QDialog::Accepted)
    {
        myUndoManager->push(new EditTimeSignature(location,
                                                  dialog.getTimeSignature()),
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

void PowerTabEditor::editClef(int system, int staff)
{
    ScoreLocation location = getLocation();
    location.setSystemIndex(system);
    location.setStaffIndex(staff);

    const Staff &currentStaff = location.getStaff();
    Staff::ClefType newClef = currentStaff.getClefType() == Staff::TrebleClef
                                  ? Staff::BassClef
                                  : Staff::TrebleClef;

    myUndoManager->push(
        new EditStaff(location, newClef, currentStaff.getStringCount()),
        location.getSystemIndex());
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
    for (const Position *pos : selectedPositions)
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
    for (const Note *note : selectedNotes)
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

void PowerTabEditor::insertStaff(int index)
{
    StaffDialog dialog(this);

    const Staff &current_staff = getLocation().getStaff();
    dialog.setStringCount(current_staff.getStringCount());

    if (dialog.exec() == QDialog::Accepted)
    {
        Staff staff;
        staff.setStringCount(dialog.getStringCount());
        staff.setClefType(dialog.getClefType());

        myUndoManager->push(new AddStaff(getLocation(), staff, index),
                            getLocation().getSystemIndex());
    }
}

void PowerTabEditor::editStaff(int system, int staff)
{
    ScoreLocation location = getLocation();
    location.setSystemIndex(system);
    location.setStaffIndex(staff);

    StaffDialog dialog(this);

    const Staff &currentStaff = location.getStaff();
    dialog.setStringCount(currentStaff.getStringCount());
    dialog.setClefType(currentStaff.getClefType());

    if (dialog.exec() == QDialog::Accepted)
    {
        myUndoManager->push(new EditStaff(location, dialog.getClefType(),
                                          dialog.getStringCount()),
                            UndoManager::AFFECTS_ALL_SYSTEMS);
    }
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

#endif
