#include <QFileDialog>
#include <QDebug>
#include <QAction>
#include <QMenuBar>
#include <QMenu>
#include <QToolBox>
#include <QListView>
#include <QTabWidget>
#include <QUndoStack>
#include <QSettings>
#include <QCoreApplication>
#include <QFileInfo>
#include <QFontDatabase>
#include <QSplitter>
#include <QScrollArea>
#include <QStackedWidget>
#include <QSignalMapper>
#include <QActionGroup>

#include <powertabeditor.h>
#include <scorearea.h>
#include <skinmanager.h>
#include <midiplayer.h>

#include <dialogs/preferencesdialog.h>
#include <dialogs/chordnamedialog.h>
#include <dialogs/rehearsalsigndialog.h>

#include <powertabdocument/powertabdocument.h>

#include <widgets/mixer/mixer.h>
#include <widgets/toolbox/toolbox.h>

#include <painters/caret.h>

#include <actions/undomanager.h>
#include <actions/removechordtext.h>
#include <actions/addchordtext.h>
#include <actions/updatenoteduration.h>
#include <actions/editrehearsalsign.h>
#include <actions/toggleproperty.h>
#include <actions/shifttabnumber.h>

QTabWidget* PowerTabEditor::tabWidget = NULL;
std::unique_ptr<UndoManager> PowerTabEditor::undoManager(new UndoManager);
QSplitter* PowerTabEditor::vertSplitter = NULL;
QSplitter* PowerTabEditor::horSplitter = NULL;

PowerTabEditor::PowerTabEditor(QWidget *parent) :
    QMainWindow(parent),
    mixerList(new QStackedWidget),
    skinManager(new SkinManager("default"))
{
    this->setWindowIcon(QIcon(":icons/app_icon.png"));

    // load fonts from the resource file
    QFontDatabase::addApplicationFont(":fonts/emmentaler-13.otf"); // used for music notation
    QFontDatabase::addApplicationFont(":fonts/LiberationSans-Regular.ttf"); // used for tab notes

    // load application settings
    QCoreApplication::setOrganizationName("Power Tab");
    QCoreApplication::setApplicationName("Power Tab Editor");
    QSettings settings;
    // retrieve the previous directory that a file was opened/saved to (default value is home directory)
    previousDirectory = settings.value("app/previousDirectory", QDir::homePath()).toString();

    connect(undoManager.get(), SIGNAL(indexChanged(int)), this, SLOT(RefreshOnUndoRedo(int)));

    CreateActions();
    CreateMenus();
    CreateTabArea();

    isPlaying = false;

    setMinimumSize(800, 600);
    setWindowState(Qt::WindowMaximized);
    setWindowTitle(tr("Power Tab Editor 2.0"));

    horSplitter = new QSplitter();
    horSplitter->setOrientation(Qt::Horizontal);

    toolBox = new Toolbox(this, skinManager);
    horSplitter->addWidget(toolBox);
    horSplitter->addWidget(tabWidget);

    vertSplitter = new QSplitter();
    vertSplitter->setOrientation(Qt::Vertical);

    vertSplitter->addWidget(horSplitter);

    mixerList->setMinimumHeight(150);
    vertSplitter->addWidget(mixerList.get());

    setCentralWidget(vertSplitter);
}

PowerTabEditor::~PowerTabEditor()
{
}

// Refreshes the score area upon undo/redo
void PowerTabEditor::RefreshOnUndoRedo(int index)
{
    Q_UNUSED(index);

    Caret* caret = getCurrentScoreArea()->getCaret();
    // Update the data model
    caret->getCurrentScore()->UpdateSystemHeight(caret->getCurrentSystem());

    // Update the score area
    getCurrentScoreArea()->updateSystem(caret->getCurrentSystemIndex());

    // Update the status of all actions
    updateActions();
}

void PowerTabEditor::CreateActions()
{
    // File-related actions
    openFileAct = new QAction(tr("&Open..."), this);
    openFileAct->setShortcuts(QKeySequence::Open);
    openFileAct->setStatusTip(tr("Open an existing document"));
    connect(openFileAct, SIGNAL(triggered()), this, SLOT(OpenFile()));

    closeTabAct = new QAction(tr("&Close Tab"), this);
    closeTabAct->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_W));
    connect(closeTabAct, SIGNAL(triggered()), this, SLOT(closeCurrentTab()));

    preferencesAct = new QAction(tr("&Preferences..."), this);
    preferencesAct->setShortcuts(QKeySequence::Preferences);
    connect(preferencesAct, SIGNAL(triggered()), this, SLOT(OpenPreferences()));

    // Exit the application
    exitAppAct = new QAction(tr("&Quit"), this);
    exitAppAct->setShortcuts(QKeySequence::Quit);
    exitAppAct->setStatusTip(tr("Exit the application"));
    connect(exitAppAct, SIGNAL(triggered()), this, SLOT(close()));

    // Redo / Undo actions
    undoAct = undoManager->createUndoAction(this, tr("&Undo"));
    undoAct->setShortcuts(QKeySequence::Undo);

    redoAct = undoManager->createRedoAction(this, tr("&Redo"));
    redoAct->setShortcuts(QKeySequence::Redo);

    // Playback-related actions
    playPauseAct = new QAction(tr("Play"), this);
    playPauseAct->setShortcut(QKeySequence(Qt::Key_Space));
    connect(playPauseAct, SIGNAL(triggered()), this, SLOT(startStopPlayback()));

    // Section navigation actions
    firstSectionAct = new QAction(tr("First Section"), this);
    firstSectionAct->setShortcuts(QKeySequence::MoveToStartOfDocument);
    connect(firstSectionAct, SIGNAL(triggered()), this, SLOT(moveCaretToFirstSection()));

    nextSectionAct = new QAction(tr("Next Section"), this);
    nextSectionAct->setShortcuts(QKeySequence::MoveToNextPage);
    connect(nextSectionAct, SIGNAL(triggered()), this, SLOT(moveCaretToNextSection()));

    prevSectionAct = new QAction(tr("Previous Section"), this);
    prevSectionAct->setShortcuts(QKeySequence::MoveToPreviousPage);
    connect(prevSectionAct, SIGNAL(triggered()), this, SLOT(moveCaretToPrevSection()));

    lastSectionAct = new QAction(tr("Last Section"), this);
    lastSectionAct->setShortcuts(QKeySequence::MoveToEndOfDocument);
    connect(lastSectionAct, SIGNAL(triggered()), this, SLOT(moveCaretToLastSection()));

    // Position-related actions
    startPositionAct = new QAction(tr("Move to &Start"), this);
    startPositionAct->setShortcuts(QKeySequence::MoveToStartOfLine);
    connect(startPositionAct, SIGNAL(triggered()), this, SLOT(moveCaretToStart()));

    nextPositionAct = new QAction(tr("&Next Position"), this);
    nextPositionAct->setShortcuts(QKeySequence::MoveToNextChar);
    connect(nextPositionAct, SIGNAL(triggered()), this, SLOT(moveCaretRight()));

    prevPositionAct = new QAction(tr("&Previous Position"), this);
    prevPositionAct->setShortcuts(QKeySequence::MoveToPreviousChar);
    connect(prevPositionAct, SIGNAL(triggered()), this, SLOT(moveCaretLeft()));

    nextStringAct = new QAction(tr("Next String"), this);
    nextStringAct->setShortcuts(QKeySequence::MoveToNextLine);
    connect(nextStringAct, SIGNAL(triggered()), this, SLOT(moveCaretDown()));

    prevStringAct = new QAction(tr("Previous String"), this);
    prevStringAct->setShortcuts(QKeySequence::MoveToPreviousLine);
    connect(prevStringAct, SIGNAL(triggered()), this, SLOT(moveCaretUp()));

    lastPositionAct = new QAction(tr("Move to &End"), this);
    lastPositionAct->setShortcuts(QKeySequence::MoveToEndOfLine);
    connect(lastPositionAct, SIGNAL(triggered()), this, SLOT(moveCaretToEnd()));

    nextStaffAct = new QAction(tr("Next Staff"), this);
    nextStaffAct->setShortcut(QKeySequence(Qt::ALT + Qt::Key_Down));
    connect(nextStaffAct, SIGNAL(triggered()), this, SLOT(moveCaretToNextStaff()));

    prevStaffAct = new QAction(tr("Previous Staff"), this);
    prevStaffAct->setShortcut(QKeySequence(Qt::ALT + Qt::Key_Up));
    connect(prevStaffAct, SIGNAL(triggered()), this, SLOT(moveCaretToPrevStaff()));

    // Actions for shifting tab numbers up/down a string
    shiftTabNumberMapper = new QSignalMapper(this);

    shiftTabNumUp = new QAction(tr("Shift Tab Number Up"), this);
    shiftTabNumUp->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_Up));
    connect(shiftTabNumUp, SIGNAL(triggered()), shiftTabNumberMapper, SLOT(map()));
    shiftTabNumberMapper->setMapping(shiftTabNumUp, Position::SHIFT_UP);

    shiftTabNumDown = new QAction(tr("Shift Tab Number Down"), this);
    shiftTabNumDown->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_Down));
    connect(shiftTabNumDown, SIGNAL(triggered()), shiftTabNumberMapper, SLOT(map()));
    shiftTabNumberMapper->setMapping(shiftTabNumDown, Position::SHIFT_DOWN);

    connect(shiftTabNumberMapper, SIGNAL(mapped(int)), this, SLOT(shiftTabNumber(int)));

    // Text-related actions
    chordNameAct = new QAction(tr("Chord Name..."), this);
    chordNameAct->setShortcut(QKeySequence(Qt::Key_C));
    chordNameAct->setCheckable(true);
    connect(chordNameAct, SIGNAL(triggered()), this, SLOT(editChordName()));

    // Note-related actions
    noteDurationMapper = new QSignalMapper(this);
    noteDurationActGroup = new QActionGroup(this);

    wholeNoteAct = new QAction(tr("Whole"), this);
    wholeNoteAct->setCheckable(true);
    connect(wholeNoteAct, SIGNAL(triggered()), noteDurationMapper, SLOT(map()));
    noteDurationMapper->setMapping(wholeNoteAct, 1);
    noteDurationActGroup->addAction(wholeNoteAct);

    halfNoteAct = new QAction(tr("Half"), this);
    halfNoteAct->setCheckable(true);
    connect(halfNoteAct, SIGNAL(triggered()), noteDurationMapper, SLOT(map()));
    noteDurationMapper->setMapping(halfNoteAct, 2);
    noteDurationActGroup->addAction(halfNoteAct);

    quarterNoteAct = new QAction(tr("Quarter"), this);
    quarterNoteAct->setCheckable(true);
    connect(quarterNoteAct, SIGNAL(triggered()), noteDurationMapper, SLOT(map()));
    noteDurationMapper->setMapping(quarterNoteAct, 4);
    noteDurationActGroup->addAction(quarterNoteAct);

    eighthNoteAct = new QAction(tr("8th"), this);
    eighthNoteAct->setCheckable(true);
    connect(eighthNoteAct, SIGNAL(triggered()), noteDurationMapper, SLOT(map()));
    noteDurationMapper->setMapping(eighthNoteAct, 8);
    noteDurationActGroup->addAction(eighthNoteAct);

    sixteenthNoteAct = new QAction(tr("16th"), this);
    sixteenthNoteAct->setCheckable(true);
    connect(sixteenthNoteAct, SIGNAL(triggered()), noteDurationMapper, SLOT(map()));
    noteDurationMapper->setMapping(sixteenthNoteAct, 16);
    noteDurationActGroup->addAction(sixteenthNoteAct);

    thirtySecondNoteAct = new QAction(tr("32nd"), this);
    thirtySecondNoteAct->setCheckable(true);
    connect(thirtySecondNoteAct, SIGNAL(triggered()), noteDurationMapper, SLOT(map()));
    noteDurationMapper->setMapping(thirtySecondNoteAct, 32);
    noteDurationActGroup->addAction(thirtySecondNoteAct);

    sixtyFourthNoteAct = new QAction(tr("64th"), this);
    sixtyFourthNoteAct->setCheckable(true);
    connect(sixtyFourthNoteAct, SIGNAL(triggered()), noteDurationMapper, SLOT(map()));
    noteDurationMapper->setMapping(sixtyFourthNoteAct, 64);
    noteDurationActGroup->addAction(sixtyFourthNoteAct);

    connect(noteDurationMapper, SIGNAL(mapped(int)), this, SLOT(updateNoteDuration(int)));

    tiedNoteAct = new QAction(tr("Tied"), this);
    tiedNoteAct->setCheckable(true);
    connect(tiedNoteAct, SIGNAL(triggered()), this, SLOT(editTiedNote()));

    noteMutedAct = new QAction(tr("Muted"), this);
    noteMutedAct->setCheckable(true);
    connect(noteMutedAct, SIGNAL(triggered()), this, SLOT(editNoteMuted()));

    ghostNoteAct = new QAction(tr("Ghost Note"), this);
    ghostNoteAct->setCheckable(true);
    connect(ghostNoteAct, SIGNAL(triggered()), this, SLOT(editGhostNote()));

    staccatoNoteAct = new QAction(tr("Staccato"), this);
    staccatoNoteAct->setCheckable(true);
    staccatoNoteAct->setShortcut(QKeySequence(Qt::Key_Z));
    connect(staccatoNoteAct, SIGNAL(triggered()), this, SLOT(editStaccato()));

    // Music Symbol Actions
    rehearsalSignAct = new QAction(tr("Rehearsal Sign..."), this);
    rehearsalSignAct->setCheckable(true);
    rehearsalSignAct->setShortcut(QKeySequence(Qt::SHIFT + Qt::Key_R));
    connect(rehearsalSignAct, SIGNAL(triggered()), this, SLOT(editRehearsalSign()));

    // Tab Symbol Actions
    naturalHarmonicAct = new QAction(tr("Natural Harmonic"), this);
    naturalHarmonicAct->setCheckable(true);
    connect(naturalHarmonicAct, SIGNAL(triggered()), this, SLOT(editNaturalHarmonic()));

    // Window Menu Actions
    tabCycleMapper = new QSignalMapper(this);

    nextTabAct = new QAction(tr("Next Tab"), this);
    nextTabAct->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_Tab));
    connect(nextTabAct, SIGNAL(triggered()), tabCycleMapper, SLOT(map()));
    tabCycleMapper->setMapping(nextTabAct, 1);

    prevTabAct = new QAction(tr("Previous Tab"), this);
    prevTabAct->setShortcut(QKeySequence(Qt::CTRL + Qt::SHIFT + Qt::Key_Tab));
    connect(prevTabAct, SIGNAL(triggered()), tabCycleMapper, SLOT(map()));
    tabCycleMapper->setMapping(prevTabAct, -1);

    connect(tabCycleMapper, SIGNAL(mapped(int)), this, SLOT(cycleTab(int)));
}

void PowerTabEditor::CreateMenus()
{
    // File Menu
    fileMenu = menuBar()->addMenu(tr("&File"));
    fileMenu->addAction(openFileAct);
    fileMenu->addAction(closeTabAct);
    fileMenu->addSeparator();
    fileMenu->addAction(preferencesAct);
    fileMenu->addSeparator();
    fileMenu->addAction(exitAppAct);

    // Edit Menu
    editMenu = menuBar()->addMenu(tr("&Edit"));
    editMenu->addAction(undoAct);
    editMenu->addAction(redoAct);

    // Playback Menu
    playbackMenu = menuBar()->addMenu(tr("Play&back"));
    playbackMenu->addAction(playPauseAct);

    // Position Menu
    positionMenu = menuBar()->addMenu(tr("&Position"));

    positionSectionMenu = positionMenu->addMenu(tr("&Section"));
    positionSectionMenu->addAction(firstSectionAct);
    positionSectionMenu->addAction(nextSectionAct);
    positionSectionMenu->addAction(prevSectionAct);
    positionSectionMenu->addAction(lastSectionAct);

    positionStaffMenu = positionMenu->addMenu(tr("&Staff"));
    positionStaffMenu->addAction(startPositionAct);
    positionStaffMenu->addAction(nextPositionAct);
    positionStaffMenu->addAction(prevPositionAct);
    positionStaffMenu->addAction(nextStringAct);
    positionStaffMenu->addAction(prevStringAct);
    positionStaffMenu->addAction(lastPositionAct);
    positionStaffMenu->addAction(nextStaffAct);
    positionStaffMenu->addAction(prevStaffAct);

    positionMenu->addSeparator();
    positionMenu->addAction(shiftTabNumUp);
    positionMenu->addAction(shiftTabNumDown);

    // Text Menu
    textMenu = menuBar()->addMenu(tr("&Text"));
    textMenu->addAction(chordNameAct);

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
    notesMenu->addAction(tiedNoteAct);
    notesMenu->addSeparator();
    notesMenu->addAction(noteMutedAct);
    notesMenu->addSeparator();
    notesMenu->addAction(ghostNoteAct);
    notesMenu->addSeparator();
    notesMenu->addAction(staccatoNoteAct);

    // Music Symbols Menu
    musicSymbolsMenu = menuBar()->addMenu(tr("&Music Symbols"));
    musicSymbolsMenu->addAction(rehearsalSignAct);

    // Tab Symbols Menu
    tabSymbolsMenu = menuBar()->addMenu(tr("&Tab Symbols"));
    tabSymbolsMenu->addAction(naturalHarmonicAct);

    // Window Menu
    windowMenu = menuBar()->addMenu(tr("&Window"));
    windowMenu->addAction(nextTabAct);
    windowMenu->addAction(prevTabAct);
}

void PowerTabEditor::CreateTabArea()
{
    tabWidget = new QTabWidget;
    tabWidget->setTabsClosable(true);

    tabWidget->setStyleSheet(skinManager->getDocumentTabStyle());

    // creates a new document by default
    /*ScoreArea* score = new ScoreArea;
    score->renderDocument(documentManager.getCurrentDocument());
    tabWidget->addTab(score, tr("Untitled"));*/

    connect(tabWidget, SIGNAL(tabCloseRequested(int)), this, SLOT(closeTab(int)));
    connect(tabWidget, SIGNAL(currentChanged(int)), this, SLOT(switchTab(int)));

    updateScoreAreaActions(false);
}

// Open a new file
void PowerTabEditor::OpenFile()
{
    QString fileFilter;
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open"), previousDirectory, fileFilter);

    if (fileName.isEmpty())
    {
        qDebug() << "No file selected";
    }
    else
    {
        qDebug() << "Opening file: " << fileName;
        bool success = documentManager.Add(fileName); // add the file to the document manager

        // draw the score if it was successful
        if (success)
        {
            ScoreArea* score = new ScoreArea;
            score->renderDocument(documentManager.getCurrentDocument());
            QFileInfo fileInfo(fileName);
            // save this as the previous directory
            previousDirectory = fileInfo.absolutePath();
            QSettings settings;
            settings.setValue("app/previousDirectory", previousDirectory);

            QString title = fileInfo.fileName();
            QFontMetrics fm (tabWidget->font());

            bool chopped = false;

            // each tab is 200px wide, so we want to shorten the name if it's wider than 140px
            while(fm.width(title)>140)
            {
                title.chop(1);
                chopped = true;
            }

            if (chopped)
                title.append("...");

            undoManager->addNewUndoStack();

            connect(score->getCaret(), SIGNAL(moved()), this, SLOT(updateActions()));

            tabWidget->addTab(score, title);

            // add the guitars to a new mixer
            Mixer* mixer = new Mixer(skinManager);
            QScrollArea* scrollArea = new QScrollArea;
            PowerTabDocument* doc = documentManager.getCurrentDocument();
            for (quint32 i=0; i < doc->GetGuitarScore()->GetGuitarCount(); i++)
            {
                mixer->AddInstrument(doc->GetGuitarScore()->GetGuitar(i));
            }
            scrollArea->setWidget(mixer);
            mixerList->addWidget(scrollArea);

            // switch to the new document
            tabWidget->setCurrentIndex(documentManager.getCurrentDocumentIndex());

             // if this is the first open document, enable score area actions
            if (documentManager.getCurrentDocumentIndex() == 0)
            {
                updateScoreAreaActions(true);
            }

            updateActions(); // update available actions for the current position
        }
    }
}

// Opens the preferences dialog
void PowerTabEditor::OpenPreferences()
{
    std::unique_ptr<PreferencesDialog> preferencesDialog(new PreferencesDialog);
    preferencesDialog->exec();
}

void PowerTabEditor::closeCurrentTab()
{
    closeTab(tabWidget->currentIndex());
}

// Close a document and clean up
void PowerTabEditor::closeTab(int index)
{
    undoManager->removeStack(index);
    documentManager.Remove(index);
    delete tabWidget->widget(index);

    mixerList->removeWidget(mixerList->widget(index));

    // get the index of the tab that we will now switch to
    const int currentIndex = tabWidget->currentIndex();

    undoManager->setActiveStackIndex(currentIndex);
    mixerList->setCurrentIndex(currentIndex);
    documentManager.setCurrentDocumentIndex(currentIndex);

    if (currentIndex == -1) // disable score-related actions if no documents are open
    {
        updateScoreAreaActions(false);
    }
}

// When the tab is switched, switch the current document in the document manager
void PowerTabEditor::switchTab(int index)
{
    documentManager.setCurrentDocumentIndex(index);
    mixerList->setCurrentIndex(index);
    undoManager->setActiveStackIndex(index);

    if(documentManager.getCurrentDocument())
    {
        QString title(documentManager.getCurrentDocument()->GetFileName().c_str());
        title.remove(0,title.lastIndexOf("/")+1);
        setWindowTitle(title+tr(" - Power Tab Editor 2.0"));
    }
    else
    {
        setWindowTitle(tr("Power Tab Editor 2.0"));
    }
}

ScoreArea* PowerTabEditor::getCurrentScoreArea()
{
    return reinterpret_cast<ScoreArea*>(tabWidget->currentWidget());
}

/// Cycles through the tabs in the tab bar
/// @param offset Direction and number of tabs to move by (i.e. -1 moves back one tab)
void PowerTabEditor::cycleTab(int offset)
{
    int newIndex = (tabWidget->currentIndex() + offset) % tabWidget->count();

    if (newIndex < 0) // make sure that negative array indices wrap around
    {
        newIndex += tabWidget->count();
    }

    tabWidget->setCurrentIndex(newIndex);
}

void PowerTabEditor::startStopPlayback()
{
    isPlaying = !isPlaying;

    if (isPlaying)
    {
        playPauseAct->setText(tr("Pause"));

        getCurrentScoreArea()->getCaret()->setPlaybackMode(true);

        midiPlayer.reset(new MidiPlayer(getCurrentScoreArea()->getCaret()));
        connect(midiPlayer.get(), SIGNAL(playbackSystemChanged()), this, SLOT(moveCaretToNextSection()));
        connect(midiPlayer.get(), SIGNAL(playbackPositionChanged(quint8)), this, SLOT(moveCaretToPosition(quint8)));
        connect(midiPlayer.get(), SIGNAL(finished()), this, SLOT(startStopPlayback()));
        midiPlayer->start();
    }
    else
    {
        // if we manually stop playback, let the midi thread finish, and the finished() signal will trigger this function again
        if (midiPlayer.get() != NULL && midiPlayer->isRunning())
        {
            isPlaying = true;
            midiPlayer.reset();
            return;
        }

        playPauseAct->setText(tr("Play"));
        getCurrentScoreArea()->getCaret()->setPlaybackMode(false);
    }
}

void PowerTabEditor::shiftTabNumber(int direction)
{
    const Position::ShiftType shiftType = static_cast<Position::ShiftType>(direction);
    Caret* caret = getCurrentScoreArea()->getCaret();
    const quint8 numStringsInStaff = caret->getCurrentStaff()->GetTablatureStaffType();
    Position* currentPos = caret->getCurrentPosition();
    Note* currentNote = caret->getCurrentNote();
    const Tuning& tuning = caret->getCurrentScore()->GetGuitar(caret->getCurrentStaffIndex())->GetTuningConstRef();

    if (!currentPos->CanShiftTabNumber(currentNote, shiftType, numStringsInStaff, tuning))
    {
        return;
    }

    undoManager->push(new ShiftTabNumber(caret, currentPos, currentNote, shiftType, numStringsInStaff, tuning));
}

bool PowerTabEditor::moveCaretToPosition(quint8 position)
{
    return getCurrentScoreArea()->getCaret()->setCurrentPositionIndex(position);
}

bool PowerTabEditor::moveCaretRight()
{
    return getCurrentScoreArea()->getCaret()->moveCaretHorizontal(1);
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

// If there is a chord name at the current position, remove it
// If there is no chord name, show the dialog to add a chord name
// Existing chord names are edited by clicking on the chord name
void PowerTabEditor::editChordName()
{
    // Find if there is a chord name at the current position
    Caret* caret = getCurrentScoreArea()->getCaret();
    const quint32 caretPosition = caret->getCurrentPositionIndex();
    System* currentSystem = caret->getCurrentSystem();

    int chordTextIndex = caret->getCurrentSystem()->FindChordText(caretPosition);
    if (chordTextIndex == -1) // if not found, add a new chord name
    {
        chordTextIndex = 0;
        ChordName chordName;
        ChordNameDialog chordNameDialog(&chordName);
        if (chordNameDialog.exec() == QDialog::Accepted)
        {
            ChordText* chordText = new ChordText(caretPosition, chordName);
            undoManager->push(new AddChordText(currentSystem, chordText, chordTextIndex));
        }
    }
    else // if found, remove the chord name
    {
        undoManager->push(new RemoveChordText(currentSystem, chordTextIndex));
    }

}

// If there is a rehearsal sign at the barline, remove it
// If there is no rehearsal sign, show the dialog to add one
void PowerTabEditor::editRehearsalSign()
{
    // Find if there is a rehearsal sign at the current position
    Caret* caret = getCurrentScoreArea()->getCaret();
    const quint32 caretPosition = caret->getCurrentPositionIndex();
    Score* currentScore = caret->getCurrentScore();
    Barline* currentBarline = caret->getCurrentSystem()->GetBarlineAtPosition(caretPosition);

    // the rehearsal sign action should not be available unless there is a barline at the current position
    Q_ASSERT(currentBarline != NULL);

    RehearsalSign* rehearsalSign = currentBarline->GetRehearsalSignPtr();

    if (rehearsalSign->IsSet())
    {
        undoManager->push(new EditRehearsalSign(rehearsalSign, false));
    }
    else
    {
        RehearsalSignDialog rehearsalSignDialog(currentScore, rehearsalSign);
        rehearsalSignDialog.exec();
    }

}

void PowerTabEditor::editNaturalHarmonic()
{
    Note* note = getCurrentScoreArea()->getCaret()->getCurrentNote();
    Q_ASSERT(note != NULL); // this action should not be available if there is no note at this position

    const bool hasHarmonic = note->IsNaturalHarmonic();
    const QString text = hasHarmonic ? tr("Remove Natural Harmonic") : tr("Set Natural Harmonic");

    undoManager->push(new ToggleProperty<Note>(note, &Note::SetNaturalHarmonic, !hasHarmonic, text));
}

void PowerTabEditor::editNoteMuted()
{
    Note* note = getCurrentScoreArea()->getCaret()->getCurrentNote();
    Q_ASSERT(note != NULL); // this action should not be available if there is no note at this position

    const bool isMuted = note->IsMuted();
    const QString text = isMuted ? tr("Remove Note Muted") : tr("Set Note Muted");

    undoManager->push(new ToggleProperty<Note>(note, &Note::SetMuted, !isMuted, text));
}

void PowerTabEditor::editStaccato()
{
    Position* position = getCurrentScoreArea()->getCaret()->getCurrentPosition();

    const bool isStaccato = position->IsStaccato();
    const QString text = isStaccato ? tr("Remove Staccato") : tr("Set Staccato");

    undoManager->push(new ToggleProperty<Position>(position, &Position::SetStaccato, !isStaccato, text));
}

void PowerTabEditor::editGhostNote()
{
    Note* note = getCurrentScoreArea()->getCaret()->getCurrentNote();

    const bool isGhostNote = note->IsGhostNote();
    const QString text = isGhostNote ? tr("Remove Ghost Note") : tr("Set Ghost Note");

    undoManager->push(new ToggleProperty<Note>(note, &Note::SetGhostNote, !isGhostNote, text));
}

void PowerTabEditor::editTiedNote()
{
    Caret* caret = getCurrentScoreArea()->getCaret();
    Note* note = caret->getCurrentNote();
    Position* currentPosition = caret->getCurrentPosition();
    Staff* currentStaff = caret->getCurrentStaff();

    // check if note is able to be tied
    if (!currentStaff->CanTieNote(currentPosition, note))
    {
        tiedNoteAct->setChecked(false);
        return;
    }

    const bool isTied = note->IsTied();
    const QString text = isTied ? tr("Remove Note Tie") : tr("Set Note Tie");

    undoManager->push(new ToggleProperty<Note>(note, &Note::SetTied, !isTied, text));
}

// Updates the given QAction to be checked and/or enabled, based on the results of calling
// the predicate member function of the provided object
namespace
{
    template<class T>
    void updatePropertyStatus(QAction* action, T* object, bool (T::*predicate)(void) const)
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

// Updates whether menu items are checked, etc, whenever the caret moves
void PowerTabEditor::updateActions()
{
    Caret* caret = getCurrentScoreArea()->getCaret();
    const quint32 caretPosition = caret->getCurrentPositionIndex();
    System* currentSystem = caret->getCurrentSystem();
    Position* currentPosition = caret->getCurrentPosition();
    Barline* currentBarline = currentSystem->GetBarlineAtPosition(caretPosition);
    Note* currentNote = caret->getCurrentNote();

    // Check for chord text
    chordNameAct->setChecked(currentSystem->HasChordText(caretPosition));

    // note duration
    if (currentPosition != NULL)
    {
        quint8 duration = currentPosition->GetDurationType();
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
    }

    RehearsalSign* currentRehearsalSign = currentBarline == NULL ? NULL : currentBarline->GetRehearsalSignPtr();
    updatePropertyStatus(rehearsalSignAct, currentRehearsalSign, &RehearsalSign::IsSet);

    updatePropertyStatus(naturalHarmonicAct, currentNote, &Note::IsNaturalHarmonic);
    updatePropertyStatus(noteMutedAct, currentNote, &Note::IsMuted);
    updatePropertyStatus(ghostNoteAct, currentNote, &Note::IsGhostNote);
    updatePropertyStatus(tiedNoteAct, currentNote, &Note::IsTied);
    updatePropertyStatus(staccatoNoteAct, currentPosition, &Position::IsStaccato);

    shiftTabNumDown->setEnabled(currentNote != NULL);
    shiftTabNumUp->setEnabled(currentNote != NULL);
}

// Enables/disables actions that should only be available when a score is opened
void PowerTabEditor::updateScoreAreaActions(bool enable)
{
    QList<QMenu*> menuList;
    menuList << playbackMenu << positionMenu << textMenu << notesMenu << musicSymbolsMenu << tabSymbolsMenu << windowMenu;

    QAction* action;
    QMenu* menu;

    foreach(menu, menuList)
    {
        foreach(action, menu->actions())
        {
            action->setEnabled(enable);
        }
    }

    closeTabAct->setEnabled(enable);
}

void PowerTabEditor::updateNoteDuration(int duration)
{
    Position* currentPosition = getCurrentScoreArea()->getCaret()->getCurrentPosition();
    undoManager->push(new UpdateNoteDuration(currentPosition, duration));
}
