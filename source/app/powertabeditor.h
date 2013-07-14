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
  
#ifndef APP_POWERTABEDITOR_H
#define APP_POWERTABEDITOR_H

#include <QMainWindow>

#include <boost/scoped_ptr.hpp>
#include <string>
#include <vector>

class Command;
class DocumentManager;
class FileFormatManager;
class RecentFiles;
class ScoreArea;

#if 0
// the sigfwd library allows for connecting Qt signals directly to
// C++ functions & functors, and supports boost::bind for binding arguments to slots
#include <boost/bind.hpp>
#include <sigfwd/sigfwd.hpp>

#include <boost/scoped_ptr.hpp>
#include <actions/undomanager.h>
#include <actions/toggleproperty.h>
#include <app/scorearea.h>
#include <painters/caret.h>

class QTabWidget;
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
class FileFormat;
class Command;
class Caret;
class PlaybackWidget;
class SettingsPubSub;
class SystemLocation;
class TuningDictionary;
class Mixer;
#endif

class PowerTabEditor : public QMainWindow
{
    Q_OBJECT

#if 0
    friend class NotePage;
    friend class ScorePage;
#endif

public:
    PowerTabEditor();
    ~PowerTabEditor();

    /// Opens the given list of files.
    void openFiles(const std::vector<std::string> &files);

private slots:
    /// Creates a new (blank) document.
    void createNewDocument();

    /// Opens a new file. If 'filename' is empty, the user will be prompted
    /// to select a filename.
    void openFile(QString filename = "");

    /// Handle when the active tab is changed.
    void switchTab(int index);

    /// Closes the specified tab.
    /// @return True if the document was closed successfully.
    bool closeTab(int index);

    /// Closes the current document.
    /// @return True if the document was closed successfully.
    bool closeCurrentTab();

    /// Saves the current document to a new filename.
    /// @return True if the file was successfully saved.
    bool saveFileAs();

    /// Cycles through the tabs in the tab bar.
    /// @param offset Direction and number of tabs to move by
    /// (i.e. -1 moves back one tab).
    void cycleTab(int offset);

    /// Launches a dialog for the user to edit keyboard shortcuts.
    void editKeyboardShortcuts();

    /// Launches the preferences dialog.
    void editPreferences();

private:
    /// Returns the application name & version (e.g. 'Power Tab Editor 2.0').
    QString getApplicationName() const;
    /// Updates the window title with the file path of the active document.
    void updateWindowTitle();

    /// Create all of the commands for the application.
    void createCommands();
    /// Set up the menus for the application.
    void createMenus();
    /// Create the tab widget and score area.
    void createTabArea();
    /// Updates the last directory that a file was opened from.
    void setPreviousDirectory(const QString &fileName);
    /// Sets up the UI for the current document after it has been opened.
    void setupNewTab();

    /// Returns the score area for the active document.
    ScoreArea *getScoreArea();

    boost::scoped_ptr<DocumentManager> myDocumentManager;
    boost::scoped_ptr<FileFormatManager> myFileFormatManager;
    /// Tracks the last directory that a file was opened from.
    QString myPreviousDirectory;
    RecentFiles *myRecentFiles;

    QTabWidget *myTabWidget;

    QMenu *myFileMenu;
    Command *myNewDocumentCommand;
    Command *myOpenFileCommand;
    Command *myCloseTabCommand;
    Command *mySaveAsCommand;
    QMenu *myRecentFilesMenu;
    Command *myEditShortcutsCommand;
    Command *myEditPreferencesCommand;
    Command *myExitCommand;

    QMenu *myWindowMenu;
    Command *myNextTabCommand;
    Command *myPrevTabCommand;

#if 0
    static boost::scoped_ptr<UndoManager> undoManager;
    void registerCaret(Caret* caret);

protected:
    virtual void closeEvent(QCloseEvent*);

private:
    void updateScoreAreaActions(bool enable);
    bool eventFilter(QObject *obj, QEvent *ev);
    void changePositionSpacing(int offset);
    void changeNoteDuration(bool increase);
    void performSystemInsert(size_t index);
    int getCurrentPlaybackSpeed() const;

    void editSlideInto(uint8_t newSlideIntoType);
    void editSlideOutOf(uint8_t newSlideType);
    void shiftTabNumber(int direction);
    void updateNoteDuration(uint8_t duration);
    void addRest();
    void editRest(uint8_t duration);
    void editKeySignature(const SystemLocation &location);
    void editTimeSignature(const SystemLocation &location);
    void editBarline(const SystemLocation &location);

    static std::vector<Position*> getSelectedPositions();
    static std::vector<Note*> getSelectedNotes();
    PlaybackWidget* getCurrentPlaybackWidget() const;
    Mixer* getCurrentMixer();

private slots:
    void updateActions();
    void updateLocationLabel();
    void updateModified(bool);
    void updateActiveVoice(int);
    void openFileInformation();
    void redrawSystem(int);
    void performFullRedraw();
    void startStopPlayback();
    void rewindPlaybackToStart();
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

    void gotoBarline();
    void gotoRehearsalSign();

    void editChordName();
    void editRehearsalSign();
    void editTempoMarker();
    void editMusicalDirection();
    void editRepeatEnding();
    void editTiedNote();
    void editHammerPull();
    void editTrill();
    void editBarlineFromCaret();
    void insertStandardBarline();
    void editArtificialHarmonic();
    void editTappedHarmonic();
    void editKeySignatureFromCaret();
    void editTimeSignatureFromCaret();
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
    void showTuningDictionary();
    void toggleGuitarVisible(uint32_t trackIndex, bool isVisible);

    void doPaste();
    void copySelectedNotes();
    void cutSelectedNotes();

private:
    bool isPlaying;

    uint8_t activeDuration;

    Toolbox* toolBox;
    QSplitter* vertSplitter;
    QSplitter* horSplitter;

    QMenu* editMenu;
    QAction* undoAct;
    QAction* redoAct;
    Command* cutAct;
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
    Command* gotoBarlineAct;
    Command* gotoRehearsalSignAct;

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
    Command* increaseDurationAct;
    Command* decreaseDurationAct;
    Command* dottedNoteAct; // sets a note to be dotted
    Command* doubleDottedNoteAct;
    Command* addDotAct;
    Command* removeDotAct;
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
    Command* addRestAct;

    QMenu* musicSymbolsMenu;
    Command* rehearsalSignAct; // add/remove rehearsal signs
    Command* tempoMarkerAct;
    Command* keySignatureAct;
    Command* timeSignatureAct;
    Command* barlineAct;
    Command* standardBarlineAct;
    Command* musicalDirectionAct;
    Command* repeatEndingAct;
    Command* dynamicAct;
    Command* volumeSwellAct;

    QMenu* tabSymbolsMenu;
    Command* hammerPullAct; // add/remove hammer-on or pull-off
    Command* naturalHarmonicAct; // add/remove natural harmonics
    Command* artificialHarmonicAct;
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
    Command* tuningDictionaryAct;

    boost::scoped_ptr<QStackedWidget> mixerList;
    boost::scoped_ptr<QStackedWidget> playbackToolbarList;

    boost::shared_ptr<SkinManager> skinManager;
    boost::scoped_ptr<MidiPlayer> midiPlayer;

    boost::shared_ptr<SettingsPubSub> settingsPubSub;
    boost::shared_ptr<TuningDictionary> tuningDictionary;

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
        undoManager->push(new ToggleProperty<T>(objects, setPropertyFn, getPropertyFn, propertyName),
                          getCurrentScoreArea()->getCaret()->getCurrentSystemIndex());
    }

#endif
};

#endif
