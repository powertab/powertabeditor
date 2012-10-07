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
  
#ifndef POWERTABEDITOR_H
#define POWERTABEDITOR_H

#include <QMainWindow>
#include <QAction>

#include <boost/shared_ptr.hpp>

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
class Toolbox;
class QSplitter;
class MidiPlayer;
class QEvent;
class Note;
class Position;
class DocumentManager;
class FileFormatManager;
struct FileFormat;
class Command;
class RecentFiles;
class Caret;

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
    void registerCaret(Caret* caret);

protected:
    virtual void closeEvent(QCloseEvent*);

private:
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

private slots:
    void updateActions();
    void updateModified(bool);
    void createNewFile();
    void openFile(QString fileName = "");
    bool saveFileAs();
    void openPreferences();
    void openFileInformation();
    void refreshOnUndoRedo(int);
    bool closeCurrentTab();
    bool closeTab(int index);
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
    void editKeySignature(int position = -1);
    void editTimeSignature();
    void editDynamic();
    void editVolumeSwell();
    void editIrregularGrouping(bool setAsTriplet = false);

    void removeCurrentSystem();
    void insertSystemAtEnd();
    void insertSystemBefore();
    void insertSystemAfter();

    void shiftForward();
    void shiftBackward();

    void clearNote();
    void clearCurrentPosition();

    void addGuitar();
    void toggleGuitarVisible(uint32_t trackIndex, bool isVisible);

    void editKeyboardShortcuts();

    void doPaste();
    void copySelectedNotes();

private:
    bool isPlaying;
    const QString fileFilter;

    QString getApplicationName() const;
    void setupNewDocument();
    void updatePreviousDirectory(const QString& fileName);
    void updateWindowTitle();

    static QTabWidget* tabWidget;
    
    Toolbox* toolBox;
    QSplitter* vertSplitter;
    QSplitter* horSplitter;

    boost::scoped_ptr<DocumentManager> documentManager;
    boost::scoped_ptr<FileFormatManager> fileFormatManager;
    RecentFiles* recentFiles;

    QMenu* fileMenu;
    Command* newFileAct;
    Command* openFileAct;
    Command* closeTabAct;
    Command* saveFileAsAct;
    QMenu* recentFilesMenu;
    Command* editShortcutsAct;
    Command* preferencesAct;
    QMenu* importFileMenu;
    QMenu* exportFileMenu;
    Command* exitAppAct;

    QMenu* editMenu;
    QAction* undoAct;
    QAction* redoAct;
    Command* copyAct;
    Command* pasteAct;
    Command* fileInfoAct;

    QMenu* playbackMenu;
    Command* playPauseAct;

    QMenu* positionMenu;
    QMenu* positionSectionMenu; // menu options for navigating between sections
    // navigate to the first, next, previous, or last section in the score
    Command* firstSectionAct;
    Command* nextSectionAct;
    Command* prevSectionAct;
    Command* lastSectionAct;
    Command* shiftForwardAct;
    Command* shiftBackwardAct;
    Command* clearNoteAct; // clears the active note
    Command* clearCurrentPositionAct; // clears out the entire position (either notes or barline)

    QMenu* positionStaffMenu;
    Command* nextPositionAct; // navigate to the next position in the staff
    Command* prevPositionAct; // navigate to the previous position in the staff
    Command* startPositionAct; // navigate to the first position in the staff
    Command* lastPositionAct; // navigate to the last position in the staff
    Command* nextStringAct; // navigate to the next string in the staff
    Command* prevStringAct; // navigate to the previous string in the staff
    Command* nextStaffAct; // navigate to the next staff in the system
    Command* prevStaffAct; // navigate to the previous staff in the system
    Command* nextBarAct; // navigate to the first non-bar pos in the next bar
    Command* prevBarAct; // navigate to the first non-bar pos in the prev bar

    Command* shiftTabNumUp; // shift tab numbers up/down by a string
    Command* shiftTabNumDown;

    QMenu* textMenu;
    Command* chordNameAct; // add/remove a chord name

    QMenu* sectionMenu;
    Command* increasePositionSpacingAct;
    Command* decreasePositionSpacingAct;
    Command* insertSystemAtEndAct;
    Command* insertSystemBeforeAct;
    Command* insertSystemAfterAct;
    Command* removeCurrentSystemAct;

    QMenu* notesMenu;
    QActionGroup* noteDurationActGroup; // only one duration can be checked at a time
    Command* wholeNoteAct; // actions for modifying the duration of a note
    Command* halfNoteAct;
    Command* quarterNoteAct;
    Command* eighthNoteAct;
    Command* sixteenthNoteAct;
    Command* thirtySecondNoteAct;
    Command* sixtyFourthNoteAct;
    Command* dottedNoteAct; // sets a note to be dotted
    Command* doubleDottedNoteAct;
    Command* tiedNoteAct; // sets a note to be tied to the previous note
    Command* noteMutedAct; // sets the note to be muted
    Command* ghostNoteAct; // set a note to be a ghost note
    Command* letRingAct;
    Command* fermataAct; // set a position as fermata
    Command* graceNoteAct;
    Command* staccatoNoteAct; // set the notes of a position to be staccato
    Command* marcatoAct; // set an accent
    Command* sforzandoAct; // set a heavy accent
    Command* tripletAct;
    Command* irregularGroupingAct;

    QMenu* octaveMenu;
    Command* octave8vaAct;
    Command* octave15maAct;
    Command* octave8vbAct;
    Command* octave15mbAct;

    QMenu* restsMenu;
    QActionGroup* restDurationsGroup; // allow only one rest duration to be selected at a time
    Command* wholeRestAct;
    Command* halfRestAct;
    Command* quarterRestAct;
    Command* eighthRestAct;
    Command* sixteenthRestAct;
    Command* thirtySecondRestAct;
    Command* sixtyFourthRestAct;

    QMenu* musicSymbolsMenu;
    Command* rehearsalSignAct; // add/remove rehearsal signs
    Command* keySignatureAct;
    Command* timeSignatureAct;
    Command* barlineAct;
    Command* repeatEndingAct;
    Command* dynamicAct;
    Command* volumeSwellAct;

    QMenu* tabSymbolsMenu;
    Command* hammerPullAct; // add/remove hammer-on or pull-off
    Command* naturalHarmonicAct; // add/remove natural harmonics
    Command* tappedHarmonicAct;
    Command* shiftSlideAct;
    Command* legatoSlideAct;
    Command* vibratoAct; // add/remove vibrato
    Command* wideVibratoAct;
    Command* palmMuteAct; // toggle palm-muting
    Command* tremoloPickingAct; // toggle tremolo picking
    Command* arpeggioUpAct;
    Command* arpeggioDownAct;
    Command* tapAct;
    Command* trillAction; // create or remove a trill
    Command* pickStrokeUpAct;
    Command* pickStrokeDownAct;

    QMenu* slideIntoMenu;
    Command* slideIntoFromAboveAct;
    Command* slideIntoFromBelowAct;

    QMenu* slideOutOfMenu;
    Command* slideOutOfDownwardsAct;
    Command* slideOutOfUpwardsAct;

    QMenu* guitarMenu;
    Command* addGuitarAct;

    QMenu* windowMenu;
    Command* nextTabAct; // cycle to the next/previous tab
    Command* prevTabAct;

    QString previousDirectory; // previous directory that a file was opened in
    boost::scoped_ptr<QStackedWidget> mixerList;
    boost::scoped_ptr<QStackedWidget> playbackToolbarList;

    boost::shared_ptr<SkinManager> skinManager;
    boost::scoped_ptr<MidiPlayer> midiPlayer;

private:
    /// helper function for connecting an action to the performToggleProperty slot
    /// @param objectsGetter - function for retrieving the objects to be edited
    /// @param propertyGetter - function for checking if an object has a certain property set
    /// @param propertySetter - function for toggling the property
    template <typename T>
    void connectToggleProperty(QAction* action,
                               boost::function<std::vector<T*> (void)> objectsGetter,
                               boost::function<bool (const T*)> propertyGetter,
                               boost::function<bool (T*, bool)> propertySetter)
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
    void performToggleProperty(const std::vector<T*>& objects, boost::function<bool (T*, bool)> setPropertyFn,
                               boost::function<bool (const T*)> getPropertyFn, const QString& propertyName)
    {
        undoManager->push(new ToggleProperty<T>(objects, setPropertyFn, getPropertyFn, propertyName));
    }
};

#endif // POWERTABEDITOR_H
