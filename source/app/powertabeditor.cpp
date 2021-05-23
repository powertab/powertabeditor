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
#include <actions/addpositionproperty.h>
#include <actions/addrest.h>
#include <actions/addspecialnoteproperty.h>
#include <actions/addstaff.h>
#include <actions/addsystem.h>
#include <actions/adjustlinespacing.h>
#include <actions/editbarline.h>
#include <actions/editdynamic.h>
#include <actions/editfileinformation.h>
#include <actions/editinstrument.h>
#include <actions/editkeysignature.h>
#include <actions/editnoteduration.h>
#include <actions/editplayer.h>
#include <actions/editplayerchange.h>
#include <actions/editrehearsalsign.h>
#include <actions/editstaff.h>
#include <actions/edittabnumber.h>
#include <actions/edittempomarker.h>
#include <actions/edittextitem.h>
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
#include <actions/removeposition.h>
#include <actions/removepositionproperty.h>
#include <actions/removespecialnoteproperty.h>
#include <actions/removestaff.h>
#include <actions/removesystem.h>
#include <actions/shiftpositions.h>
#include <actions/shiftstring.h>
#include <actions/undomanager.h>
#include <actions/volumeswell.h>

#include <app/appinfo.h>
#include <app/caret.h>
#include <app/clipboard.h>
#include <app/command.h>
#include <app/documentmanager.h>
#include <app/paths.h>
#include <app/recentfiles.h>
#include <app/scorearea.h>
#include <app/settings.h>
#include <app/settingsmanager.h>
#include <app/tuningdictionary.h>

#include <audio/midiplayer.h>
#include <audio/settings.h>

#include <boost/range/algorithm/transform.hpp>
#include <chrono>

#include <dialogs/alterationofpacedialog.h>
#include <dialogs/alternateendingdialog.h>
#include <dialogs/artificialharmonicdialog.h>
#include <dialogs/barlinedialog.h>
#include <dialogs/benddialog.h>
#include <dialogs/bulkconverterdialog.h>
#include <dialogs/chordnamedialog.h>
#include <dialogs/directiondialog.h>
#include <dialogs/dynamicdialog.h>
#include <dialogs/fileinformationdialog.h>
#include <dialogs/gotobarlinedialog.h>
#include <dialogs/gotorehearsalsigndialog.h>
#include <dialogs/infodialog.h>
#include <dialogs/irregulargroupingdialog.h>
#include <dialogs/keyboardsettingsdialog.h>
#include <dialogs/keysignaturedialog.h>
#include <dialogs/lefthandfingeringdialog.h>
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
#include <dialogs/volumeswelldialog.h>

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

#include <score/dynamic.h>
#include <score/utils.h>
#include <score/voiceutils.h>

#include <util/tostring.h>
#include <util/version.h>

#include <widgets/instruments/instrumentpanel.h>
#include <widgets/mixer/mixer.h>
#include <widgets/playback/playbackwidget.h>
#include <widgets/toolbox/toolbox.h>

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
      myToolBox(nullptr),
      myToolBoxDockWidget(new QDockWidget(tr("Toolbox"), this)),
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

    connect(myUndoManager.get(), &UndoManager::redrawNeeded, this,
            &PowerTabEditor::redrawSystem);
    connect(myUndoManager.get(), &UndoManager::fullRedrawNeeded, this,
            &PowerTabEditor::redrawScore);
    connect(myUndoManager.get(), &UndoManager::cleanChanged, this,
            &PowerTabEditor::updateModified);

    myTuningDictionary->loadInBackground();
    mySettingsManager->load(Paths::getConfigDir());

    createMixer();
    createInstrumentPanel();
    createCommands();
    loadKeyboardShortcuts();
    createMenus();
    createToolBox();

    // Set up the recent files menu.
    myRecentFiles =
        new RecentFiles(*mySettingsManager, myRecentFilesMenu, this);
    connect(myRecentFiles, &RecentFiles::fileSelected, this,
            &PowerTabEditor::openFile);

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

void PowerTabEditor::openFilesInteractive()
{
    auto files = QFileDialog::getOpenFileNames(
        this, tr("Open"), myPreviousDirectory,
        QString::fromStdString(myFileFormatManager->importFileFilter()));

    openFiles(files);
}

void PowerTabEditor::openFile(QString filename)
{
    assert(!filename.isEmpty());

    auto path = Paths::fromQString(filename);

    int validationResult = myDocumentManager->findDocument(path);
    if (validationResult > -1)
    {
        qDebug() << "File: " << filename << " is already open";
        myTabWidget->setCurrentIndex(validationResult);
        return;
    }

    auto start = std::chrono::high_resolution_clock::now();

    qDebug() << "Opening file: " << filename;

    QFileInfo fileInfo(filename);
    std::optional<FileFormat> format = myFileFormatManager->findFormat(
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
        myFileFormatManager->importFile(doc.getScore(), path, *format);
        auto end = std::chrono::high_resolution_clock::now();
        qDebug() << "File loaded in"
                 << std::chrono::duration_cast<std::chrono::milliseconds>(end - start) .count()
                 << "ms";

        doc.setFilename(path);
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
    if (!myUndoManager->stacks()[index]->isClean())
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
            if (!saveFile(index))
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

bool PowerTabEditor::saveFile(int doc_index)
{
    Document &doc = myDocumentManager->getDocument(doc_index);
    if (!doc.hasFilename())
        return saveFileAs(doc_index);

    const QString filename = Paths::toQString(doc.getFilename());
    return QFileInfo(filename).suffix() == "pt2" ? saveFile(doc_index, filename)
                                                 : saveFileAs(doc_index);
}

bool PowerTabEditor::saveFile(int doc_index, QString path)
{
    QFileInfo info(path);
    QString extension = info.suffix();
    Q_ASSERT(!extension.isEmpty());

    std::optional<FileFormat> format =
        myFileFormatManager->findFormat(extension.toStdString());
    if (!format)
    {
        QMessageBox::warning(this, tr("Error Saving File"),
                             tr("Unsupported file type."));
        return false;
    }

    auto path_str = Paths::fromQString(path);
    Document &doc = myDocumentManager->getDocument(doc_index);

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
        myTabWidget->setTabText(doc_index, filename);
        myTabWidget->setTabToolTip(doc_index, filename);

        // Add to the recent files list and update the last used directory.
        myRecentFiles->add(path);
        setPreviousDirectory(path);

        // Mark the file as being in an unmodified state.
        myUndoManager->stacks()[doc_index]->setClean();
    }

    return true;
}

bool PowerTabEditor::saveFileAs(int doc_index)
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

        return saveFile(doc_index, path);
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

void PowerTabEditor::bulkConverter()
{
    BulkConverterDialog dialog(this, myFileFormatManager);
    dialog.exec();
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

        connect(myMidiPlayer.get(), &MidiPlayer::playbackSystemChanged, this,
                &PowerTabEditor::moveCaretToSystem);
        connect(myMidiPlayer.get(), &MidiPlayer::playbackPositionChanged, this,
                &PowerTabEditor::moveCaretToPosition);
        connect(myMidiPlayer.get(), &MidiPlayer::finished, this,
                [this]() { startStopPlayback(); });
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

void PowerTabEditor::removeSelectedItem()
{
    switch (getCaret().getSelectedItem())
    {
        case ScoreItem::Clef:
        case ScoreItem::KeySignature:
        case ScoreItem::ScoreInfo:
        case ScoreItem::TimeSignature:
            // Do nothing.
            break;

        case ScoreItem::Barline:
            removeSelectedPositions();
            break;

        case ScoreItem::TempoMarker:
            editTempoMarker(/* remove */ true);
            break;

        case ScoreItem::AlterationOfPace:
            editAlterationOfPace(/* remove */ true);
            break;

        case ScoreItem::RehearsalSign:
            editRehearsalSign(/* remove */ true);
            break;

        case ScoreItem::TextItem:
            editTextItem(/* remove */ true);
            break;

        case ScoreItem::PlayerChange:
            editPlayerChange(/* remove */ true);
            break;

        case ScoreItem::VolumeSwell:
            editVolumeSwell(/* remove */ true);
            break;

        case ScoreItem::AlternateEnding:
            editRepeatEnding(/* remove */ true);
            break;

        case ScoreItem::Direction:
            editMusicalDirection(/* remove */ true);
            break;

        case ScoreItem::Dynamic:
            editDynamic(/* remove */ true);
            break;

        case ScoreItem::MultiBarRest:
            editMultiBarRest(/* remove */ true);
            break;

        case ScoreItem::ChordText:
            editChordName(/* remove */ true);
            break;

        case ScoreItem::Bend:
            editBend(/* remove */ true);
            break;

        case ScoreItem::Staff:
        {
            auto location = getLocation();
            if (!location.getNote())
                removeSelectedPositions();
            else
            {
                myUndoManager->push(new RemoveNote(location),
                                    location.getSystemIndex());
            }
        }
        break;
    }
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
                   [](const Position *p) { return p->getPosition(); });

    // Remove each of the selected positions.
    for (int position : positions)
    {
        location.setPositionIndex(position);
        myUndoManager->push(new RemovePosition(location),
                            location.getSystemIndex());
    }

    std::vector<int> barPositions;
    std::transform(bars.begin(), bars.end(), std::back_inserter(barPositions),
                   [](const Barline *b) { return b->getPosition(); });

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
    Score &score = getLocation().getScore();
    GoToBarlineDialog dialog(this, score);

    if (dialog.exec() == QDialog::Accepted)
    {
        auto &&new_location = dialog.getLocation();
        getCaret().moveToSystem(new_location.getSystemIndex(), true);
        getCaret().moveToPosition(new_location.getPositionIndex());
    }
}

void PowerTabEditor::gotoRehearsalSign()
{
    GoToRehearsalSignDialog dialog(this, getLocation().getScore());

    if (dialog.exec() == QDialog::Accepted)
    {
        auto &&new_location = dialog.getLocation();
        getCaret().moveToSystem(new_location.getSystemIndex(), true);
        getCaret().moveToPosition(new_location.getPositionIndex());
    }
}

void
PowerTabEditor::editChordName(bool remove)
{
    ScoreLocation &location(getLocation());
    const ChordText *text = ScoreUtils::findByPosition(
        location.getSystem().getChords(), location.getPositionIndex());

    if (remove)
    {
        Q_ASSERT(text);
        myUndoManager->push(new RemoveChordText(location),
                            location.getSystemIndex());
        return;
    }

    ChordNameDialog dialog(this, text ? text->getChordName() : ChordName());
    if (dialog.exec() == QDialog::Accepted)
    {
        ChordText new_text(location.getPositionIndex(), dialog.getChordName());

        if (text)
        {
            myUndoManager->beginMacro(tr("Edit Chord Text"));
            myUndoManager->push(new RemoveChordText(location),
                                location.getSystemIndex());
        }

        myUndoManager->push(new AddChordText(location, new_text),
                            location.getSystemIndex());

        if (text)
            myUndoManager->endMacro();
    }
    else
        myChordNameCommand->setChecked(text != nullptr);
}

void
PowerTabEditor::editTextItem(bool remove)
{
    const ScoreLocation &location = getLocation();
    const TextItem *item = ScoreUtils::findByPosition(
        location.getSystem().getTextItems(), location.getPositionIndex());

    if (remove)
    {
        Q_ASSERT(item);
        myUndoManager->push(new RemoveTextItem(location),
                            location.getSystemIndex());
        return;
    }

    TextItemDialog dialog(this, item);
    if (dialog.exec() == QDialog::Accepted)
    {
        TextItem new_item(location.getPositionIndex(), dialog.getContents());

        if (item)
        {
            myUndoManager->beginMacro(tr("Edit Text Item"));
            myUndoManager->push(new RemoveTextItem(location),
                                location.getSystemIndex());
        }

        myUndoManager->push(new AddTextItem(location, new_item),
                            location.getSystemIndex());

        if (item)
            myUndoManager->endMacro();
    }
    else
        myTextCommand->setChecked(item != nullptr);
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
    const Voice *prev_voice = VoiceUtils::getAdjacentVoice(location, -1);

    // If at an empty position, try to insert a new note that's tied to the
    // previous note.
    if (!location.hasSelection() && !location.getNote())
    {
        const int string = location.getString();
        const Note *prevNote = VoiceUtils::getPreviousNote(
            voice, location.getPositionIndex(), string, prev_voice);
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
                                        *note, prev_voice))
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
                                                note, prev_voice))
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

void PowerTabEditor::editMultiBarRest(bool remove)
{
    const ScoreLocation &location = getLocation();
    const Position *position = location.getPosition();

    if (remove)
    {
        Q_ASSERT(position);
        Q_ASSERT(position->hasMultiBarRest());
        myUndoManager->push(
            new RemovePosition(location, tr("Remove Multi-Bar Rest")),
            location.getSystemIndex());
        return;
    }

    // Verify that the bar is empty if adding a new rest.
    if (!position)
    {
        const System &system = location.getSystem();
        const Barline *prevBar =
            system.getPreviousBarline(location.getPositionIndex());
        if (!prevBar)
            prevBar = &system.getBarlines().front();
        const Barline *nextBar =
            system.getNextBarline(location.getPositionIndex());

        if (ScoreUtils::findInRange(location.getVoice().getPositions(),
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
    else
    {
        Q_ASSERT(position->hasMultiBarRest());
    }

    MultiBarRestDialog dialog(this,
                              position ? position->getMultiBarRestCount() : 2);
    if (dialog.exec() == QDialog::Accepted)
    {
        if (position)
        {
            myUndoManager->beginMacro(tr("Edit Multi-Bar Rest"));
            myUndoManager->push(new RemovePosition(location),
                                location.getSystemIndex());
        }

        myUndoManager->push(new AddMultiBarRest(location, dialog.getBarCount()),
                            location.getSystemIndex());

        if (position)
            myUndoManager->endMacro();
    }
    else
    {
        myMultibarRestCommand->setChecked(position &&
                                          position->hasMultiBarRest());
    }
}

void
PowerTabEditor::editRehearsalSign(bool remove)
{
    const ScoreLocation &location = getLocation();
    const Barline *barline = location.getBarline();
    Q_ASSERT(barline);

    if (remove)
    {
        Q_ASSERT(barline->hasRehearsalSign());
        myUndoManager->push(new RemoveRehearsalSign(location),
                            UndoManager::AFFECTS_ALL_SYSTEMS);
        return;
    }

    auto sign =
        barline->hasRehearsalSign() ? &barline->getRehearsalSign() : nullptr;
    RehearsalSignDialog dialog(this, sign ? sign->getDescription() : "");
    if (dialog.exec() == QDialog::Accepted)
    {
        if (sign)
        {
            myUndoManager->push(
                new EditRehearsalSign(location, dialog.getDescription()),
                location.getSystemIndex());
        }
        else
        {
            myUndoManager->push(
                new AddRehearsalSign(location, dialog.getDescription()),
                UndoManager::AFFECTS_ALL_SYSTEMS);
        }
    }
    else
        myRehearsalSignCommand->setChecked(sign != nullptr);
}

void PowerTabEditor::editTempoMarker(bool remove)
{
    const ScoreLocation &location = getLocation();
    const TempoMarker *marker = ScoreUtils::findByPosition(
        location.getSystem().getTempoMarkers(), location.getPositionIndex());

    if (remove)
    {
        Q_ASSERT(marker);
        myUndoManager->push(new RemoveTempoMarker(location),
                            location.getSystemIndex());
        return;
    }

    TempoMarkerDialog dialog(this, marker);
    if (dialog.exec() == QDialog::Accepted)
    {
        TempoMarker new_marker(dialog.getTempoMarker());
        new_marker.setPosition(location.getPositionIndex());

        if (marker)
        {
            myUndoManager->beginMacro(tr("Edit Tempo Marker"));
            myUndoManager->push(new RemoveTempoMarker(location),
                                location.getSystemIndex());
        }

        myUndoManager->push(new AddTempoMarker(location, new_marker),
                            location.getSystemIndex());

        if (marker)
            myUndoManager->endMacro();
    }
    else
        myTempoMarkerCommand->setChecked(marker != nullptr);
}

void PowerTabEditor::editAlterationOfPace(bool remove)
{
    const ScoreLocation &location = getLocation();
    const TempoMarker *marker = ScoreUtils::findByPosition(
        location.getSystem().getTempoMarkers(), location.getPositionIndex());

    if (remove)
    {
        Q_ASSERT(marker);
        Q_ASSERT(marker->getMarkerType() == TempoMarker::AlterationOfPace);
        myUndoManager->push(new RemoveTempoMarker(location),
                            location.getSystemIndex());
        return;
    }

    AlterationOfPaceDialog dialog(this, marker);
    if (dialog.exec() == QDialog::Accepted)
    {
        TempoMarker new_marker(location.getPositionIndex());
        new_marker.setMarkerType(TempoMarker::AlterationOfPace);
        new_marker.setAlterationOfPace(dialog.getAlterationOfPaceType());

        if (marker)
        {
            myUndoManager->beginMacro(tr("Edit Alteration of Pace"));
            myUndoManager->push(new RemoveTempoMarker(location),
                                location.getSystemIndex());
        }

        myUndoManager->push(new AddTempoMarker(location, new_marker),
                            location.getSystemIndex());

        if (marker)
            myUndoManager->endMacro();
    }
    else
        myAlterationOfPaceCommand->setChecked(marker != nullptr);
}

void PowerTabEditor::insertStandardBarline()
{
    ScoreLocation &location = getLocation();
    myUndoManager->push(new AddBarline(location,
                                       Barline(location.getPositionIndex(),
                                               Barline::SingleBar)),
                        location.getSystemIndex());
}

void
PowerTabEditor::editMusicalDirection(bool remove)
{
    const ScoreLocation &location = getLocation();
    const Direction *direction = ScoreUtils::findByPosition(
        location.getSystem().getDirections(), location.getPositionIndex());

    if (remove)
    {
        Q_ASSERT(direction);
        myUndoManager->push(new RemoveDirection(location),
                            location.getSystemIndex());
        return;
    }

    DirectionDialog dialog(this, direction);
    if (dialog.exec() == QDialog::Accepted)
    {
        Direction new_direction(dialog.getDirection());
        new_direction.setPosition(location.getPositionIndex());

        if (direction)
        {
            myUndoManager->beginMacro(tr("Edit Musical Direction"));
            myUndoManager->push(new RemoveDirection(location),
                                location.getSystemIndex());
        }

        myUndoManager->push(new AddDirection(location, new_direction),
                            location.getSystemIndex());

        if (direction)
            myUndoManager->endMacro();
    }
    else
        myDirectionCommand->setChecked(direction != nullptr);
}

void PowerTabEditor::editRepeatEnding(bool remove)
{
    const ScoreLocation &location = getLocation();
    const AlternateEnding *current_ending =
        ScoreUtils::findByPosition(location.getSystem().getAlternateEndings(),
                                   location.getPositionIndex());

    if (remove)
    {
        Q_ASSERT(current_ending);
        myUndoManager->push(new RemoveAlternateEnding(location),
                            location.getSystemIndex());
        return;
    }

    AlternateEndingDialog dialog(this, current_ending);
    if (dialog.exec() == QDialog::Accepted)
    {
        AlternateEnding ending(dialog.getAlternateEnding());
        ending.setPosition(location.getPositionIndex());

        if (current_ending)
        {
            myUndoManager->beginMacro(tr("Edit Repeat Ending"));
            myUndoManager->push(new RemoveAlternateEnding(location),
                                location.getSystemIndex());
        }

        myUndoManager->push(new AddAlternateEnding(location, ending),
                            location.getSystemIndex());

        if (current_ending)
            myUndoManager->endMacro();
    }
    else
        myRepeatEndingCommand->setChecked(current_ending != nullptr);
}

void PowerTabEditor::updateDynamic(VolumeLevel volume)
{
    ScoreLocation &location = getLocation();
    const Dynamic *currentDynamic = ScoreUtils::findByPosition(
                location.getStaff().getDynamics(), location.getPositionIndex());
    Dynamic newDynamic(location.getPositionIndex(), volume);

    if (currentDynamic)
    {
        if (newDynamic.getVolume() == currentDynamic->getVolume())
        {
            myUndoManager->push(new RemoveDynamic(location),
                                location.getSystemIndex());
            QAction *checkedAction = myDynamicGroup->checkedAction();
            if (checkedAction)
              checkedAction->setChecked(false);
        }
        else
        {
            myUndoManager->push(
                new EditDynamic(location, *currentDynamic, newDynamic),
                location.getSystemIndex());
        }
    }
    else
        myUndoManager->push(new AddDynamic(location, newDynamic),
                            location.getSystemIndex());
}

void
PowerTabEditor::editDynamic(bool remove)
{
    ScoreLocation &location = getLocation();
    const Dynamic *dynamic = ScoreUtils::findByPosition(
        location.getStaff().getDynamics(), location.getPositionIndex());

    if (remove)
    {
        Q_ASSERT(dynamic);
        myUndoManager->push(new RemoveDynamic(location),
                            location.getSystemIndex());
        return;
    }

    DynamicDialog dialog(this, dynamic);
    if (dialog.exec() == QDialog::Accepted)
    {
        Dynamic new_dynamic(location.getPositionIndex(),
                            dialog.getVolumeLevel());

        if (dynamic)
        {
            myUndoManager->push(
                new EditDynamic(location, *dynamic, new_dynamic),
                location.getSystemIndex());
        }
        else
        {
            myUndoManager->push(new AddDynamic(location, new_dynamic),
                                location.getSystemIndex());
        }
    }
    else
        myDynamicCommand->setChecked(dynamic != nullptr);
}

void
PowerTabEditor::editVolumeSwell(bool remove)
{
    const ScoreLocation &location = getLocation();
    const Position *position = location.getPosition();
    assert(position);

    if (remove)
    {
        Q_ASSERT(position->hasVolumeSwell());
        myUndoManager->push(new RemoveVolumeSwell(location),
                            location.getSystemIndex());
        return;
    }

    VolumeSwellDialog dialog(this, position->hasVolumeSwell()
                                       ? &position->getVolumeSwell()
                                       : nullptr);
    if (dialog.exec() == QDialog::Accepted)
    {
        const bool editing = position->hasVolumeSwell();
        if (editing)
        {
            myUndoManager->beginMacro(tr("Edit Volume Swell"));
            myUndoManager->push(new RemoveVolumeSwell(location),
                                location.getSystemIndex());
        }
        myUndoManager->push(
            new AddVolumeSwell(location, dialog.getVolumeSwell()),
            location.getSystemIndex());

        if (editing)
            myUndoManager->endMacro();
    }
    else
        myVolumeSwellCommand->setChecked(position->hasVolumeSwell());
}

void PowerTabEditor::editHammerPull()
{
    ScoreLocation &location = getLocation();
    const Voice &voice = location.getVoice();
    const int position = location.getPositionIndex();
    const Note *note = location.getNote();
    if (!note)
        return;

    const Voice *next_voice = VoiceUtils::getAdjacentVoice(location, 1);

    // TODO - support editing groups of notes.
    if (VoiceUtils::canHammerOnOrPullOff(voice, position, *note, next_voice))
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

void
PowerTabEditor::editBend(bool remove)
{
    const ScoreLocation &location = getLocation();
    const Note *note = location.getNote();
    Q_ASSERT(note);

    if (remove)
    {
        Q_ASSERT(note->hasBend());
        myUndoManager->push(new RemoveBend(location),
                            location.getSystemIndex());
        return;
    }

    BendDialog dialog(this, note->hasBend() ? &note->getBend() : nullptr);
    if (dialog.exec() == QDialog::Accepted)
    {
        const bool is_edit = note->hasBend();
        if (is_edit)
        {
            myUndoManager->beginMacro(tr("Edit Bend"));
            myUndoManager->push(new RemoveBend(location),
                                location.getSystemIndex());
        }

        myUndoManager->push(new AddBend(location, dialog.getBend()),
                            location.getSystemIndex());

        if (is_edit)
            myUndoManager->endMacro();
    }
    else
        myBendCommand->setChecked(note->hasBend());
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

void PowerTabEditor::editLeftHandFingering()
{
    const ScoreLocation &location = getLocation();
    const Note *note = location.getNote();
    Q_ASSERT(note);

    if (note->hasLeftHandFingering())
    {
        myUndoManager->push(new RemoveLeftHandFingering(location),
                            location.getSystemIndex());
    }
    else
    {
        LeftHandFingeringDialog dialog(this);
        if (dialog.exec() == QDialog::Accepted)
        {
            myUndoManager->push(new AddLeftHandFingering(location,
                                dialog.getLeftHandFingering()),
                                location.getSystemIndex());
        }
        else
            myLeftHandFingeringCommand->setChecked(false);
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

void PowerTabEditor::editPlayerChange(bool remove)
{
    const ScoreLocation &location = getLocation();
    const PlayerChange *existing_change = ScoreUtils::findByPosition(
        location.getSystem().getPlayerChanges(), location.getPositionIndex());

    // Note that adding/removing a player change affects multiple systems,
    // since the standard notation will need to be updated if the new player
    // has a different tuning.
    if (remove)
    {
        Q_ASSERT(existing_change);
        myUndoManager->push(new RemovePlayerChange(location),
                            UndoManager::AFFECTS_ALL_SYSTEMS);
        return;
    }

    // Initialize the dialog with the current staves for each player (e.g. from
    // a previous player change if we're not editing an existing one at this
    // position).
    const PlayerChange *active_players = ScoreUtils::getCurrentPlayers(
        location.getScore(), location.getSystemIndex(),
        location.getPositionIndex());

    PlayerChangeDialog dialog(this, location.getScore(), location.getSystem(),
                              active_players);
    if (dialog.exec() == QDialog::Accepted)
    {
        if (existing_change)
        {
            myUndoManager->beginMacro(tr("Edit Player Change"));
            myUndoManager->push(new RemovePlayerChange(location),
                                UndoManager::AFFECTS_ALL_SYSTEMS);
        }

        myUndoManager->push(
            new AddPlayerChange(location, dialog.getPlayerChange()),
            UndoManager::AFFECTS_ALL_SYSTEMS);

        if (existing_change)
            myUndoManager->endMacro();
    }
    else
        myPlayerChangeCommand->setChecked(existing_change != nullptr);
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

void PowerTabEditor::info(void)
{
    InfoDialog(this).exec();
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

    name += QString::fromStdString(Version::get());

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
            const QString path = Paths::toQString(doc.getFilename());
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
    connect(myNewDocumentCommand, &QAction::triggered, this,
            &PowerTabEditor::createNewDocument);

    myOpenFileCommand = new Command(tr("&Open..."), "File.Open",
                                    QKeySequence::Open, this);
    connect(myOpenFileCommand, &QAction::triggered,
            [this]() { openFilesInteractive(); });

    myCloseTabCommand = new Command(tr("&Close Tab"), "File.CloseTab",
                                    Qt::CTRL + Qt::Key_W, this);
    connect(myCloseTabCommand, &QAction::triggered, this,
            &PowerTabEditor::closeCurrentTab);

    mySaveCommand = new Command(tr("Save"), "File.Save",
                                QKeySequence::Save, this);
    connect(mySaveCommand, &QAction::triggered, this, [this]() {
        saveFile(myDocumentManager->getCurrentDocumentIndex());
    });

    mySaveAsCommand = new Command(tr("Save As..."), "File.SaveAs",
                                  QKeySequence::SaveAs, this);
    connect(mySaveAsCommand, &QAction::triggered, this,
            &PowerTabEditor::saveFileAs);

    myPrintCommand = new Command(tr("Print..."), "File.Print",
                                 QKeySequence::Print, this);
    connect(myPrintCommand, &QAction::triggered, this,
            &PowerTabEditor::printDocument);

    myPrintPreviewCommand = new Command(
        tr("Print Preview..."), "File.PrintPreview", QKeySequence(), this);
    connect(myPrintPreviewCommand, &QAction::triggered, this,
            &PowerTabEditor::printPreview);

    myBulkConverterCommand = new Command(tr("Bulk Converter..."),
                                        "File.BulkConverter",
                                        QKeySequence(), this);
    connect(myBulkConverterCommand, &QAction::triggered, this,
            &PowerTabEditor::bulkConverter);

    myEditShortcutsCommand = new Command(tr("Customize Shortcuts..."),
                                         "File.CustomizeShortcuts",
                                         QKeySequence(), this);
    connect(myEditShortcutsCommand, &QAction::triggered, this,
            &PowerTabEditor::editKeyboardShortcuts);

    myEditPreferencesCommand = new Command(tr("&Preferences..."),
                                           "File.Preferences",
                                           QKeySequence::Preferences, this);
    connect(myEditPreferencesCommand, &QAction::triggered, this,
            &PowerTabEditor::editPreferences);

    myExitCommand = new Command(tr("&Quit"), "File.Quit", QKeySequence::Quit,
                                this);
    connect(myExitCommand, &QAction::triggered, this, &PowerTabEditor::close);

    // Undo / Redo actions.
    myUndoAction = myUndoManager->createUndoAction(this, tr("&Undo"));
    myUndoAction->setShortcuts(QKeySequence::Undo);

    myRedoAction = myUndoManager->createRedoAction(this, tr("&Redo"));
    myRedoAction->setShortcuts(QKeySequence::Redo);

    // Copy/Paste actions.
    myCutCommand = new Command(tr("Cut"), "Edit.Cut", QKeySequence::Cut, this);
    connect(myCutCommand, &QAction::triggered, this,
            &PowerTabEditor::cutSelectedNotes);

    myCopyCommand = new Command(tr("Copy"), "Edit.Copy", QKeySequence::Copy,
                                this);
    connect(myCopyCommand, &QAction::triggered, this,
            &PowerTabEditor::copySelectedNotes);

    myPasteCommand = new Command(tr("Paste"), "Edit.Paste",
                                 QKeySequence::Paste, this);
    connect(myPasteCommand, &QAction::triggered, this, &PowerTabEditor::pasteNotes);

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
    connect(myFileInfoCommand, &QAction::triggered, this,
            &PowerTabEditor::editFileInformation);

    // Playback-related actions.
    myPlayPauseCommand = new Command(tr("Play"), "Playback.PlayPause",
                                     Qt::Key_Space, this);
    connect(myPlayPauseCommand, &QAction::triggered, this,
            &PowerTabEditor::startStopPlayback);

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
    connect(myFirstSectionCommand, &QAction::triggered, this,
            &PowerTabEditor::moveCaretToFirstSection);

    myNextSectionCommand =
        new Command(tr("Next Section"), "Position.Section.NextSection",
                    QKeySequence::MoveToNextPage, this);
    connect(myNextSectionCommand, &QAction::triggered, this,
            &PowerTabEditor::moveCaretToNextSection);

    myPrevSectionCommand =
        new Command(tr("Previous Section"), "Position.Section.PreviousSection",
                    QKeySequence::MoveToPreviousPage, this);
    connect(myPrevSectionCommand, &QAction::triggered, this,
            &PowerTabEditor::moveCaretToPrevSection);

    myLastSectionCommand =
        new Command(tr("Last Section"), "Position.Section.LastSection",
                    QKeySequence::MoveToEndOfDocument, this);
    connect(myLastSectionCommand, &QAction::triggered, this,
            &PowerTabEditor::moveCaretToLastSection);

    myInsertSpaceCommand =
        new Command(tr("Insert Space"), "Position.InsertSpace",
                    QKeySequence(Qt::Key_Insert), this);
    connect(myInsertSpaceCommand, &QAction::triggered, this,
            &PowerTabEditor::shiftForward);

    myRemoveSpaceCommand =
        new Command(tr("Remove Space"), "Position.RemoveSpace",
                    QKeySequence(Qt::SHIFT + Qt::Key_Insert), this);
    connect(myRemoveSpaceCommand, &QAction::triggered, this,
            &PowerTabEditor::shiftBackward);

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
    connect(myStartPositionCommand, &QAction::triggered, this,
            &PowerTabEditor::moveCaretToStart);

    myNextPositionCommand =
        new Command(tr("&Next Position"), "Position.Staff.NextPosition",
                    QKeySequence::MoveToNextChar, this);
    connect(myNextPositionCommand, &QAction::triggered, this,
            &PowerTabEditor::moveCaretRight);

    myPrevPositionCommand =
        new Command(tr("&Previous Position"), "Position.Staff.PreviousPosition",
                    QKeySequence::MoveToPreviousChar, this);
    connect(myPrevPositionCommand, &QAction::triggered, this,
            &PowerTabEditor::moveCaretLeft);

    myNextStringCommand =
        new Command(tr("Next String"), "Position.Staff.NextString",
                    QKeySequence::MoveToNextLine, this);
    connect(myNextStringCommand, &QAction::triggered, this,
            &PowerTabEditor::moveCaretDown);

    myPrevStringCommand =
        new Command(tr("Previous String"), "Position.Staff.PreviousString",
                    QKeySequence::MoveToPreviousLine, this);
    connect(myPrevStringCommand, &QAction::triggered, this,
            &PowerTabEditor::moveCaretUp);

    myLastPositionCommand =
        new Command(tr("Move to &End"), "Position.Staff.MoveToEnd",
                    move_end_seq, this);
    connect(myLastPositionCommand, &QAction::triggered, this,
            &PowerTabEditor::moveCaretToEnd);

    myNextStaffCommand =
        new Command(tr("Next Staff"), "Position.Staff.NextStaff",
                    Qt::ALT + Qt::Key_Down, this);
    connect(myNextStaffCommand, &QAction::triggered, this,
            &PowerTabEditor::moveCaretToNextStaff);

    myPrevStaffCommand =
        new Command(tr("Previous Staff"), "Position.Staff.PreviousStaff",
                    Qt::ALT + Qt::Key_Up, this);
    connect(myPrevStaffCommand, &QAction::triggered, this,
            &PowerTabEditor::moveCaretToPrevStaff);

    myNextBarCommand = new Command(tr("Next Bar"), "Position.Staff.NextBar",
                                   Qt::Key_Tab, this);
    connect(myNextBarCommand, &QAction::triggered, this,
            &PowerTabEditor::moveCaretToNextBar);

    myPrevBarCommand =
        new Command(tr("Previous Bar"), "Position.Staff.PreviousBar",
                    Qt::SHIFT + Qt::Key_Tab, this);
    connect(myPrevBarCommand, &QAction::triggered, this,
            &PowerTabEditor::moveCaretToPrevBar);

#ifdef Q_OS_MAC
    // On macOS we need to use the "backspace" key so that you can just press
    // Delete rather than Fn+Delete.
    QKeySequence delete_seq = Qt::Key_Backspace;
    QKeySequence ctrl_delete_seq = Qt::CTRL + Qt::Key_Backspace;
#else
    QKeySequence delete_seq = QKeySequence::Delete;
    QKeySequence ctrl_delete_seq = QKeySequence::DeleteEndOfWord;
#endif

    myRemoveItemCommand =
        new Command(tr("Remove Selected Item"), "Position.RemoveSelectedItem",
                    delete_seq, this);
    connect(myRemoveItemCommand, &QAction::triggered, this,
            &PowerTabEditor::removeSelectedItem);

    myRemovePositionCommand =
        new Command(tr("Remove Position"), "Position.RemovePosition",
                    ctrl_delete_seq, this);
    connect(myRemovePositionCommand, &QAction::triggered, this,
            &PowerTabEditor::removeSelectedPositions);

    myGoToBarlineCommand = new Command(tr("Go To Barline..."),
                                       "Position.GoToBarline",
                                       Qt::CTRL + Qt::Key_G, this);
    connect(myGoToBarlineCommand, &QAction::triggered, this,
            &PowerTabEditor::gotoBarline);

    myGoToRehearsalSignCommand = new Command(tr("Go To Rehearsal Sign..."),
                                             "Position.GoToRehearsalSign",
                                             Qt::CTRL + Qt::Key_H, this);
    connect(myGoToRehearsalSignCommand, &QAction::triggered, this,
            &PowerTabEditor::gotoRehearsalSign);

    // Text-related actions.
    myChordNameCommand = new Command(tr("Chord Name..."), "Text.ChordName",
                                     Qt::Key_C, this);
    myChordNameCommand->setCheckable(true);
    connect(myChordNameCommand, &QAction::triggered, this,
            [=]() { editChordName(); });

    myTextCommand = new Command(tr("Text..."), "Text.TextItem",
                                QKeySequence(), this,
                                QStringLiteral(u":images/text.png"));
    myTextCommand->setCheckable(true);
    connect(myTextCommand, &QAction::triggered, this,
            [=]() { editTextItem(); });

    myInsertSystemAtEndCommand = new Command(tr("Insert System At End"),
                                             "Section.InsertSystemAtEnd",
                                             Qt::Key_N, this);
    connect(myInsertSystemAtEndCommand, &QAction::triggered, this,
            &PowerTabEditor::insertSystemAtEnd);

    myInsertSystemBeforeCommand = new Command(tr("Insert System Before"),
                                              "Section.InsertSystemBefore",
                                              QKeySequence(Qt::ALT + Qt::SHIFT +
                                                           Qt::Key_N),this);
    connect(myInsertSystemBeforeCommand, &QAction::triggered, this,
            &PowerTabEditor::insertSystemBefore);

    myInsertSystemAfterCommand = new Command(tr("Insert System After"),
                                             "Section.InsertSystemAfter",
                                             QKeySequence(Qt::SHIFT + Qt::Key_N),
                                             this);
    connect(myInsertSystemAfterCommand, &QAction::triggered, this,
            &PowerTabEditor::insertSystemAfter);

    myRemoveCurrentSystemCommand = new Command(tr("Remove Current System"),
                                         "Section.RemoveCurrentSystem",
                                         QKeySequence(Qt::CTRL + Qt::SHIFT +
                                                      Qt::Key_N), this);
    connect(myRemoveCurrentSystemCommand, &QAction::triggered, this,
            &PowerTabEditor::removeCurrentSystem);

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
                              Position::WholeNote,
                              QStringLiteral(u":images/whole_note"));
    createNoteDurationCommand(myHalfNoteCommand, tr("Half"), "Notes.HalfNote",
                              Position::HalfNote,
                              QStringLiteral(u":images/half_note"));
    createNoteDurationCommand(myQuarterNoteCommand, tr("Quarter"),
                              "Notes.QuarterNote", Position::QuarterNote,
                              QStringLiteral(u":images/quarter_note"));
    createNoteDurationCommand(myEighthNoteCommand, tr("8th"),
                              "Notes.EighthNote", Position::EighthNote,
                              QStringLiteral(u":images/8th_note"));
    createNoteDurationCommand(mySixteenthNoteCommand, tr("16th"),
                              "Notes.SixteenthNote", Position::SixteenthNote,
                              QStringLiteral(u":images/16th_note"));
    createNoteDurationCommand(myThirtySecondNoteCommand, tr("32nd"),
                              "Notes.ThirtySecondNote",
                              Position::ThirtySecondNote,
                              QStringLiteral(u":images/32nd_note"));
    createNoteDurationCommand(mySixtyFourthNoteCommand, tr("64th"),
                              "Notes.SixtyFourthNote",
                              Position::SixtyFourthNote,
                              QStringLiteral(u":images/64th_note"));

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
                                  QKeySequence(), Position::Dotted,
                                  QStringLiteral(u":images/dotted_note"));

    createPositionPropertyCommand(myDoubleDottedCommand, tr("Double Dotted"),
                                  "Notes.DoubleDotted", QKeySequence(),
                                  Position::DoubleDotted,
                                  QStringLiteral(u":images/doubledotted_note"));

    myAddDotCommand = new Command(tr("Add Dot"), "Notes.Dot.Add",
                            Qt::SHIFT + Qt::Key_Right, this);
    connect(myAddDotCommand, &QAction::triggered, this, &PowerTabEditor::addDot);

    myRemoveDotCommand = new Command(tr("Remove Dot"), "Notes.Dot.Remove",
                               Qt::SHIFT + Qt::Key_Left, this);
    connect(myRemoveDotCommand, &QAction::triggered, this, &PowerTabEditor::removeDot);

    myLeftHandFingeringCommand = new Command(tr("Left Hand Fingering..."),
                                             "Notes.LeftHandFingering",
                                             QKeySequence(), this,
                                             QStringLiteral(u":images/lefthandfingering.png"));
    myLeftHandFingeringCommand->setCheckable(true);
    connect(myLeftHandFingeringCommand, &QAction::triggered, this,
            &PowerTabEditor::editLeftHandFingering);

    myShiftStringUpCommand =
        new Command(tr("Shift String Up"), "Notes.ShiftStringUp",
                    Qt::CTRL + Qt::Key_Up, this);
    connect(myShiftStringUpCommand, &QAction::triggered, this,
            [=]() { shiftString(true); });

    myShiftStringDownCommand =
        new Command(tr("Shift String Down"), "Notes.ShiftStringDown",
                    Qt::CTRL + Qt::Key_Down, this);
    connect(myShiftStringDownCommand, &QAction::triggered, this,
            [=]() { shiftString(false); });

    myTieCommand = new Command(tr("Tied"), "Notes.Tied", Qt::Key_Y, this,
                               QStringLiteral(u":images/tie_note"));
    myTieCommand->setCheckable(true);
    connect(myTieCommand, &QAction::triggered, this, &PowerTabEditor::editTiedNote);

    createNotePropertyCommand(myMutedCommand, tr("Muted"), "Notes.Muted",
                              Qt::Key_X, Note::Muted,
                              QStringLiteral(u":images/muted.png"));
    createNotePropertyCommand(myGhostNoteCommand, tr("Ghost Note"),
                              "Notes.GhostNote", Qt::Key_G, Note::GhostNote,
                              QStringLiteral(u":images/ghost.png"));

    createPositionPropertyCommand(myFermataCommand, tr("Fermata"),
                                  "Notes.Fermata", Qt::Key_F, Position::Fermata,
                                  QStringLiteral(u":images/fermata"));

    createPositionPropertyCommand(myLetRingCommand, tr("Let Ring"),
                                  "Notes.LetRing", QKeySequence(),
                                  Position::LetRing, QStringLiteral(u":images/let_ring.png"));

    createPositionPropertyCommand(myGraceNoteCommand, tr("Grace Note"),
                                  "Notes.GraceNote", QKeySequence(),
                                  Position::Acciaccatura,
                                  QStringLiteral(u":images/grace.png"));

    createPositionPropertyCommand(myStaccatoCommand, tr("Staccato"),
                                  "Notes.Staccato", Qt::Key_Z,
                                  Position::Staccato, QStringLiteral(u":images/staccato.png"));

    createPositionPropertyCommand(myMarcatoCommand, tr("Accent"), "Notes.Accent",
                                  Qt::Key_A, Position::Marcato,
                                  QStringLiteral(u":images/accent_normal.png"));

    createPositionPropertyCommand(mySforzandoCommand, tr("Heavy Accent"),
                                  "Notes.HeavyAccent", QKeySequence(),
                                  Position::Sforzando,
                                  QStringLiteral(u":images/accent_heavy.png"));

    // Octave actions
    createNotePropertyCommand(myOctave8vaCommand, tr("8va"), "Notes.Octave.8va",
                              QKeySequence(), Note::Octave8va,
                              QStringLiteral(u":images/8va.png"));
    createNotePropertyCommand(myOctave15maCommand, tr("15ma"), "Notes.Octave.15ma",
                              QKeySequence(), Note::Octave15ma,
                              QStringLiteral(u":images/15ma.png"));
    createNotePropertyCommand(myOctave8vbCommand, tr("8vb"), "Notes.Octave.8vb",
                              QKeySequence(), Note::Octave8vb,
                              QStringLiteral(u":images/8vb.png"));
    createNotePropertyCommand(myOctave15mbCommand, tr("15mb"),
                              "Notes.Octave.15mb", QKeySequence(),
                              Note::Octave15mb, QStringLiteral(u":images/15mb.png"));

    myTripletCommand = new Command(tr("Triplet"), "Notes.Triplet", Qt::Key_E, this,
                                   QStringLiteral(u":images/group_note"));
    connect(myTripletCommand, &QAction::triggered, [=]() {
        editIrregularGrouping(true);
    });

    myIrregularGroupingCommand = new Command(
                                             tr("Irregular Grouping"), "Notes.IrregularGrouping", Qt::Key_I, this,
                                             QStringLiteral(u":images/group_note_irregular.png"));
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
                                   this, QStringLiteral(u":images/quarter_rest"));
    connect(myAddRestCommand, &QAction::triggered, this,
            &PowerTabEditor::addRest);

    myMultibarRestCommand = new Command(
        tr("Multi-Bar Rest..."), "Rests.MultibarRest", QKeySequence(), this,
        QStringLiteral(u":images/rest_multibar.png"));
    myMultibarRestCommand->setCheckable(true);
    connect(myMultibarRestCommand, &QAction::triggered, this,
            [=]() { editMultiBarRest(); });

    // Music Symbol Actions
    myRehearsalSignCommand =
        new Command(tr("Rehearsal Sign..."), "MusicSymbols.RehearsalSign",
                    Qt::SHIFT + Qt::Key_R, this);
    myRehearsalSignCommand->setCheckable(true);
    connect(myRehearsalSignCommand, &QAction::triggered, this,
            [=]() { editRehearsalSign(); });

    myTempoMarkerCommand = new Command(
        tr("Tempo Marker..."), "MusicSymbols.TempoMarker", Qt::Key_O, this,
        QStringLiteral(u":images/tempo.png"));
    myTempoMarkerCommand->setCheckable(true);
    connect(myTempoMarkerCommand, &QAction::triggered, this,
            [=]() { editTempoMarker(); });

    myAlterationOfPaceCommand =
        new Command(tr("Alteration of Pace..."),
                    "MusicSymbols.AlterationOfPace", QKeySequence(), this);
    myAlterationOfPaceCommand->setCheckable(true);
    connect(myAlterationOfPaceCommand, &QAction::triggered, this,
            [=]() { editAlterationOfPace(); });

    myKeySignatureCommand = new Command(tr("Edit Key Signature..."),
                                        "MusicSymbols.EditKeySignature",
                                        Qt::Key_K, this,
                                        QStringLiteral(u":images/keysignature.png"));
    connect(myKeySignatureCommand, &QAction::triggered, this,
            &PowerTabEditor::editKeySignature);

    myTimeSignatureCommand = new Command(tr("Edit Time Signature..."),
                                   "MusicSymbols.EditTimeSignature",
                                         Qt::Key_T, this,
                                         QStringLiteral(u":images/timesignature.png"));
    connect(myTimeSignatureCommand, &QAction::triggered, this,
            &PowerTabEditor::editTimeSignature);

    myStandardBarlineCommand = new Command(tr("Insert Standard Barline"),
                                     "MusicSymbols.InsertStandardBarline",
                                           Qt::Key_B, this,
                                           QStringLiteral(u":images/barline_single.png"));
    connect(myStandardBarlineCommand, &QAction::triggered, this,
            &PowerTabEditor::insertStandardBarline);

    myBarlineCommand = new Command(tr("Barline..."), "MusicSymbols.Barline",
                                   Qt::SHIFT + Qt::Key_B, this);
    connect(myBarlineCommand, &QAction::triggered, this,
            &PowerTabEditor::editBarline);

    myDirectionCommand =
        new Command(tr("Musical Direction..."), "MusicSymbols.MusicalDirection",
                    Qt::SHIFT + Qt::Key_D, this,
                    QStringLiteral(u":images/coda.png"));
    myDirectionCommand->setCheckable(true);
    connect(myDirectionCommand, &QAction::triggered, this,
            [=]() { editMusicalDirection(); });

    myRepeatEndingCommand =
        new Command(tr("Repeat Ending..."), "MusicSymbols.RepeatEnding",
                    Qt::SHIFT + Qt::Key_E, this,
                    QStringLiteral(u":images/barline_repeatend.png"));
    myRepeatEndingCommand->setCheckable(true);
    connect(myRepeatEndingCommand, &QAction::triggered, this,
            [=]() { editRepeatEnding(); });

    myDynamicCommand =
        new Command(tr("Dynamic..."), "MusicSymbols.Dynamic", Qt::Key_D, this);
    myDynamicCommand->setCheckable(true);
    connect(myDynamicCommand, &QAction::triggered, this,
            [=]() { editDynamic(); });

    myDynamicGroup = new QActionGroup(this);

    createDynamicCommand(myDynamicPPPCommand, tr("Dynamics"),
                         "Dynamics.ppp", VolumeLevel::ppp,
                         QStringLiteral(u":images/dynamic_ppp.png"));
    createDynamicCommand(myDynamicPPCommand, tr("Dynamics"),
                         "Dynamics.pp", VolumeLevel::pp,
                         QStringLiteral(u":images/dynamic_pp.png"));
    createDynamicCommand(myDynamicPCommand, tr("Dynamics"),
                         "Dynamics.p", VolumeLevel::p,
                         QStringLiteral(u":images/dynamic_p.png"));
    createDynamicCommand(myDynamicMPCommand, tr("Dynamics"),
                         "Dynamics.mp", VolumeLevel::mp,
                         QStringLiteral(u":images/dynamic_mp.png"));
    createDynamicCommand(myDynamicMFCommand, tr("Dynamics"),
                         "Dynamics.mf", VolumeLevel::mf,
                         QStringLiteral(u":images/dynamic_mf.png"));
    createDynamicCommand(myDynamicFCommand, tr("Dynamics"),
                         "Dynamics.f", VolumeLevel::f,
                         QStringLiteral(u":images/dynamic_f.png"));
    createDynamicCommand(myDynamicFFCommand, tr("Dynamics"),
                         "Dynamics.ff", VolumeLevel::ff,
                         QStringLiteral(u":images/dynamic_ff.png"));
    createDynamicCommand(myDynamicFFFCommand, tr("Dynamics"),
                         "Dynamics.fff", VolumeLevel::fff,
                         QStringLiteral(u":images/dynamic_fff.png"));

    myVolumeSwellCommand =
        new Command(tr("Volume Swell..."), "MusicSymbols.VolumeSwell",
                    QKeySequence(), this);
    myVolumeSwellCommand->setCheckable(true);
    connect(myVolumeSwellCommand, &QAction::triggered, this,
            [=] { editVolumeSwell(); });

    // Tab Symbol Actions.
    myHammerPullCommand = new Command(tr("Hammer On/Pull Off"),
                                      "TabSymbols.HammerPull", Qt::Key_H, this,
                                      QStringLiteral(u":images/legato"));//image name wrong
    myHammerPullCommand->setCheckable(true);
    connect(myHammerPullCommand, &QAction::triggered, this,
            &PowerTabEditor::editHammerPull);

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
                              Note::NaturalHarmonic, QStringLiteral(u":images/harmonic_natural.png"));

    myArtificialHarmonicCommand =
        new Command(tr("Artificial Harmonic..."),
                    "TabSymbols.ArtificialHarmonic", QKeySequence(), this,
                    QStringLiteral(u":images/harmonic_artificial.png"));
    myArtificialHarmonicCommand->setCheckable(true);
    connect(myArtificialHarmonicCommand, &QAction::triggered, this,
            &PowerTabEditor::editArtificialHarmonic);

    myTappedHarmonicCommand = new Command(tr("Tapped Harmonic..."),
                                          "TabSymbols.TappedHarmonic",
                                          QKeySequence(), this,
                                          QStringLiteral(u":images/harmonic_tapped.png"));
    myTappedHarmonicCommand->setCheckable(true);
    connect(myTappedHarmonicCommand, &QAction::triggered, this,
            &PowerTabEditor::editTappedHarmonic);

    myBendCommand =
        new Command(tr("Bend..."), "TabSymbols.Bend", QKeySequence(), this,
                    QStringLiteral(u":images/bend"));
    myBendCommand->setCheckable(true);
    connect(myBendCommand, &QAction::triggered, this,
            [=]() { editBend(); });

    createPositionPropertyCommand(myVibratoCommand, tr("Vibrato"),
                                  "TabSymbols.Vibrato", Qt::Key_V,
                                  Position::Vibrato,
                                  QStringLiteral(u":images/vibrato.png"));

    createPositionPropertyCommand(myWideVibratoCommand, tr("Wide Vibrato"),
                                  "TabSymbols.WideVibrato", Qt::Key_W,
                                  Position::WideVibrato,
                                  QStringLiteral(u":images/widevibrato.png"));

    createPositionPropertyCommand(myPalmMuteCommand, tr("Palm Mute"),
                                  "TabSymbols.PalmMute", Qt::Key_M,
                                  Position::PalmMuting, QStringLiteral(u":images/palm_mute.png"));

    createPositionPropertyCommand(myTremoloPickingCommand, tr("Tremolo Picking"),
                                  "TabSymbols.TremoloPicking", QKeySequence(),
                                  Position::TremoloPicking);

    createPositionPropertyCommand(myArpeggioUpCommand, tr("Arpeggio Up"),
                                  "TabSymbols.ArpeggioUp", QKeySequence(),
                                  Position::ArpeggioUp,
                                  QStringLiteral(u":images/arpeggio_up.png"));

    createPositionPropertyCommand(myArpeggioDownCommand, tr("Arpeggio Down"),
                                  "TabSymbols.ArpeggioDown", QKeySequence(),
                                  Position::ArpeggioDown,
                                  QStringLiteral(u":images/arpeggio_down.png"));

    createPositionPropertyCommand(myTapCommand, tr("Tap"), "TabSymbols.Tap",
                                  Qt::Key_P, Position::Tap,
                                  QStringLiteral(u":images/tap.png"));

    myTrillCommand = new Command(tr("Trill..."), "TabSymbols.Trill",
                                 QKeySequence(), this,
                                 QStringLiteral(u":images/trill.png"));
    myTrillCommand->setCheckable(true);
    connect(myTrillCommand, &QAction::triggered, this, &PowerTabEditor::editTrill);

    createPositionPropertyCommand(myPickStrokeUpCommand, tr("Pickstroke Up"),
                                  "TabSymbols.PickStrokeUp", QKeySequence(),
                                  Position::PickStrokeUp,
                                  QStringLiteral(u":images/pickstroke_up.png"));

    createPositionPropertyCommand(myPickStrokeDownCommand, tr("Pickstroke Down"),
                                  "TabSymbols.PickStrokeDown", QKeySequence(),
                                  Position::PickStrokeDown,
                                  QStringLiteral(u":images/pickstroke_down.png"));

    createNotePropertyCommand(mySlideIntoFromAboveCommand,
                              tr("Slide Into From Above"),
                              "TabSymbols.SlideInto.FromAbove", QKeySequence(),
                              Note::SlideIntoFromAbove,
                              QStringLiteral(u":images/slideinabove"));
    createNotePropertyCommand(mySlideIntoFromBelowCommand,
                              tr("Slide Into From Below"),
                              "TabSymbols.SlideInto.FromBelow", QKeySequence(),
                              Note::SlideIntoFromBelow,
                              QStringLiteral(u":images/slideinbelow"));

    createNotePropertyCommand(myShiftSlideCommand, tr("Shift Slide"),
                              "TabSymbols.ShiftSlide", Qt::Key_S,
                              Note::ShiftSlide,
                              QStringLiteral(u":images/shiftslide"));
    createNotePropertyCommand(myLegatoSlideCommand, tr("Legato Slide"),
                              "TabSymbols.LegatoSlide", Qt::Key_L,
                              Note::LegatoSlide,
                              QStringLiteral(u":images/legatoslide"));

    createNotePropertyCommand(mySlideOutOfDownwardsCommand,
                              tr("Slide Out Of Downwards"),
                              "TabSymbols.SlideOutOf.Downwards", QKeySequence(),
                              Note::SlideOutOfDownwards,
                              QStringLiteral(u":images/slideoutdown"));
    createNotePropertyCommand(mySlideOutOfUpwardsCommand,
                              tr("Slide Out Of Upwards"),
                              "TabSymbols.SlideOutOf.Upwards", QKeySequence(),
                              Note::SlideOutOfUpwards,
                              QStringLiteral(u":images/slideoutup"));

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
    connect(myPlayerChangeCommand, &QAction::triggered, this,
            [=]() { editPlayerChange(); });

    myShowTuningDictionaryCommand = new Command(tr("Tuning Dictionary..."),
                                                "Player.TuningDictionary",
                                                QKeySequence(), this);
    connect(myShowTuningDictionaryCommand, &QAction::triggered,
            this, &PowerTabEditor::showTuningDictionary);

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

    myInfoCommand = new Command(tr("App Info"), "Help.Info",
                                QKeySequence(), this);

    connect(myInfoCommand, &QAction::triggered, this, &PowerTabEditor::info);

    myMixerDockWidgetCommand =
        createCommandWrapper(myMixerDockWidget->toggleViewAction(),
                             "Window.Mixer", QKeySequence(), this);
    myInstrumentDockWidgetCommand =
        createCommandWrapper(myInstrumentDockWidget->toggleViewAction(),
                             "Window.Instruments", QKeySequence(), this);
    myToolBoxDockWidgetCommand =
        createCommandWrapper(myToolBoxDockWidget->toggleViewAction(),
                             "Window.Toolbox", QKeySequence(), this);
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
    auto commands = findChildren<const Command *>();
    return std::vector<const Command *>(commands.begin(), commands.end());
}

std::vector<Command *> PowerTabEditor::getCommands()
{
    auto commands = findChildren<Command *>();
    return std::vector<Command *>(commands.begin(), commands.end());
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

    myMixer = new Mixer(scroll, *myTuningDictionary);

    scroll->setWidget(myMixer);
    myMixerDockWidget->setWidget(scroll);
    addDockWidget(Qt::BottomDockWidgetArea, myMixerDockWidget);

    connect(myMixer, &Mixer::playerEdited,
            [=](int index, const Player &player, bool undoable) {
                editPlayer(index, player, undoable);
            });
    connect(myMixer, &Mixer::playerRemoved,
            [=](int index) { removePlayer(index); });
}

void PowerTabEditor::createInstrumentPanel()
{
    myInstrumentDockWidget = new QDockWidget(tr("Instruments"), this);
    myInstrumentDockWidget->setAllowedAreas(Qt::BottomDockWidgetArea);
    myInstrumentDockWidget->setFeatures(QDockWidget::DockWidgetClosable);
    myInstrumentDockWidget->setObjectName("Instruments");

    QScrollArea *scroll = new QScrollArea(this);
    scroll->setMinimumSize(0, 150);

    myInstrumentPanel = new InstrumentPanel(scroll);

    scroll->setWidget(myInstrumentPanel);
    myInstrumentDockWidget->setWidget(scroll);
    addDockWidget(Qt::BottomDockWidgetArea, myInstrumentDockWidget);

    connect(myInstrumentPanel, &InstrumentPanel::instrumentEdited,
            [=](int index, const Instrument &instrument) {
                editInstrument(index, instrument);
            });
    connect(myInstrumentPanel, &InstrumentPanel::instrumentRemoved,
            [=](int index) { removeInstrument(index); });
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
        Position::DurationType durationType, const QString &iconFileName)
{
    command = new Command(menuName, commandName, QKeySequence(), this,
                          iconFileName);
    command->setCheckable(true);
    connect(command, &QAction::triggered, [=]() {
        updateNoteDuration(durationType);
    });
    myNoteDurationGroup->addAction(command);
}

void PowerTabEditor::createRestDurationCommand(
        Command *&command, const QString &menuName, const QString &commandName,
        Position::DurationType durationType, const QString &iconFileName)
{
    command = new Command(menuName, commandName, QKeySequence(), this,
                          iconFileName);
    command->setCheckable(true);
    connect(command, &QAction::triggered, [=]() {
        editRest(durationType);
    });
    myRestDurationGroup->addAction(command);
}

void PowerTabEditor::createNotePropertyCommand(
        Command *&command, const QString &menuName, const QString &commandName,
        const QKeySequence &shortcut, Note::SimpleProperty property,
        const QString &iconFileName)
{
    command = new Command(menuName, commandName, shortcut, this, iconFileName);
    command->setCheckable(true);
    connect(command, &QAction::triggered, [=]() {
        editSimpleNoteProperty(command, property);
    });
}

void PowerTabEditor::createPositionPropertyCommand(
        Command *&command, const QString &menuName, const QString &commandName,
        const QKeySequence &shortcut, Position::SimpleProperty property,
        const QString &iconFileName)
{
    command = new Command(menuName, commandName, shortcut, this, iconFileName);
    command->setCheckable(true);
    connect(command, &QAction::triggered, [=]() {
        editSimplePositionProperty(command, property);
    });
}

void PowerTabEditor::createDynamicCommand(
        Command *&command, const QString &menuName, const QString &commandName,
        VolumeLevel volume, const QString &iconFileName)
{
    command = new Command(menuName, commandName, QKeySequence(), this,
                          iconFileName);
    command->setCheckable(true);
    connect(command, &QAction::triggered, [=]() {
        updateDynamic(volume);
    });
    myDynamicGroup->addAction(command);
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
    myFileMenu->addAction(myBulkConverterCommand);
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
    myEditMenu->addAction(myRemoveItemCommand);
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

    myPositionMenu->addSeparator();
    myPositionMenu->addAction(myInsertSpaceCommand);
    myPositionMenu->addAction(myRemoveSpaceCommand);
    myPositionMenu->addSeparator();
    myPositionMenu->addAction(myRemoveItemCommand);
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
    myNotesMenu->addAction(myLeftHandFingeringCommand);
    myNotesMenu->addAction(myShiftStringUpCommand);
    myNotesMenu->addAction(myShiftStringDownCommand);
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
    myMusicSymbolsMenu->addAction(myVolumeSwellCommand);

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
    myWindowMenu->addAction(myToolBoxDockWidgetCommand);

    // Help menu.
    myHelpMenu = menuBar()->addMenu(tr("&Help"));
    myHelpMenu->addAction(myReportBugCommand);
    myHelpMenu->addAction(myInfoCommand);
}

void PowerTabEditor::createToolBox()
{
    myToolBoxDockWidget->setAllowedAreas(Qt::LeftDockWidgetArea);
    myToolBoxDockWidget->setFeatures(QDockWidget::DockWidgetClosable);
    myToolBoxDockWidget->setObjectName("Toolbox");

    myToolBox = new ToolBox(myKeySignatureCommand,
                            myTimeSignatureCommand,
                            myStandardBarlineCommand,
                            myRepeatEndingCommand,
                            myOctave8vaCommand,
                            myOctave15maCommand,
                            myOctave8vbCommand,
                            myOctave15mbCommand,
                            myWholeNoteCommand,
                            myHalfNoteCommand,
                            myQuarterNoteCommand,
                            myEighthNoteCommand,
                            mySixteenthNoteCommand,
                            myThirtySecondNoteCommand,
                            mySixtyFourthNoteCommand,
                            myAddRestCommand,
                            myDottedCommand,
                            myDoubleDottedCommand,
                            myTripletCommand,
                            myIrregularGroupingCommand,
                            myTieCommand,
                            myFermataCommand,
                            myTempoMarkerCommand,
                            myMultibarRestCommand,
                            myDynamicPPPCommand,
                            myDynamicPPCommand,
                            myDynamicPCommand,
                            myDynamicMPCommand,
                            myDynamicMFCommand,
                            myDynamicFCommand,
                            myDynamicFFCommand,
                            myDynamicFFFCommand,
                            myMutedCommand,
                            myGhostNoteCommand,
                            myMarcatoCommand,
                            mySforzandoCommand,
                            myStaccatoCommand,
                            myLetRingCommand,
                            myPalmMuteCommand,
                            myTapCommand,
                            myNaturalHarmonicCommand,
                            myArtificialHarmonicCommand,
                            myTappedHarmonicCommand,
                            myBendCommand,
                            myVibratoCommand,
                            myWideVibratoCommand,
                            myGraceNoteCommand,
                            myLeftHandFingeringCommand,
                            myHammerPullCommand,
                            myTrillCommand,
                            myLegatoSlideCommand,
                            myShiftSlideCommand,
                            mySlideIntoFromAboveCommand,
                            mySlideIntoFromBelowCommand,
                            mySlideOutOfDownwardsCommand,
                            mySlideOutOfUpwardsCommand,
                            myPickStrokeUpCommand,
                            myPickStrokeDownCommand,
                            myArpeggioUpCommand,
                            myArpeggioDownCommand,
                            myTextCommand,
                            myDirectionCommand,
                            myToolBoxDockWidget);

    myToolBoxDockWidget->setWidget(myToolBox);
    addDockWidget(Qt::LeftDockWidgetArea, myToolBoxDockWidget);
}

void PowerTabEditor::createTabArea()
{
    myTabWidget = new QTabWidget(this);
    myTabWidget->setDocumentMode(true);
    myTabWidget->setTabsClosable(true);

    connect(myTabWidget, &QTabWidget::tabCloseRequested, this,
            &PowerTabEditor::closeTab);
    connect(myTabWidget, &QTabWidget::currentChanged, this,
            &PowerTabEditor::switchTab);

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

        // When changing location to somewhere on the staff, clear any existing
        // selected item.
        if (getCaret().getSelectedItem() == ScoreItem::Staff ||
            !myIsHandlingClick)
        {
            getScoreArea()->clearSelection();
        }
    });

    auto scorearea = new ScoreArea(*mySettingsManager, this);
    scorearea->renderDocument(doc);
    scorearea->installEventFilter(this);

    // Connect the signals for mouse clicks on time signatures, barlines, etc.
    // to the appropriate event handlers.
    connect(scorearea, &ScoreArea::itemClicked,
            [&](ScoreItem item, const ConstScoreLocation &location,
                ScoreItemAction action) {
                if (getCaret().isInPlaybackMode())
                    return;

                myIsHandlingClick = true;

                getCaret().moveToSystem(location.getSystemIndex(), true);
                getCaret().moveToPosition(location.getPositionIndex());
                getCaret().setSelectedItem(item);

                switch (action)
                {
                    case ScoreItemAction::Selected:
                        switch (item)
                        {
                            case ScoreItem::Staff:
                                getCaret().moveToLocation(location);
                                break;
                            default:
                                // Do nothing.
                                break;
                        }
                        break;

                    case ScoreItemAction::DoubleClicked:
                        switch (item)
                        {
                            case ScoreItem::Staff:
                                // Do nothing.
                                break;
                            case ScoreItem::Barline:
                                editBarline();
                                break;
                            case ScoreItem::TimeSignature:
                                editTimeSignature();
                                break;
                            case ScoreItem::KeySignature:
                                editKeySignature();
                                break;
                            case ScoreItem::Clef:
                                editStaff(location.getSystemIndex(),
                                          location.getStaffIndex());
                                break;
                            case ScoreItem::TempoMarker:
                                editTempoMarker();
                                break;
                            case ScoreItem::AlterationOfPace:
                                editAlterationOfPace();
                                break;
                            case ScoreItem::RehearsalSign:
                                editRehearsalSign();
                                break;
                            case ScoreItem::TextItem:
                                editTextItem();
                                break;
                            case ScoreItem::PlayerChange:
                                editPlayerChange();
                                break;
                            case ScoreItem::VolumeSwell:
                                editVolumeSwell();
                                break;
                            case ScoreItem::ScoreInfo:
                                editFileInformation();
                                break;
                            case ScoreItem::AlternateEnding:
                                editRepeatEnding();
                                break;
                            case ScoreItem::Direction:
                                editMusicalDirection();
                                break;
                            case ScoreItem::Dynamic:
                                editDynamic();
                                break;
                            case ScoreItem::MultiBarRest:
                                editMultiBarRest();
                                break;
                            case ScoreItem::ChordText:
                                editChordName();
                                break;
                            case ScoreItem::Bend:
                                editBend();
                                break;
                        }

                        // Clear the selection after editing. We may have also
                        // triggered a redraw anyways that cleared the graphics
                        // scene selection.
                        getScoreArea()->clearSelection();
                        getCaret().setSelectedItem(ScoreItem::Staff);
                        break;
                }

                myIsHandlingClick = false;
            });

    myUndoManager->addNewUndoStack();

    QString filename = "Untitled";
    if (doc.hasFilename())
        filename = Paths::toQString(doc.getFilename());

    QFileInfo fileInfo(filename);

    // Create title for the tab bar.
    QString title = fileInfo.fileName();
    QFontMetrics fm (myTabWidget->font());

    // Each tab is 200px wide, so we want to shorten the name if it's wider
    // than 140px.
    bool chopped = false;
#if (QT_VERSION >= QT_VERSION_CHECK(5,11,0))
    while (fm.horizontalAdvance(title) > 140)
#else
    while (fm.width(title) > 140)
#endif
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

bool
canDeleteItem(ScoreItem item)
{
    switch (item)
    {
        case ScoreItem::AlterationOfPace:
        case ScoreItem::AlternateEnding:
        case ScoreItem::Barline:
        case ScoreItem::Bend:
        case ScoreItem::ChordText:
        case ScoreItem::Direction:
        case ScoreItem::Dynamic:
        case ScoreItem::MultiBarRest:
        case ScoreItem::PlayerChange:
        case ScoreItem::RehearsalSign:
        case ScoreItem::TempoMarker:
        case ScoreItem::TextItem:
        case ScoreItem::VolumeSwell:
            return true;
        case ScoreItem::Clef:
        case ScoreItem::KeySignature:
        case ScoreItem::ScoreInfo:
        case ScoreItem::Staff:
        case ScoreItem::TimeSignature:
            return false;
    }

    // This is unreachable, but neccessary to avoid a gcc warning.
    return false;
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
    const bool positions_selected = !location.getSelectedPositions().empty();

    myRemoveCurrentSystemCommand->setEnabled(score.getSystems().size() > 1);
    myRemoveCurrentStaffCommand->setEnabled(system.getStaves().size() > 1);
    myIncreaseLineSpacingCommand->setEnabled(score.getLineSpacing() <
                                             Score::MAX_LINE_SPACING);
    myDecreaseLineSpacingCommand->setEnabled(score.getLineSpacing() >
                                             Score::MIN_LINE_SPACING);
    myRemoveSpaceCommand->setEnabled(!pos && (position == 0 || !barline) &&
                                     !tempoMarker && !altEnding && !dynamic);
    myRemoveItemCommand->setEnabled(
        pos || positions_selected ||
        canDeleteItem(getCaret().getSelectedItem()));
    myRemovePositionCommand->setEnabled(pos || positions_selected);

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
            myWholeRestCommand->setChecked(true);
            break;
        case Position::HalfNote:
            myHalfNoteCommand->setChecked(true);
            myHalfRestCommand->setChecked(true);
            break;
        case Position::QuarterNote:
            myQuarterNoteCommand->setChecked(true);
            myQuarterRestCommand->setChecked(true);
            break;
        case Position::EighthNote:
            myEighthNoteCommand->setChecked(true);
            myEighthRestCommand->setChecked(true);
            break;
        case Position::SixteenthNote:
            mySixteenthNoteCommand->setChecked(true);
            mySixteenthRestCommand->setChecked(true);
            break;
        case Position::ThirtySecondNote:
            myThirtySecondNoteCommand->setChecked(true);
            myThirtySecondRestCommand->setChecked(true);
            break;
        case Position::SixtyFourthNote:
            mySixtyFourthNoteCommand->setChecked(true);
            mySixtyFourthRestCommand->setChecked(true);
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

    myLeftHandFingeringCommand->setEnabled(note != nullptr);
    myLeftHandFingeringCommand->setChecked(note && note->hasLeftHandFingering());

    myShiftStringUpCommand->setEnabled(note != nullptr || positions_selected);
    myShiftStringDownCommand->setEnabled(note != nullptr || positions_selected);

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

    // dynamics
    myDynamicCommand->setChecked(dynamic != nullptr);

    if (dynamic)
    {
        switch (dynamic->getVolume())
        {
            case VolumeLevel::Off:
                break;
            case VolumeLevel::ppp:
                myDynamicPPPCommand->setChecked(true);
                break;
            case VolumeLevel::pp:
                myDynamicPPCommand->setChecked(true);
                break;
            case VolumeLevel::p:
                myDynamicPCommand->setChecked(true);
                break;
            case VolumeLevel::mp:
                myDynamicMPCommand->setChecked(true);
                break;
            case VolumeLevel::mf:
                myDynamicMFCommand->setChecked(true);
                break;
            case VolumeLevel::f:
                myDynamicFCommand->setChecked(true);
                break;
            case VolumeLevel::ff:
                myDynamicFFCommand->setChecked(true);
                break;
            case VolumeLevel::fff:
                myDynamicFFFCommand->setChecked(true);
                break;
        }
    }
    else
    {
        QAction *checkedAction = myDynamicGroup->checkedAction();
        if (checkedAction)
            checkedAction->setChecked(false);
    }

    myVolumeSwellCommand->setEnabled(pos);
    myVolumeSwellCommand->setChecked(pos && pos->hasVolumeSwell());

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

    // Set the duration for future notes / rests that are added.
    myActiveDurationType = duration;

    if (pos && pos->isRest())
    {
        // Adjust the duration of an existing rest.
        if (pos->getDurationType() != duration)
        {
            myUndoManager->push(new EditNoteDuration(location, duration, true),
                                location.getSystemIndex());
        }
    }
    else
    {
        // Convert or add a new rest.
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
        Util::toString(getCaret().getLocation()));
}

void PowerTabEditor::editKeySignature()
{
    ScoreLocation &location = getLocation();

    const Barline *barline = location.getBarline();
    Q_ASSERT(barline);

    KeySignatureDialog dialog(this, barline->getKeySignature());
    if (dialog.exec() == QDialog::Accepted)
    {
        myUndoManager->push(new EditKeySignature(location, dialog.getNewKey()),
                            UndoManager::AFFECTS_ALL_SYSTEMS);
    }
}

void PowerTabEditor::editTimeSignature()
{
    ScoreLocation &location = getLocation();

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

void PowerTabEditor::editBarline()
{
	ScoreLocation &location = getLocation();
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

void
PowerTabEditor::shiftString(bool shift_up)
{
    ScoreLocation location = getLocation();
    myUndoManager->push(new ShiftString(location, shift_up),
                        location.getSystemIndex());
    getCaret().moveVertical(shift_up ? -1 : 1);
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
