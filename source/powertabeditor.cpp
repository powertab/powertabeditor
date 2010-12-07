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
#include <QSignalMapper>
#include <QSplitter>
#include <QScrollArea>
#include <QStackedWidget>
#include <QTimer>

#include "powertabeditor.h"
#include "scorearea.h"
#include "painters/caret.h"
#include "skinmanager.h"
#include "rtmidiwrapper.h"
#include "dialogs/preferencesdialog.h"
#include "powertabdocument/powertabdocument.h"
#include "widgets/mixer/mixer.h"
#include "widgets/toolbox/toolbox.h"

QTabWidget* PowerTabEditor::tabWidget = NULL;
QUndoStack* PowerTabEditor::undoStack = NULL;
QSplitter* PowerTabEditor::vertSplitter = NULL;
QSplitter* PowerTabEditor::horSplitter = NULL;
Toolbox* PowerTabEditor::toolBox = NULL;

PowerTabEditor::PowerTabEditor(QWidget *parent) :
        QMainWindow(parent)
{
    signalMapper = new QSignalMapper();

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

    undoStack = new QUndoStack();
    connect(undoStack, SIGNAL(indexChanged(int)), this, SLOT(RefreshOnUndoRedo(int)));

    skinManager = new SkinManager("default");

    CreateActions();
    CreateMenus();
    CreateTabArea();

    rtMidiWrapper = new RtMidiWrapper();
    rtMidiWrapper->initialize(settings.value("midi/preferredPort", 0).toInt());
    rtMidiWrapper->setPatch(0,24);

    isPlaying = false;

    for (int i=0; i<8; ++i)
    {
        songTimer[i] = new QTimer;
        connect(songTimer[i],SIGNAL(timeout()),signalMapper,SLOT(map()));
        signalMapper->setMapping(songTimer[i], i);
    }

    connect(signalMapper, SIGNAL(mapped(const int)), this, SLOT(playbackSong(int)), Qt::DirectConnection);

    preferencesDialog = new PreferencesDialog();

    setMinimumSize(800, 600);
    setWindowState(Qt::WindowMaximized);
    setWindowTitle(tr("Power Tab Editor 2.0"));

    horSplitter = new QSplitter();
    horSplitter->setOrientation(Qt::Horizontal);

    toolBox = new Toolbox(0,skinManager);
    horSplitter->addWidget(toolBox);
    horSplitter->addWidget(tabWidget);

    vertSplitter = new QSplitter();
    vertSplitter->setOrientation(Qt::Vertical);

    vertSplitter->addWidget(horSplitter);

    mixerList = new QStackedWidget;
    mixerList->setMinimumHeight(150);
    vertSplitter->addWidget(mixerList);

    setCentralWidget(vertSplitter);
}

PowerTabEditor::~PowerTabEditor()
{
    for (int i=0; i<8; i++)
    {
        delete songTimer[i];
    }

    delete rtMidiWrapper;
    delete preferencesDialog;
    delete skinManager;
}

// Redraws the *entire* document upon undo/redo
// TODO - notify the appropriate painter to redraw itself, instead
// of redrawing the whole score
void PowerTabEditor::RefreshOnUndoRedo(int index)
{
    Q_UNUSED(index);
    RefreshCurrentDocument();
}

void PowerTabEditor::CreateActions()
{
    // File-related actions
    openFileAct = new QAction(tr("&Open..."), this);
    openFileAct->setShortcuts(QKeySequence::Open);
    openFileAct->setStatusTip(tr("Open an existing document"));
    connect(openFileAct, SIGNAL(triggered()), this, SLOT(OpenFile()));

    preferencesAct = new QAction(tr("&Preferences..."), this);
    preferencesAct->setShortcuts(QKeySequence::Preferences);
    connect(preferencesAct, SIGNAL(triggered()), this, SLOT(OpenPreferences()));

    // Exit the application
    exitAppAct = new QAction(tr("&Quit"), this);
    exitAppAct->setShortcuts(QKeySequence::Quit);
    exitAppAct->setStatusTip(tr("Exit the application"));
    connect(exitAppAct, SIGNAL(triggered()), this, SLOT(close()));

    // Redo / Undo actions
    undoAct = undoStack->createUndoAction(this, tr("&Undo"));
    undoAct->setShortcuts(QKeySequence::Undo);

    redoAct = undoStack->createRedoAction(this, tr("&Redo"));
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
}

void PowerTabEditor::CreateMenus()
{
    // File Menu
    fileMenu = menuBar()->addMenu(tr("&File"));
    fileMenu->addAction(openFileAct);
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
}

void PowerTabEditor::CreateTabArea()
{
    tabWidget = new QTabWidget;
    tabWidget->setTabsClosable(true);

    tabWidget->setStyleSheet(skinManager->getDocumentTabStyle());

    // creates a new document by default
    /*ScoreArea* score = new ScoreArea;
    score->RenderDocument(documentManager.getCurrentDocument());
    tabWidget->addTab(score, tr("Untitled"));*/

    connect(tabWidget, SIGNAL(tabCloseRequested(int)), this, SLOT(closeTab(int)));
    connect(tabWidget, SIGNAL(currentChanged(int)), this, SLOT(switchTab(int)));
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
            score->RenderDocument(documentManager.getCurrentDocument());
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

            tabWidget->addTab(score, title);

            // add the guitars to a new mixer
            Mixer* mixer = new Mixer(0,skinManager);
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
        }
    }
}

// Opens the preferences dialog
void PowerTabEditor::OpenPreferences()
{
    preferencesDialog->exec();
}

// Redraws the whole score of the current document
void PowerTabEditor::RefreshCurrentDocument()
{
    ScoreArea* currentDoc = reinterpret_cast<ScoreArea *>(tabWidget->currentWidget());
    currentDoc->RenderDocument();
}

// Close a document and clean up
void PowerTabEditor::closeTab(int index)
{
    documentManager.Remove(index);
    delete tabWidget->widget(index);
    tabWidget->removeTab(index);

    mixerList->removeWidget(mixerList->widget(index));
    mixerList->setCurrentIndex(tabWidget->currentIndex());

    documentManager.setCurrentDocumentIndex(tabWidget->currentIndex());
}

// When the tab is switched, switch the current document in the document manager
void PowerTabEditor::switchTab(int index)
{
    documentManager.setCurrentDocumentIndex(index);
    mixerList->setCurrentIndex(index);

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

void PowerTabEditor::startStopPlayback()
{
    isPlaying = !isPlaying;

    if (isPlaying)
    {
        QSettings settings;
        rtMidiWrapper->initialize(settings.value("midi/preferredPort", 0).toInt());

        playPauseAct->setText(tr("Pause"));

        getCurrentScoreArea()->getCaret()->setPlaybackMode(true);
        previousPositionInStaff.clear();

        moveCaretToStart();

        for (unsigned int j = 0; j < getCurrentScoreArea()->getCaret()->getCurrentSystem()->GetStaffCount(); ++j)
        {
            playNotesAtCurrentPosition(j);
        }
    }
    else
    {
        playPauseAct->setText(tr("Play"));

        for (int j = 0; j < 8; ++j)
        {
            songTimer[j]->stop();
        }

        getCurrentScoreArea()->getCaret()->setPlaybackMode(false);

        QMultiMap<unsigned int, NoteHistory>::iterator i = oldNotes.begin();
        for (; i != oldNotes.end(); ++i)
        {
            rtMidiWrapper->stopNote(i.key(), i.value().pitch);
        }
        oldNotes.clear();
    }
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

void PowerTabEditor::playNotesAtCurrentPosition(int system)
{
    quint32 position_index = previousPositionInStaff.value(system, -1) + 1; // get position
    previousPositionInStaff.insert(system, position_index); // update

    // retrieve the current position
    //qDebug() << "System: " << system << " Position " << position_index;
    Position* position = getCurrentScoreArea()->getCaret()->getCurrentSystem()->GetStaff(system)->GetPosition(0,position_index);

    if (!position) // check for invalid position (caused by invalid position index)
    {
        return;
    }
    // stop all previous notes except for notes that are to be tied
    {
        QList<unsigned int> notesToBeKept;
        notesToBeKept.reserve(position->GetNoteCount());
        for (unsigned int i = 0; i < position->GetNoteCount(); ++i)
        {
            if (position->GetNote(i)->IsTied())
            {
                notesToBeKept.push_back(position->GetNote(i)->GetString());
            }
        }
        QMutableMapIterator<unsigned int, NoteHistory> i(oldNotes);
        while(i.hasNext())
        {
            i.next();
            if (i.key() == (unsigned int)system && !notesToBeKept.contains(i.value().stringNum))
            {
                songTimer[system]->stop();
                rtMidiWrapper->stopNote(system, i.value().pitch);
                i.remove();
            }
        }
    }

    if (!position->IsRest())
    {
        for (unsigned int i = 0; i < position->GetNoteCount(); ++i)
        // loop through all notes in the current position on the current system
        {
            if (!position->GetNote(i)->IsTied())
            {
                Guitar* guitar = getCurrentScoreArea()->document->GetGuitarScore()->GetGuitar(system);
                Note* note = position->GetNote(i);

                unsigned int pitch = guitar->GetTuning().GetNote(note->GetString()) + guitar->GetCapo();
                pitch += note->GetFretNumber();

                rtMidiWrapper->setVolume(system,guitar->GetInitialVolume());
                rtMidiWrapper->setPan(system,guitar->GetPan());
                rtMidiWrapper->setPatch(system,guitar->GetPreset());


                rtMidiWrapper->playNote(system, pitch ,127);
                NoteHistory noteHistory = {pitch, note->GetString()};
                oldNotes.insertMulti(system, noteHistory );
            }
        }
    }

    TempoMarker* tempo_marker = getCurrentScoreArea()->getCaret()->getCurrentScore()->GetTempoMarker(0);
    double tempo = 60.0 / (double)tempo_marker->GetBeatsPerMinute() * 1000.0;

    double duration = position->GetDurationType();
    duration = tempo * 4.0 / duration;
    duration += position->IsDotted() * 0.5 * duration;
    duration += position->IsDoubleDotted() * 0.75 * duration;
    songTimer[system]->start(duration);
}

void PowerTabEditor::playbackSong(int staff)
{
    // make sure all staves are finished before switching to the next system
    bool okayToSwitchStaves = true;
    // the caret should move along with the staff that is furthest along in playback
    quint32 staffWithMaxPosition = 0;

    QMap<quint32, quint32>::const_iterator i = previousPositionInStaff.constBegin();
    while (i != previousPositionInStaff.constEnd())
    {
        Staff* staff = getCurrentScoreArea()->getCaret()->getCurrentSystem()->GetStaff(i.key());
        if (i.value() < staff->GetPositionCount(0)) // check if a staff is not finished
        {
            okayToSwitchStaves = false;
        }
        if (previousPositionInStaff.value(staffWithMaxPosition) <= i.value())
        {
            staffWithMaxPosition = i.key();
        }
        ++i;
    }

    // only advance the caret if we are in the staff that is furthest along
    if(previousPositionInStaff.value(staff) == previousPositionInStaff.value(staffWithMaxPosition))
    {
        if (!moveCaretRight())
        {
            if (!moveCaretToNextSection() && okayToSwitchStaves)
            {
                this->startStopPlayback();
                return;
            }
            else
            {
                previousPositionInStaff.clear();
                // once we move to a new system, start playing all each of the staves for that system
                for (unsigned int j = 0; j < getCurrentScoreArea()->getCaret()->getCurrentSystem()->GetStaffCount(); ++j)
                {
                    playNotesAtCurrentPosition(j);
                }
                return;
            }
        }
    }

    // play next note for this staff
    playNotesAtCurrentPosition(staff);
}
