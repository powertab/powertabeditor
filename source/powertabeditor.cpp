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
#include <QKeyEvent>
#include <QEvent>

#include <powertabeditor.h>
#include <scorearea.h>
#include <skinmanager.h>
#include <midiplayer.h>

#include <dialogs/preferencesdialog.h>
#include <dialogs/chordnamedialog.h>
#include <dialogs/rehearsalsigndialog.h>
#include <dialogs/trilldialog.h>
#include <dialogs/barlinedialog.h>

#include <powertabdocument/powertabdocument.h>
#include <powertabdocument/guitar.h>
#include <powertabdocument/system.h>
#include <powertabdocument/chordtext.h>
#include <powertabdocument/score.h>

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

using std::shared_ptr;

QTabWidget* PowerTabEditor::tabWidget = NULL;
std::unique_ptr<UndoManager> PowerTabEditor::undoManager(new UndoManager);

PowerTabEditor::PowerTabEditor(QWidget *parent) :
    QMainWindow(parent),
    fileFilter("Power Tab Documents (*.ptb)"),
    mixerList(new QStackedWidget),
    skinManager(new SkinManager("default"))
{
    this->setWindowIcon(QIcon(":icons/app_icon.png"));

    // load fonts from the resource file
    QFontDatabase::addApplicationFont(":fonts/emmentaler-13.otf"); // used for music notation
    QFontDatabase::addApplicationFont(":fonts/LiberationSans-Regular.ttf"); // used for tab notes

    // set app information
    QCoreApplication::setOrganizationName("Power Tab");
    QCoreApplication::setApplicationName("Power Tab Editor");
    QCoreApplication::setApplicationVersion("2.0");

    // load application settings
    QSettings settings;
    // retrieve the previous directory that a file was opened/saved to (default value is home directory)
    previousDirectory = settings.value("app/previousDirectory", QDir::homePath()).toString();

    connect(undoManager.get(), SIGNAL(indexChanged(int)), this, SLOT(refreshOnUndoRedo(int)));
    connect(undoManager.get(), SIGNAL(cleanChanged(bool)), this, SLOT(updateModified(bool)));

    createActions();
    createMenus();
    createTabArea();

    isPlaying = false;

    setMinimumSize(800, 600);
    setWindowState(Qt::WindowMaximized);
    setWindowTitle(getApplicationName());

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
void PowerTabEditor::refreshOnUndoRedo(int index)
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

// this is a reimplementation of QObject's eventFilter function
// it returns true to tell Qt to stop propagating this event
// this is necessary to intercept tab key presses before they are
// used to cycle focus to different widgets
bool PowerTabEditor::eventFilter(QObject *object, QEvent *event)
{
    ScoreArea* currentDoc = getCurrentScoreArea();

    if (object == currentDoc && event->type() == QEvent::KeyPress)
    {
        QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);
        if ((keyEvent->key() == Qt::Key_Tab) &&
            (keyEvent->modifiers() == Qt::NoModifier))
        {
            moveCaretToNextBar();
            return true;
        }
        else if ((keyEvent->key() == Qt::Key_Backtab) ||
                 (keyEvent->key() == Qt::Key_Tab &&
                  keyEvent->modifiers() == Qt::ShiftModifier))
        {
            moveCaretToPrevBar();
            return true;
        }
        else if ((keyEvent->key() >= Qt::Key_0) &&
                 (keyEvent->key() <= Qt::Key_9))
        {
            // this arithmetic and this condition assume that Key_0 ... Key_9
            // are assigned ascending continuous numbers as they are in Qt 4.7
            int typedNumber = keyEvent->key() - Qt::Key_0;
            Caret *caret = getCurrentScoreArea()->getCaret();
            Note *currentNote = caret->getCurrentNote();
            Position *currentPosition = caret->getCurrentPosition();
            Staff *currentStaff = caret->getCurrentStaff();

            if (currentNote != NULL)
            {
                // if there is already a number here update it
                undoManager->push(new UpdateTabNumber(typedNumber, currentNote, currentPosition, currentStaff));
            }
            else
            {
                std::cout << "Insert tab number not yet implemented!" << std::endl;
            }
        }
    }
    return QMainWindow::eventFilter(object, event);
}

void PowerTabEditor::createActions()
{
    slideOutMapper = new QSignalMapper(this);
    connect(slideOutMapper, SIGNAL(mapped(int)), this, SLOT(editSlide(int)));

    // File-related actions
    newFileAct = new QAction(tr("&New"), this);
    newFileAct->setShortcuts(QKeySequence::New);
    connect(newFileAct, SIGNAL(triggered()), this, SLOT(createNewFile()));

    openFileAct = new QAction(tr("&Open..."), this);
    openFileAct->setShortcuts(QKeySequence::Open);
    openFileAct->setStatusTip(tr("Open an existing document"));
    connect(openFileAct, SIGNAL(triggered()), this, SLOT(openFile()));

    closeTabAct = new QAction(tr("&Close Tab"), this);
    closeTabAct->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_W));
    connect(closeTabAct, SIGNAL(triggered()), this, SLOT(closeCurrentTab()));

    saveFileAsAct = new QAction(tr("Save As..."), this);
    saveFileAsAct->setShortcut(QKeySequence::SaveAs);
    connect(saveFileAsAct, SIGNAL(triggered()), this, SLOT(saveFileAs()));

    preferencesAct = new QAction(tr("&Preferences..."), this);
    preferencesAct->setShortcuts(QKeySequence::Preferences);
    connect(preferencesAct, SIGNAL(triggered()), this, SLOT(openPreferences()));

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

    shiftForwardAct = new QAction(tr("Shift Forward"), this);
    connect(shiftForwardAct, SIGNAL(triggered()), this, SLOT(shiftForward()));

    shiftBackwardAct = new QAction(tr("Shift Backward"), this);
    connect(shiftBackwardAct, SIGNAL(triggered()), this, SLOT(shiftBackward()));

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

    // the shortcuts for these two won't do anything since the tabs get 
    // sucked up by our event filter first
    nextBarAct = new QAction(tr("Next Bar"), this);
    nextBarAct->setShortcut(QKeySequence(Qt::Key_Tab));
    connect(nextBarAct, SIGNAL(triggered()), this, SLOT(moveCaretToNextBar()));
    
    prevBarAct = new QAction(tr("Previous Bar"), this);
    prevBarAct->setShortcut(QKeySequence(Qt::SHIFT + Qt::Key_Tab));
    connect(prevBarAct, SIGNAL(triggered()), this, SLOT(moveCaretToPrevBar()));

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

    clearNoteAct = new QAction(tr("Clear Note"), this);
    clearNoteAct->setShortcut(QKeySequence::Delete);
    connect(clearNoteAct, SIGNAL(triggered()), this, SLOT(clearNote()));

    clearCurrentPositionAct = new QAction(tr("Clear Position"), this);
    clearCurrentPositionAct->setShortcut(QKeySequence::DeleteEndOfWord);
    connect(clearCurrentPositionAct, SIGNAL(triggered()), this, SLOT(clearCurrentPosition()));

    // Text-related actions
    chordNameAct = new QAction(tr("Chord Name..."), this);
    chordNameAct->setShortcut(QKeySequence(Qt::Key_C));
    chordNameAct->setCheckable(true);
    connect(chordNameAct, SIGNAL(triggered()), this, SLOT(editChordName()));

    // Section-related actions
    increasePositionSpacingAct = new QAction(tr("Increase Position Spacing"), this);
    increasePositionSpacingAct->setShortcut(QKeySequence(Qt::Key_Plus));
    connect(increasePositionSpacingAct, SIGNAL(triggered()), this, SLOT(increasePositionSpacing()));

    decreasePositionSpacingAct = new QAction(tr("Decrease Position Spacing"), this);
    decreasePositionSpacingAct->setShortcut(QKeySequence(Qt::Key_Minus));
    connect(decreasePositionSpacingAct, SIGNAL(triggered()), this, SLOT(decreasePositionSpacing()));

    removeCurrentSystemAct = new QAction(tr("Remove Current System"), this);
    connect(removeCurrentSystemAct, SIGNAL(triggered()), this, SLOT(removeCurrentSystem()));

    insertSystemAtEndAct = new QAction(tr("Insert System At End"), this);
    insertSystemAtEndAct->setShortcut(QKeySequence(Qt::Key_N));
    connect(insertSystemAtEndAct, SIGNAL(triggered()), this, SLOT(insertSystemAtEnd()));

    insertSystemBeforeAct = new QAction(tr("Insert System Before"), this);
    connect(insertSystemBeforeAct, SIGNAL(triggered()), this, SLOT(insertSystemBefore()));

    insertSystemAfterAct = new QAction(tr("Insert System After"), this);
    connect(insertSystemAfterAct, SIGNAL(triggered()), this, SLOT(insertSystemAfter()));

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

    togglePropertyMapper = new QSignalMapper(this);

    dottedNoteAct = new QAction(tr("Dotted"), this);
    dottedNoteAct->setCheckable(true);
    connectTogglePropertyAction(dottedNoteAct, ToggleablePropertyRecord<Position>
                                ( &getSelectedPositions, &Position::IsDotted, &Position::SetDotted, dottedNoteAct->text() ));

    doubleDottedNoteAct = new QAction(tr("Double Dotted"), this);
    doubleDottedNoteAct->setCheckable(true);
    connectTogglePropertyAction(doubleDottedNoteAct, ToggleablePropertyRecord<Position>
                                ( &getSelectedPositions, &Position::IsDoubleDotted, &Position::SetDoubleDotted, doubleDottedNoteAct->text() ));

    tiedNoteAct = new QAction(tr("Tied"), this);
    tiedNoteAct->setShortcut(QKeySequence(Qt::Key_Y));
    tiedNoteAct->setCheckable(true);
    connect(tiedNoteAct, SIGNAL(triggered()), this, SLOT(editTiedNote()));
    
    noteMutedAct = new QAction(tr("Muted"), this);
    noteMutedAct->setCheckable(true);
    connectTogglePropertyAction(noteMutedAct, ToggleablePropertyRecord<Note>
                                ( &getSelectedNotes, &Note::IsMuted, &Note::SetMuted, noteMutedAct->text() ));

    ghostNoteAct = new QAction(tr("Ghost Note"), this);
    ghostNoteAct->setShortcut(QKeySequence(Qt::Key_N));
    ghostNoteAct->setCheckable(true);
    connectTogglePropertyAction(ghostNoteAct, ToggleablePropertyRecord<Note>
                                  ( &getSelectedNotes, &Note::IsGhostNote, &Note::SetGhostNote, ghostNoteAct->text() ));

    fermataAct = new QAction(tr("Fermata"), this);
    fermataAct->setShortcut(QKeySequence(Qt::Key_F));
    fermataAct->setCheckable(true);
    connectTogglePropertyAction(fermataAct, ToggleablePropertyRecord<Position>
                                   ( &getSelectedPositions, &Position::HasFermata, &Position::SetFermata, fermataAct->text() ));

    letRingAct = new QAction(tr("Let Ring"), this);
    letRingAct->setCheckable(true);
    connectTogglePropertyAction(letRingAct, ToggleablePropertyRecord<Position>
                                ( &getSelectedPositions, &Position::HasLetRing, &Position::SetLetRing, letRingAct->text() ));
    
    staccatoNoteAct = new QAction(tr("Staccato"), this);
    staccatoNoteAct->setCheckable(true);
    staccatoNoteAct->setShortcut(QKeySequence(Qt::Key_Z));
    connectTogglePropertyAction(staccatoNoteAct, ToggleablePropertyRecord<Position>
                                  ( &getSelectedPositions, &Position::IsStaccato, &Position::SetStaccato, staccatoNoteAct->text() ));

    marcatoAct = new QAction(tr("Accent"), this);
    marcatoAct->setCheckable(true);
    marcatoAct->setShortcut(QKeySequence(Qt::Key_A));
    connectTogglePropertyAction(marcatoAct, ToggleablePropertyRecord<Position>
                                  ( &getSelectedPositions, &Position::HasMarcato, &Position::SetMarcato, marcatoAct->text() ));

    sforzandoAct = new QAction(tr("Heavy Accent"), this);
    sforzandoAct->setCheckable(true);
    connectTogglePropertyAction(sforzandoAct, ToggleablePropertyRecord<Position>
                                  ( &getSelectedPositions, &Position::HasSforzando, &Position::SetSforzando, sforzandoAct->text() ));
    
    // Music Symbol Actions
    rehearsalSignAct = new QAction(tr("Rehearsal Sign..."), this);
    rehearsalSignAct->setCheckable(true);
    rehearsalSignAct->setShortcut(QKeySequence(Qt::SHIFT + Qt::Key_R));
    connect(rehearsalSignAct, SIGNAL(triggered()), this, SLOT(editRehearsalSign()));

    barlineAct = new QAction(tr("Barline..."), this);
    barlineAct->setCheckable(true);
    barlineAct->setShortcut(QKeySequence(Qt::Key_B));
    connect(barlineAct, SIGNAL(triggered()), this, SLOT(editBarline()));

    // Tab Symbol Actions
    hammerPullAct = new QAction(tr("Hammer On/Pull Off"), this);
    hammerPullAct->setCheckable(true);
    hammerPullAct->setShortcut(QKeySequence(Qt::Key_H));
    connect(hammerPullAct, SIGNAL(triggered()), this, SLOT(editHammerPull()));
    
    naturalHarmonicAct = new QAction(tr("Natural Harmonic"), this);
    naturalHarmonicAct->setCheckable(true);
    connectTogglePropertyAction(naturalHarmonicAct, ToggleablePropertyRecord<Note>
                                  ( &getSelectedNotes, &Note::IsNaturalHarmonic,
                                   &Note::SetNaturalHarmonic, naturalHarmonicAct->text() ));

    shiftSlideAct = new QAction(tr("Shift Slide"), this);
    shiftSlideAct->setCheckable(true);
    shiftSlideAct->setShortcut(QKeySequence(Qt::Key_S));
    connect(shiftSlideAct, SIGNAL(triggered()), slideOutMapper, SLOT(map()));
    slideOutMapper->setMapping(shiftSlideAct, Note::slideOutOfShiftSlide);

    legatoSlideAct = new QAction(tr("Legato Slide"), this);
    legatoSlideAct->setCheckable(true);
    legatoSlideAct->setShortcut(QKeySequence(Qt::Key_L));
    connect(legatoSlideAct, SIGNAL(triggered()), slideOutMapper, SLOT(map()));
    slideOutMapper->setMapping(legatoSlideAct, Note::slideOutOfLegatoSlide);

    vibratoAct = new QAction(tr("Vibrato"), this);
    vibratoAct->setCheckable(true);
    vibratoAct->setShortcut(QKeySequence(Qt::Key_V));
    connectTogglePropertyAction(vibratoAct, ToggleablePropertyRecord<Position>
                                   ( &getSelectedPositions, &Position::HasVibrato,
                                    &Position::SetVibrato, vibratoAct->text() ));

    wideVibratoAct = new QAction(tr("Wide Vibrato"), this);
    wideVibratoAct->setCheckable(true);
    wideVibratoAct->setShortcut(QKeySequence(Qt::Key_W));
    connectTogglePropertyAction(wideVibratoAct, ToggleablePropertyRecord<Position>
                                   ( &getSelectedPositions, &Position::HasWideVibrato,
                                    &Position::SetWideVibrato, wideVibratoAct->text() ));

    palmMuteAct = new QAction(tr("Palm Mute"), this);
    palmMuteAct->setCheckable(true);
    palmMuteAct->setShortcut(QKeySequence(Qt::Key_M));
    connectTogglePropertyAction(palmMuteAct, ToggleablePropertyRecord<Position>
                                   ( &getSelectedPositions, &Position::HasPalmMuting,
                                    &Position::SetPalmMuting, palmMuteAct->text() ));

    tremoloPickingAct = new QAction(tr("Tremolo Picking"), this);
    tremoloPickingAct->setCheckable(true);
    connectTogglePropertyAction(tremoloPickingAct, ToggleablePropertyRecord<Position>
                                   ( &getSelectedPositions, &Position::HasTremoloPicking,
                                    &Position::SetTremoloPicking, tremoloPickingAct->text() ));

    arpeggioUpAct = new QAction(tr("Arpeggio Up"), this);
    arpeggioUpAct->setCheckable(true);
    connectTogglePropertyAction(arpeggioUpAct, ToggleablePropertyRecord<Position>
                                ( &getSelectedPositions, &Position::HasArpeggioUp,
                                 &Position::SetArpeggioUp, arpeggioUpAct->text() ));

    arpeggioDownAct = new QAction(tr("Arpeggio Down"), this);
    arpeggioDownAct->setCheckable(true);
    connectTogglePropertyAction(arpeggioDownAct, ToggleablePropertyRecord<Position>
                                ( &getSelectedPositions, &Position::HasArpeggioDown,
                                 &Position::SetArpeggioDown, arpeggioUpAct->text() ));

    tapAct = new QAction(tr("Tap"), this);
    tapAct->setCheckable(true);
    tapAct->setShortcut(QKeySequence(Qt::Key_P));
    connectTogglePropertyAction(tapAct, ToggleablePropertyRecord<Position>
                                ( &getSelectedPositions, &Position::HasTap,
                                 &Position::SetTap, tapAct->text() ));

    trillAction = new QAction(tr("Trill"), this);
    trillAction->setCheckable(true);
    connect(trillAction, SIGNAL(triggered()), this, SLOT(editTrill()));

    pickStrokeUpAct = new QAction(tr("Pickstroke Up"), this);
    pickStrokeUpAct->setCheckable(true);
    connectTogglePropertyAction(pickStrokeUpAct, ToggleablePropertyRecord<Position>
                                ( &getSelectedPositions, &Position::HasPickStrokeUp,
                                 &Position::SetPickStrokeUp, pickStrokeUpAct->text() ));

    pickStrokeDownAct = new QAction(tr("Pickstroke Down"), this);
    pickStrokeDownAct->setCheckable(true);
    connectTogglePropertyAction(pickStrokeDownAct, ToggleablePropertyRecord<Position>
                                ( &getSelectedPositions, &Position::HasPickStrokeDown,
                                 &Position::SetPickStrokeDown, pickStrokeDownAct->text() ));

    // Slide Into Menu
    slideIntoFromAboveAct = new QAction(tr("Slide Into From Above"), this);
    slideIntoFromAboveAct->setCheckable(true);

    slideIntoFromBelowAct = new QAction(tr("Slide Into From Below"), this);
    slideIntoFromBelowAct->setCheckable(true);

    // Slide Out Of Menu
    slideOutOfDownwardsAct = new QAction(tr("Slide Out Of Downwards"), this);
    slideOutOfDownwardsAct->setCheckable(true);
    connect(slideOutOfDownwardsAct, SIGNAL(triggered()), slideOutMapper, SLOT(map()));
    slideOutMapper->setMapping(slideOutOfDownwardsAct, Note::slideOutOfDownwards);

    slideOutOfUpwardsAct = new QAction(tr("Slide Out Of Upwards"), this);
    slideOutOfUpwardsAct->setCheckable(true);
    connect(slideOutOfUpwardsAct, SIGNAL(triggered()), slideOutMapper, SLOT(map()));
    slideOutMapper->setMapping(slideOutOfUpwardsAct, Note::slideOutOfUpwards);

    // Guitar Menu
    addGuitarAct = new QAction(tr("Add Guitar"), this);
    connect(addGuitarAct, SIGNAL(triggered()), this, SLOT(addGuitar()));

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
    
    connect(togglePropertyMapper, SIGNAL(mapped(int)), this, SLOT(editProperty(int)));
}

/// This function simplifies the process of initializing a QAction for a ToggleableProperty.
/// Connects and maps the QAction to the QSignalMapper, and adds the propertyInfo to the toggleableProperties list
void PowerTabEditor::connectTogglePropertyAction(QAction* action, const ToggleableProperty& propertyInfo)
{
    Q_ASSERT(togglePropertyMapper != NULL);

    connect(action, SIGNAL(triggered()), togglePropertyMapper, SLOT(map()));

    toggleableProperties.push_back(propertyInfo);
    togglePropertyMapper->setMapping(action, toggleableProperties.size() - 1);
}

void PowerTabEditor::createMenus()
{
    // File Menu
    fileMenu = menuBar()->addMenu(tr("&File"));
    fileMenu->addAction(newFileAct);
    fileMenu->addAction(openFileAct);
    fileMenu->addAction(closeTabAct);
    fileMenu->addSeparator();
    fileMenu->addAction(saveFileAsAct);
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
    positionStaffMenu->addAction(nextBarAct);
    positionStaffMenu->addAction(prevBarAct);

    positionMenu->addSeparator();
    positionMenu->addAction(shiftTabNumUp);
    positionMenu->addAction(shiftTabNumDown);
    positionMenu->addSeparator();
    positionMenu->addAction(shiftForwardAct);
    positionMenu->addAction(shiftBackwardAct);
    positionMenu->addSeparator();
    positionMenu->addAction(clearNoteAct);
    positionMenu->addAction(clearCurrentPositionAct);

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
    notesMenu->addAction(dottedNoteAct);
    notesMenu->addAction(doubleDottedNoteAct);
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
    notesMenu->addAction(staccatoNoteAct);
    notesMenu->addAction(marcatoAct);
    notesMenu->addAction(sforzandoAct);

    // Music Symbols Menu
    musicSymbolsMenu = menuBar()->addMenu(tr("&Music Symbols"));
    musicSymbolsMenu->addAction(rehearsalSignAct);
    musicSymbolsMenu->addAction(barlineAct);

    // Tab Symbols Menu
    tabSymbolsMenu = menuBar()->addMenu(tr("&Tab Symbols"));
    tabSymbolsMenu->addAction(hammerPullAct);
    tabSymbolsMenu->addAction(naturalHarmonicAct);
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

    // Window Menu
    windowMenu = menuBar()->addMenu(tr("&Window"));
    windowMenu->addAction(nextTabAct);
    windowMenu->addAction(prevTabAct);
}

void PowerTabEditor::createTabArea()
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

// Create a blank file
void PowerTabEditor::createNewFile()
{
    documentManager.createDocument();
    setupNewDocument();
}

// Open a new file
void PowerTabEditor::openFile()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open"), previousDirectory, fileFilter);

    if (fileName.isEmpty())
    {
        qDebug() << "No file selected";
    }
    else
    {
        qDebug() << "Opening file: " << fileName;

        // add the file to the document manager; draw the score and initialize if successful
        if (documentManager.addDocument(fileName))
        {
            QFileInfo fileInfo(fileName);
            // save this as the previous directory
            previousDirectory = fileInfo.absolutePath();
            QSettings settings;
            settings.setValue("app/previousDirectory", previousDirectory);

            setupNewDocument();
        }
    }
}

/// Creates the scorearea, mixer, etc, for a new document
/// Assumes that the new document has already been added to the document manager,
/// and is set as the current document
void PowerTabEditor::setupNewDocument()
{
    std::shared_ptr<PowerTabDocument> doc(documentManager.getCurrentDocument());

    ScoreArea* score = new ScoreArea;
    score->installEventFilter(this);
    score->renderDocument(doc);

    connect(score->getCaret(), SIGNAL(moved()), this, SLOT(updateActions()));
    connect(score, SIGNAL(barlineClicked(int)), this, SLOT(editBarline(int)));

    undoManager->addNewUndoStack();

    QFileInfo fileInfo(QString().fromStdString(doc->GetFileName()));

    // create title for the tab bar
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

    int tabIndex = tabWidget->addTab(score, title);
    tabWidget->setTabToolTip(tabIndex, fileInfo.fileName());

    // add the guitars to a new mixer
    Mixer* mixer = new Mixer(skinManager);
    QScrollArea* scrollArea = new QScrollArea;
    for (quint32 i=0; i < doc->GetGuitarScore()->GetGuitarCount(); i++)
    {
        mixer->addInstrument(doc->GetGuitarScore()->GetGuitar(i));
    }
    scrollArea->setWidget(mixer);
    mixerList->addWidget(scrollArea);

    // switch to the new document
    tabWidget->setCurrentIndex(documentManager.getCurrentDocumentIndex());

     // if this is the only open document, enable score area actions
    if (documentManager.getCurrentDocumentIndex() == 0)
    {
        updateScoreAreaActions(true);
    }

    updateActions(); // update available actions for the current position
}

void PowerTabEditor::saveFileAs()
{
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save As"), previousDirectory, fileFilter);
    if (!fileName.isEmpty())
    {
        documentManager.getCurrentDocument()->Save(fileName.toStdString());
    }
}

// Opens the preferences dialog
void PowerTabEditor::openPreferences()
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
    documentManager.removeDocument(index);
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

    // update the window title with the file path of the active document
    if(documentManager.getCurrentDocument())
    {
        const QString path = QString::fromStdString(documentManager.getCurrentDocument()->GetFileName());
        const QString docName = QFileInfo(path).fileName();
        setWindowTitle(docName + "[*] - " + getApplicationName()); // need the [*] for using setWindowModified
    }
    else
    {
        setWindowTitle(getApplicationName());
    }
}

/// Marks the window as modified/unmodified depending on the state of the active UndoStack
void PowerTabEditor::updateModified(bool clean)
{
    setWindowModified(!clean);
}

/// Returns the application name & version (e.g. 'Power Tab Editor 2.0')
QString PowerTabEditor::getApplicationName() const
{
    return QCoreApplication::applicationName() + " " + QCoreApplication::applicationVersion();
}

ScoreArea* PowerTabEditor::getCurrentScoreArea()
{
    return dynamic_cast<ScoreArea*>(tabWidget->currentWidget());
}

void PowerTabEditor::getSelectedPositions(std::vector<Position*>& positions)
{
    getCurrentScoreArea()->getCaret()->getSelectedPositions(positions);
}

void PowerTabEditor::getSelectedNotes(std::vector<Note*>& notes)
{
    getCurrentScoreArea()->getCaret()->getSelectedNotes(notes);
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
        connect(midiPlayer.get(), SIGNAL(playbackSystemChanged(quint32)), this, SLOT(moveCaretToSystem(quint32)));
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

void PowerTabEditor::moveCaretToNextBar()
{
    getCurrentScoreArea()->getCaret()->moveCaretToNextBar();
}

void PowerTabEditor::moveCaretToPrevBar()
{
    getCurrentScoreArea()->getCaret()->moveCaretToPrevBar();
}

void PowerTabEditor::increasePositionSpacing()
{
    changePositionSpacing(1);
}

void PowerTabEditor::decreasePositionSpacing()
{
    changePositionSpacing(-1);
}

void PowerTabEditor::changePositionSpacing(int offset)
{
    shared_ptr<System> currentSystem = getCurrentScoreArea()->getCaret()->getCurrentSystem();

    const int newSpacing = currentSystem->GetPositionSpacing() + offset;
    if (currentSystem->IsValidPositionSpacing(newSpacing))
    {
        undoManager->push(new ChangePositionSpacing(currentSystem, newSpacing));
    }
}

void PowerTabEditor::removeCurrentSystem()
{
    Caret* caret = getCurrentScoreArea()->getCaret();

    RemoveSystem* removeSystemAct = new RemoveSystem(caret->getCurrentScore(), caret->getCurrentSystemIndex());
    connect(removeSystemAct, SIGNAL(triggered()), getCurrentScoreArea(), SLOT(requestFullRedraw()));
    undoManager->push(removeSystemAct);
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

void PowerTabEditor::performSystemInsert(size_t index)
{
    Caret* caret = getCurrentScoreArea()->getCaret();
    Score* score = caret->getCurrentScore();

    AddSystem* addSystemAct = new AddSystem(score, index);
    connect(addSystemAct, SIGNAL(triggered()), getCurrentScoreArea(), SLOT(requestFullRedraw()));
    undoManager->push(addSystemAct);
}

void PowerTabEditor::shiftForward()
{
    Caret* caret = getCurrentScoreArea()->getCaret();

    undoManager->push(new PositionShift(caret->getCurrentSystem(),
                                        caret->getCurrentPositionIndex(), PositionShift::SHIFT_FORWARD));
}

void PowerTabEditor::shiftBackward()
{
    Caret* caret = getCurrentScoreArea()->getCaret();

    undoManager->push(new PositionShift(caret->getCurrentSystem(),
                                        caret->getCurrentPositionIndex(), PositionShift::SHIFT_BACKWARD));
}

/// Clears the note at the caret's current position
void PowerTabEditor::clearNote()
{
    Caret* caret = getCurrentScoreArea()->getCaret();
    Note* currentNote = caret->getCurrentNote();

    Q_ASSERT(currentNote != NULL);

    // TODO - implement this
}

/// Completely clears the caret's current position
/// Either removes a barline, or all of the notes at the position
void PowerTabEditor::clearCurrentPosition()
{
    Caret* caret = getCurrentScoreArea()->getCaret();
    shared_ptr<System> system = caret->getCurrentSystem();
    Position* currentPos = caret->getCurrentPosition();
    Barline* currentBar = caret->getCurrentBarline();

    if (currentPos != NULL)
    {
        const quint32 voice = 0; // leaving this here for when we support multiple voices
        undoManager->push(new DeletePosition(caret->getCurrentStaff(), currentPos, voice));
    }
    else if (currentBar != NULL) // remove barline
    {
        undoManager->push(new DeleteBarline(system, currentBar));
    }
}

void PowerTabEditor::addGuitar()
{
    Score* score = getCurrentScoreArea()->getCaret()->getCurrentScore();

    QScrollArea* scrollArea = dynamic_cast<QScrollArea*>(mixerList->currentWidget());
    Mixer* currentMixer = dynamic_cast<Mixer*>(scrollArea->widget());
    Q_ASSERT(currentMixer != NULL);

    AddGuitar* addGuitar = new AddGuitar(score, currentMixer);
    connect(addGuitar, SIGNAL(triggered()), getCurrentScoreArea(), SLOT(requestFullRedraw()));
    undoManager->push(addGuitar);
}

/// Edits or creates a barline.
/// If position is not specified, the caret's current position is used
void PowerTabEditor::editBarline(int position)
{
    Caret* caret = getCurrentScoreArea()->getCaret();
    Barline* barLine = NULL;

    if (position == -1)
    {
         barLine = caret->getCurrentBarline();
    }
    else
    {
        barLine = caret->getCurrentSystem()->GetBarlineAtPosition(position);
    }

    if (barLine != NULL) // edit existing barline
    {
        quint8 type = barLine->GetType(), repeats = barLine->GetRepeatCount();

        BarlineDialog dialog(type, repeats);
        if (dialog.exec() == QDialog::Accepted)
        {
            undoManager->push(new ChangeBarLineType(barLine, type, repeats));
        }
    }
    else // create new barline
    {
        quint8 type = Barline::bar, repeats = Barline::MIN_REPEAT_COUNT;

        BarlineDialog dialog(type, repeats);
        if (dialog.exec() == QDialog::Accepted)
        {
            undoManager->push(new AddBarline(caret->getCurrentSystem(),
                                             caret->getCurrentPositionIndex(), type, repeats));
        }
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

// Add/Remove the trill at the current position
void PowerTabEditor::editTrill()
{
    Note* currentNote = getCurrentScoreArea()->getCaret()->getCurrentNote();

    if (currentNote->HasTrill())
    {
        undoManager->push(new RemoveTrill(currentNote));
    }
    else // add a new trill
    {
        quint8 trillFret = 0;
        TrillDialog trillDialog(currentNote, trillFret);
        if (trillDialog.exec() == QDialog::Accepted)
        {
            undoManager->push(new AddTrill(currentNote, trillFret));
        }
        else
        {
            trillAction->setChecked(false);
        }
    }
}

// If there is a rehearsal sign at the barline, remove it
// If there is no rehearsal sign, show the dialog to add one
void PowerTabEditor::editRehearsalSign()
{
    // Find if there is a rehearsal sign at the current position
    Caret* caret = getCurrentScoreArea()->getCaret();
    Score* currentScore = caret->getCurrentScore();
    Barline* currentBarline = caret->getCurrentBarline();

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

void PowerTabEditor::editHammerPull()
{
    Caret* caret = getCurrentScoreArea()->getCaret();
    Note* note = caret->getCurrentNote();

    // TODO - allow editing of hammerons/pulloffs for a group of selected notes??
    std::vector<Note*> notes;
    notes.push_back(note);
    
    // if 'h' is pressed but there is no note do nothing
    if (note == NULL) return;
    
    Position* currentPosition = caret->getCurrentPosition();
    Staff* currentStaff = caret->getCurrentStaff();
    
    if (currentStaff->CanHammerOn(currentPosition, note))
    {
        undoManager->push(new ToggleProperty<Note>(notes, &Note::SetHammerOn,
                                                   &Note::HasHammerOn, tr("Hammer On")));
    }
    else if (currentStaff->CanPullOff(currentPosition, note))
    {
        undoManager->push(new ToggleProperty<Note>(notes, &Note::SetPullOff,
                                                   &Note::HasPullOff, tr("Pull Off")));
    }
    else
    {
        hammerPullAct->setChecked(false);
    }
}

void PowerTabEditor::editTiedNote()
{
    Caret* caret = getCurrentScoreArea()->getCaret();

    std::vector<Note*> notes;
    getSelectedNotes(notes);

    Staff* currentStaff = caret->getCurrentStaff();

    // check if all selected notes are able to be tied
    std::vector<Position*> positions;
    getSelectedPositions(positions);

    for (size_t i = 0; i < positions.size(); i++)
    {
        Position* currentPosition = positions.at(i);
        for (size_t j = 0; j < currentPosition->GetNoteCount(); j++)
        {
            if (!currentStaff->CanTieNote(currentPosition, currentPosition->GetNote(j)))
            {
                tiedNoteAct->setChecked(false);
                return;
            }
        }
    }

    undoManager->push(new ToggleProperty<Note>(notes, &Note::SetTied, &Note::IsTied, "Note Tie"));
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
    const Score* currentScore = caret->getCurrentScore();
    const quint32 caretPosition = caret->getCurrentPositionIndex();
    shared_ptr<System> currentSystem = caret->getCurrentSystem();
    Position* currentPosition = caret->getCurrentPosition();
    Barline* currentBarline = caret->getCurrentBarline();
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

    if (currentBarline != NULL) // current position is bar
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

    updatePropertyStatus(naturalHarmonicAct, currentNote, &Note::IsNaturalHarmonic);
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

    updatePropertyStatus(trillAction, currentNote, &Note::HasTrill);
    updatePropertyStatus(tapAct, currentPosition, &Position::HasTap);
    updatePropertyStatus(pickStrokeUpAct, currentPosition, &Position::HasPickStrokeUp);
    updatePropertyStatus(pickStrokeDownAct, currentPosition, &Position::HasPickStrokeDown);

    updatePropertyStatus(shiftSlideAct, currentNote, &Note::HasShiftSlide);
    updatePropertyStatus(legatoSlideAct, currentNote, &Note::HasLegatoSlide);
    updatePropertyStatus(slideOutOfDownwardsAct, currentNote, &Note::HasSlideOutOfDownwards);
    updatePropertyStatus(slideOutOfUpwardsAct, currentNote, &Note::HasSlideOutOfUpwards);

    shiftBackwardAct->setEnabled(currentPosition == NULL);

    clearNoteAct->setEnabled(currentNote != NULL);

    removeCurrentSystemAct->setEnabled(currentScore->GetSystemCount() > 1);

    clearCurrentPositionAct->setEnabled(currentPosition != NULL || currentBarline != NULL);

    shiftTabNumDown->setEnabled(currentNote != NULL);
    shiftTabNumUp->setEnabled(currentNote != NULL);
}

// Enables/disables actions that should only be available when a score is opened
void PowerTabEditor::updateScoreAreaActions(bool enable)
{
    QList<QMenu*> menuList;
    menuList << playbackMenu << positionMenu << textMenu << notesMenu << musicSymbolsMenu << tabSymbolsMenu << windowMenu;
    menuList << positionSectionMenu << positionStaffMenu << sectionMenu;

    foreach(QMenu* menu, menuList)
    {
        foreach(QAction* action, menu->actions())
        {
            action->setEnabled(enable);
        }
    }

    closeTabAct->setEnabled(enable);
    saveFileAsAct->setEnabled(enable);
}

void PowerTabEditor::updateNoteDuration(int duration)
{
    Position* currentPosition = getCurrentScoreArea()->getCaret()->getCurrentPosition();
    undoManager->push(new UpdateNoteDuration(currentPosition, duration));
}

void PowerTabEditor::editSlide(int newSlideType)
{
    Caret* caret = getCurrentScoreArea()->getCaret();
    Note* note = caret->getCurrentNote();
    Position* position = caret->getCurrentPosition();

    qint8 newSteps = 0;

    // for shift/legato slides, get the number of steps we will shift
    if (newSlideType == Note::slideOutOfLegatoSlide || newSlideType == Note::slideOutOfShiftSlide)
    {
        Staff* staff = caret->getCurrentStaff();

        // if we can't do a slide, uncheck the action that was just pressed and abort
        if (!staff->CanSlideBetweenNotes(position, note))
        {
            if (newSlideType == Note::slideOutOfLegatoSlide)
                legatoSlideAct->setChecked(false);
            else if (newSlideType == Note::slideOutOfShiftSlide)
                shiftSlideAct->setChecked(false);

            return;
        }

        newSteps = caret->getCurrentStaff()->GetSlideSteps(position, note);
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

    undoManager->push(new EditSlideOut(note, newSlideType, newSteps));
}
