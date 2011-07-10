#ifndef POWERTABEDITOR_H
#define POWERTABEDITOR_H

#include <QMainWindow>
#include <QAction>

#include <memory>

// the sigfwd library allows for connecting Qt signals directly to
// C++ functions & functors, and supports boost::bind for binding arguments to slots
#include <boost/bind.hpp>
#include <sigfwd/sigfwd.hpp>

#include <boost/scoped_ptr.hpp>
#include <actions/undomanager.h>
#include <actions/toggleproperty.h>

class QTabWidget;
class ScoreArea;
class QStackedWidget;
class SkinManager;
class PreferencesDialog;
class Toolbox;
class QSplitter;
class MidiPlayer;
class QActionGroup;
class QEvent;
class Note;
class Position;
class DocumentManager;
class FileFormatManager;
class FileFormat;
class Command;

class PowerTabEditor : public QMainWindow
{
    Q_OBJECT

    friend class NotePage;
    friend class ScorePage;

public:
    PowerTabEditor(QWidget *parent = 0);
    ~PowerTabEditor();
    static boost::scoped_ptr<UndoManager> undoManager;
    static ScoreArea* getCurrentScoreArea();
    static std::vector<Position*> getSelectedPositions();
    static std::vector<Note*> getSelectedNotes();

protected:
    void createActions();
    void createMenus();
    void initFileFormatMenus();
    void createTabArea();
    void updateScoreAreaActions(bool disable);
    bool eventFilter(QObject *obj, QEvent *ev);
    void changePositionSpacing(int offset);
    void performSystemInsert(size_t index);
    int getCurrentPlaybackSpeed() const;

    void editSlideInto(uint8_t newSlideIntoType);
    void cycleTab(int offset);
    void editSlideOutOf(uint8_t newSlideType);
    void shiftTabNumber(int direction);
    void updateNoteDuration(uint8_t duration);
    void editRest(uint8_t duration);

    void importFile(const FileFormat& format);

protected slots:
    void updateActions();
    void updateModified(bool);
    void createNewFile();
    void openFile();
    void saveFileAs();
    void openPreferences();
    void refreshOnUndoRedo(int);
    void closeCurrentTab();
    void closeTab(int index);
    void switchTab(int index);
    void startStopPlayback();
    bool moveCaretRight();
    bool moveCaretLeft();
    void moveCaretDown();
    void moveCaretUp();
    void moveCaretToStart();
    void moveCaretToEnd();
    bool moveCaretToPosition(quint8 position);
    bool moveCaretToSystem(quint32 system);

    bool moveCaretToNextStaff();
    bool moveCaretToPrevStaff();

    void moveCaretToFirstSection();
    bool moveCaretToNextSection();
    bool moveCaretToPrevSection();
    void moveCaretToLastSection();
    
    void moveCaretToNextBar();
    void moveCaretToPrevBar();

    void editChordName();
    void editRehearsalSign();
    void editRepeatEnding();
    void editTiedNote();
    void editHammerPull();
    void editTrill();
    void editBarline(int position = -1);
    void editTappedHarmonic();
    void editKeySignature();
    void editTimeSignature();

    void increasePositionSpacing();
    void decreasePositionSpacing();

    void removeCurrentSystem();
    void insertSystemAtEnd();
    void insertSystemBefore();
    void insertSystemAfter();

    void shiftForward();
    void shiftBackward();

    void clearNote();
    void clearCurrentPosition();

    void addGuitar();

    void editKeyboardShortcuts();

protected:
    bool isPlaying;
    const QString fileFilter;

    QString getApplicationName() const;
    void setupNewDocument();
    void updatePreviousDirectory(const QString& fileName);

    static QTabWidget* tabWidget;
    
    Toolbox* toolBox;
    QSplitter* vertSplitter;
    QSplitter* horSplitter;

    boost::scoped_ptr<DocumentManager> documentManager;
    boost::scoped_ptr<FileFormatManager> fileFormatManager;

    QMenu* fileMenu;
    QAction* newFileAct;
    QAction* openFileAct;
    QAction* closeTabAct;
    QAction* saveFileAsAct;
    Command* editShortcutsAct;
    QAction* preferencesAct;
    QMenu* importFileMenu;
    QMenu* exportFileMenu;
    QAction* exitAppAct;

    QMenu* editMenu;
    QAction* undoAct;
    QAction* redoAct;

    QMenu* playbackMenu;
    QAction* playPauseAct;

    QMenu* positionMenu;
    QMenu* positionSectionMenu; // menu options for navigating between sections
    // navigate to the first, next, previous, or last section in the score
    QAction* firstSectionAct;
    QAction* nextSectionAct;
    QAction* prevSectionAct;
    QAction* lastSectionAct;
    QAction* shiftForwardAct;
    QAction* shiftBackwardAct;
    QAction* clearNoteAct; // clears the active note
    QAction* clearCurrentPositionAct; // clears out the entire position (either notes or barline)

    QMenu* positionStaffMenu;
    QAction* nextPositionAct; // navigate to the next position in the staff
    QAction* prevPositionAct; // navigate to the previous position in the staff
    QAction* startPositionAct; // navigate to the first position in the staff
    QAction* lastPositionAct; // navigate to the last position in the staff
    QAction* nextStringAct; // navigate to the next string in the staff
    QAction* prevStringAct; // navigate to the previous string in the staff
    QAction* nextStaffAct; // navigate to the next staff in the system
    QAction* prevStaffAct; // navigate to the previous staff in the system
    QAction* nextBarAct; // navigate to the first non-bar pos in the next bar
    QAction* prevBarAct; // navigate to the first non-bar pos in the prev bar

    QAction* shiftTabNumUp; // shift tab numbers up/down by a string
    QAction* shiftTabNumDown;

    QMenu* textMenu;
    QAction* chordNameAct; // add/remove a chord name

    QMenu* sectionMenu;
    QAction* increasePositionSpacingAct;
    QAction* decreasePositionSpacingAct;
    QAction* insertSystemAtEndAct;
    QAction* insertSystemBeforeAct;
    QAction* insertSystemAfterAct;
    QAction* removeCurrentSystemAct;

    QMenu* notesMenu;
    QActionGroup* noteDurationActGroup; // only one duration can be checked at a time
    QAction* wholeNoteAct; // actions for modifying the duration of a note
    QAction* halfNoteAct;
    QAction* quarterNoteAct;
    QAction* eighthNoteAct;
    QAction* sixteenthNoteAct;
    QAction* thirtySecondNoteAct;
    QAction* sixtyFourthNoteAct;
    QAction* dottedNoteAct; // sets a note to be dotted
    QAction* doubleDottedNoteAct;
    QAction* tiedNoteAct; // sets a note to be tied to the previous note
    QAction* noteMutedAct; // sets the note to be muted
    QAction* ghostNoteAct; // set a note to be a ghost note
    QAction* letRingAct;
    QAction* fermataAct; // set a position as fermata
    QAction* graceNoteAct;
    QAction* staccatoNoteAct; // set the notes of a position to be staccato
    QAction* marcatoAct; // set an accent
    QAction* sforzandoAct; // set a heavy accent

    QMenu* octaveMenu;
    QAction* octave8vaAct;
    QAction* octave15maAct;
    QAction* octave8vbAct;
    QAction* octave15mbAct;

    QMenu* restsMenu;
    QActionGroup* restDurationsGroup; // allow only one rest duration to be selected at a time
    QAction* wholeRestAct;
    QAction* halfRestAct;
    QAction* quarterRestAct;
    QAction* eighthRestAct;
    QAction* sixteenthRestAct;
    QAction* thirtySecondRestAct;
    QAction* sixtyFourthRestAct;

    QMenu* musicSymbolsMenu;
    Command* rehearsalSignAct; // add/remove rehearsal signs
    Command* keySignatureAct;
    QAction* timeSignatureAct;
    QAction* barlineAct;
    QAction* repeatEndingAct;

    QMenu* tabSymbolsMenu;
    QAction* hammerPullAct; // add/remove hammer-on or pull-off
    QAction* naturalHarmonicAct; // add/remove natural harmonics
    QAction* tappedHarmonicAct;
    QAction* shiftSlideAct;
    QAction* legatoSlideAct;
    QAction* vibratoAct; // add/remove vibrato
    QAction* wideVibratoAct;
    QAction* palmMuteAct; // toggle palm-muting
    QAction* tremoloPickingAct; // toggle tremolo picking
    QAction* arpeggioUpAct;
    QAction* arpeggioDownAct;
    QAction* tapAct;
    QAction* trillAction; // create or remove a trill
    QAction* pickStrokeUpAct;
    QAction* pickStrokeDownAct;

    QMenu* slideIntoMenu;
    QAction* slideIntoFromAboveAct;
    QAction* slideIntoFromBelowAct;

    QMenu* slideOutOfMenu;
    QAction* slideOutOfDownwardsAct;
    QAction* slideOutOfUpwardsAct;

    QMenu* guitarMenu;
    QAction* addGuitarAct;

    QMenu* windowMenu;
    QAction* nextTabAct; // cycle to the next/previous tab
    QAction* prevTabAct;

    QString previousDirectory; // previous directory that a file was opened in
    boost::scoped_ptr<QStackedWidget> mixerList;
    boost::scoped_ptr<QStackedWidget> playbackToolbarList;

    std::shared_ptr<SkinManager> skinManager;
    boost::scoped_ptr<MidiPlayer> midiPlayer;

private:
    /// helper function for connecting an action to the performToggleProperty slot
    /// @param objectsGetter - function for retrieving the objects to be edited
    /// @param propertyGetter - function for checking if an object has a certain property set
    /// @param propertySetter - function for toggling the property
    template <typename T>
    void connectToggleProperty(QAction* action,
                               std::function<std::vector<T*> (void)> objectsGetter,
                               std::function<bool (const T*)> propertyGetter,
                               std::function<bool (T*, bool)> propertySetter)
    {
        sigfwd::connect(action, SIGNAL(triggered()),
                        boost::bind(&PowerTabEditor::performToggleProperty<T>, this,
                                    // wrap using boost::bind so that the function is evaluated later (we want the currently
                                    // selected objects, not the objects that were selected when the signal was connected)
                                    boost::bind(objectsGetter),
                                    propertySetter,
                                    propertyGetter,
                                    action->text()));
    }

    /// Creates a new ToggleProperty<T> action, forwarding the parameters to its constructor
    template <typename T>
    void performToggleProperty(const std::vector<T*>& objects, std::function<bool (T*, bool)> setPropertyFn,
                               std::function<bool (const T*)> getPropertyFn, const QString& propertyName)
    {
        undoManager->push(new ToggleProperty<T>(objects, setPropertyFn, getPropertyFn, propertyName));
    }
};

#endif // POWERTABEDITOR_H
