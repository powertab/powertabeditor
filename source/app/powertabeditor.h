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
#include <boost/shared_ptr.hpp>
#include <score/position.h>
#include <string>
#include <vector>

class Caret;
class Command;
class DocumentManager;
class FileFormatManager;
class MidiPlayer;
class QActionGroup;
class RecentFiles;
class ScoreArea;
class ScoreLocation;
class SettingsPubSub;
class TuningDictionary;
class UndoManager;

class PowerTabEditor : public QMainWindow
{
    Q_OBJECT

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

    /// Update the titlebar to show whether the current document has been
    /// modified.
    void updateModified(bool);

    /// Cycles through the tabs in the tab bar.
    /// @param offset Direction and number of tabs to move by
    /// (i.e. -1 moves back one tab).
    void cycleTab(int offset);

    /// Launches a dialog for the user to edit keyboard shortcuts.
    void editKeyboardShortcuts();

    /// Launches the preferences dialog.
    void editPreferences();

    /// Copies the selected positions to the keyboard.
    void copySelectedNotes();
    /// Paste notes from the clipboard at the current location.
    void pasteNotes();

    /// Starts or stops playback of the score.
    void startStopPlayback();

    /// Redraws only the given system.
    void redrawSystem(int);
    /// Redraws the entire score.
    void redrawScore();

    /// Moves the caret to the first position in the staff.
    void moveCaretToStart();
    /// Moves the caret to the right by one position.
    void moveCaretRight();
    /// Moves the caret to the left by one position.
    void moveCaretLeft();
    /// Moves the caret down by one string.
    void moveCaretDown();
    /// Moves the caret up by one string.
    void moveCaretUp();
    /// Moves the caret to the last position in the staff.
    void moveCaretToEnd();
    /// Moves the caret to the specified position.
    void moveCaretToPosition(int position);
    /// Moves the caret to the first system in the score.
    void moveCaretToFirstSection();
    /// Moves the caret to the next system in the score.
    void moveCaretToNextSection();
    /// Moves the caret to the previous system in the score.
    void moveCaretToPrevSection();
    /// Moves the caret to the last system in the score.
    void moveCaretToLastSection();
    /// Moves the caret to the specified system.
    void moveCaretToSystem(int system);
    /// Moves the caret to the next staff in the system.
    void moveCaretToNextStaff();
    /// Moves the caret to the previous staff in the system.
    void moveCaretToPrevStaff();
    /// Moves the caret to the next bar after the current position.
    void moveCaretToNextBar();
    /// Moves the caret to the last bar before the current position.
    void moveCaretToPrevBar();
    /// Shifts all positions after the current location forward.
    void shiftForward();
    /// Moves all positions after the current location backwards.
    void shiftBackward();
    /// Moves the caret to a specific barline.
    void gotoBarline();
    /// Moves the caret to a specific rehearsal sign.
    void gotoRehearsalSign();

    /// Inserts a new system at the end of the score.
    void insertSystemAtEnd();
    /// Inserts a new system before the current system.
    void insertSystemBefore();
    /// Inserts a new system after the current system.
    void insertSystemAfter();
    /// Deletes the current system.
    void removeCurrentSystem();

    /// Updates the duration of the current note and sets the default duration
    /// for new notes.
    void updateNoteDuration(Position::DurationType duration);
    /// Increase or decrease the current note duration.
    void changeNoteDuration(bool increase);
    /// Adds a dot to the current position's duration.
    void addDot();
    /// Removes a dot from the current position's duration.
    void removeDot();
    /// Toggles whether the current note is tied.
    void editTiedNote();

    /// Inserts a rest at the current location.
    void addRest();

    /// Adds or removes a rehearsal sign at the current barline.
    void editRehearsalSign();
    /// Adds or removes a tempo marker at the current position.
    void editTempoMarker();
    /// Edits the key signature at the caret's current location.
    void editKeySignatureFromCaret();
    /// Edits the time signature at the caret's current location.
    void editTimeSignatureFromCaret();
    /// Inserts a single barline at the current location.
    void insertStandardBarline();
    /// Edits or inserts a barline at the current location.
    void editBarlineFromCaret();
    /// Adds or removes a musical direction at the current position.
    void editMusicalDirection();
    /// Adds or removes a dynamic at the current location.
    void editDynamic();

    /// Adds or removes a hammeron/pulloff for the current note.
    void editHammerPull();
    /// Adds or removes a tapped harmonic for the current note.
    void editTappedHarmonic();
    /// Adds or removes a trill for the current note.
    void editTrill();

    /// Adds or removes a player change at the current location.
    void editPlayerChange();
    /// Shows a dialog to view or edit the tuning dictionary.
    void showTuningDictionary();

protected:
    /// Handle key presses for 0-9 when entering tab numbers.
    virtual bool eventFilter(QObject *object, QEvent *event);

    /// Performs some final actions before exiting.
    virtual void closeEvent(QCloseEvent*);

private:
    /// Returns the application name & version (e.g. 'Power Tab Editor 2.0').
    QString getApplicationName() const;
    /// Updates the window title with the file path of the active document.
    void updateWindowTitle();

    /// Create all of the commands for the application.
    void createCommands();
    /// Helper function to create a note duration command.
    void createNoteDurationCommand(Command *&command, const QString &menuName,
                                   const QString &commandName,
                                   Position::DurationType durationType);
    /// Helper function to create a rest duration command.
    void createRestDurationCommand(Command *&command, const QString &menuName,
                                   const QString &commandName,
                                   Position::DurationType durationType);
    /// Helper function to create a command for toggling a simple note
    /// property.
    void createNotePropertyCommand(Command *&command, const QString &menuName,
                                   const QString &commandName,
                                   const QKeySequence &shortcut,
                                   Note::SimpleProperty property);
    /// Helper function to create a command for toggling a simple position
    /// property.
    void createPositionPropertyCommand(Command *&command,
                                       const QString &menuName,
                                       const QString &commandName,
                                       const QKeySequence &shortcut,
                                       Position::SimpleProperty property);
    /// Set up the menus for the application.
    void createMenus();
    /// Create the tab widget and score area.
    void createTabArea();
    /// Updates the last directory that a file was opened from.
    void setPreviousDirectory(const QString &fileName);
    /// Sets up the UI for the current document after it has been opened.
    void setupNewTab();
    /// Updates whether menu items are enabled, checked, etc. depending on the
    /// current location.
    void updateCommands();
    /// Enables or disables all editing commands.
    void enableEditing(bool enable);

    /// Adds or removes a rest at the current location.
    void editRest(Position::DurationType duration);

    /// Edits the key signature at the given location.
    void editKeySignature(const ScoreLocation &keyLocation);
    /// Edits the time signature at the given location.
    void editTimeSignature(const ScoreLocation &timeLocation);
    /// Edits the barline at the given location.
    void editBarline(const ScoreLocation &barLocation);
    /// Edits the clef at the given location.
    void editClef(int system, int staff);

    /// Toggles a simple position property.
    void editSimplePositionProperty(Command *command,
                                    Position::SimpleProperty property);
    /// Toggles a simple note property.
    void editSimpleNoteProperty(Command *command, Note::SimpleProperty property);

    /// Helper function to insert a system at the given index.
    void insertSystem(int index);
    /// Increases or decreases the line spacing by the given amount.
    void adjustLineSpacing(int amount);

    /// Returns the score area for the active document.
    ScoreArea *getScoreArea();
    /// Returns the caret for the active document.
    Caret &getCaret();
    /// Returns the location of the caret within the active document.
    ScoreLocation &getLocation();

    boost::scoped_ptr<DocumentManager> myDocumentManager;
    boost::scoped_ptr<FileFormatManager> myFileFormatManager;
    boost::scoped_ptr<UndoManager> myUndoManager;
    boost::scoped_ptr<MidiPlayer> myMidiPlayer;
    boost::scoped_ptr<TuningDictionary> myTuningDictionary;
    boost::shared_ptr<SettingsPubSub> mySettingsPubSub;
    /// Tracks whether we are currently in playback mode.
    bool myIsPlaying;
    /// Tracks the last directory that a file was opened from.
    QString myPreviousDirectory;
    RecentFiles *myRecentFiles;
    Position::DurationType myActiveDurationType;

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

    QMenu *myEditMenu;
    QAction *myUndoAction;
    QAction *myRedoAction;
    Command *myCopyCommand;
    Command *myPasteCommand;

    QMenu *myPlaybackMenu;
    Command *myPlayPauseCommand;

    QMenu *myPositionMenu;
    QMenu *myPositionSectionMenu;
    Command *myFirstSectionCommand;
    Command *myNextSectionCommand;
    Command *myPrevSectionCommand;
    Command *myLastSectionCommand;
    QMenu *myPositionStaffMenu;
    Command *myStartPositionCommand;
    Command *myNextPositionCommand;
    Command *myPrevPositionCommand;
    Command *myNextStringCommand;
    Command *myPrevStringCommand;
    Command *myLastPositionCommand;    
    Command *myNextStaffCommand;
    Command *myPrevStaffCommand;
    Command *myNextBarCommand;
    Command *myPrevBarCommand;
    Command *myShiftForwardCommand;
    Command *myShiftBackwardCommand;
    Command *myGoToBarlineCommand;
    Command *myGoToRehearsalSignCommand;

    QMenu *mySectionMenu;
    Command *myInsertSystemAtEndCommand;
    Command *myInsertSystemBeforeCommand;
    Command *myInsertSystemAfterCommand;
    Command *myRemoveCurrentSystemCommand;
    QMenu *myLineSpacingMenu;
    Command *myIncreaseLineSpacingCommand;
    Command *myDecreaseLineSpacingCommand;

    QMenu *myNotesMenu;
    /// Used to ensure that only one duration option is checked at a time.
    QActionGroup *myNoteDurationGroup;
    Command *myWholeNoteCommand;
    Command *myHalfNoteCommand;
    Command *myQuarterNoteCommand;
    Command *myEighthNoteCommand;
    Command *mySixteenthNoteCommand;
    Command *myThirtySecondNoteCommand;
    Command *mySixtyFourthNoteCommand;
    Command *myIncreaseDurationCommand;
    Command *myDecreaseDurationCommand;
    Command *myDottedCommand;
    Command *myDoubleDottedCommand;
    Command *myAddDotCommand;
    Command *myRemoveDotCommand;
    Command *myTieCommand;
    Command *myMutedCommand;
    Command *myGhostNoteCommand;
    Command *myLetRingCommand;
    Command *myFermataCommand;
    Command *myGraceNoteCommand;
    Command *myStaccatoCommand;
    Command *myMarcatoCommand;
    Command *mySforzandoCommand;
    QMenu *myOctaveMenu;
    Command *myOctave8vaCommand;
    Command *myOctave15maCommand;
    Command *myOctave8vbCommand;
    Command *myOctave15mbCommand;    

    QMenu *myRestsMenu;
    /// Used to ensure that only one duration option is checked at a time.
    QActionGroup *myRestDurationGroup;
    Command *myWholeRestCommand;
    Command *myHalfRestCommand;
    Command *myQuarterRestCommand;
    Command *myEighthRestCommand;
    Command *mySixteenthRestCommand;
    Command *myThirtySecondRestCommand;
    Command *mySixtyFourthRestCommand;
    Command *myAddRestCommand;

    QMenu *myMusicSymbolsMenu;
    Command *myRehearsalSignCommand;
    Command *myTempoMarkerCommand;
    Command *myKeySignatureCommand;
    Command *myTimeSignatureCommand;
    Command *myStandardBarlineCommand;
    Command *myBarlineCommand;
    Command *myDirectionCommand;
    Command *myDynamicCommand;

    QMenu *myTabSymbolsMenu;
    QMenu *myHammerOnMenu;
    Command *myHammerPullCommand;
    Command *myHammerOnFromNowhereCommand;
    Command *myPullOffToNowhereCommand;

    Command *myNaturalHarmonicCommand;
    Command *myTappedHarmonicCommand;

    QMenu *mySlideIntoMenu;
    Command *mySlideIntoFromAboveCommand;
    Command *mySlideIntoFromBelowCommand;

    Command *myShiftSlideCommand;
    Command *myLegatoSlideCommand;

    QMenu *mySlideOutOfMenu;
    Command *mySlideOutOfDownwardsCommand;
    Command *mySlideOutOfUpwardsCommand;

    Command *myVibratoCommand;
    Command *myWideVibratoCommand;
    Command *myPalmMuteCommand;
    Command *myTremoloPickingCommand;
    Command *myTrillCommand;
    Command *myTapCommand;
    Command *myPickStrokeUpCommand;
    Command *myPickStrokeDownCommand;
    Command *myArpeggioUpCommand;
    Command *myArpeggioDownCommand;

    QMenu *myPlayerMenu;
    Command *myPlayerChangeCommand;
    Command *myShowTuningDictionaryCommand;

    QMenu *myWindowMenu;
    Command *myNextTabCommand;
    Command *myPrevTabCommand;

#if 0

private:
    bool eventFilter(QObject *obj, QEvent *ev);
    void changePositionSpacing(int offset);
    void performSystemInsert(size_t index);
    int getCurrentPlaybackSpeed() const;

    void shiftTabNumber(int direction);

    PlaybackWidget* getCurrentPlaybackWidget() const;
    Mixer* getCurrentMixer();

private slots:
    void updateActions();
    void updateLocationLabel();
    void updateActiveVoice(int);
    void openFileInformation();
    void performFullRedraw();
    void rewindPlaybackToStart();
    bool moveCaretLeft();
    void moveCaretDown();
    void moveCaretUp();

    void editChordName();
    void editRepeatEnding();
    void editArtificialHarmonic();
    void editVolumeSwell();
    void editIrregularGrouping(bool setAsTriplet = false);

    void clearNote();
    void clearCurrentPosition();

    void addGuitar();
    void toggleGuitarVisible(uint32_t trackIndex, bool isVisible);

    void cutSelectedNotes();

private:
    Toolbox* toolBox;
    QSplitter* vertSplitter;
    QSplitter* horSplitter;

    Command* cutAct;
    Command* fileInfoAct;

    Command* clearNoteAct; // clears the active note
    Command* clearCurrentPositionAct; // clears out the entire position (either notes or barline)

    Command* shiftTabNumUp; // shift tab numbers up/down by a string
    Command* shiftTabNumDown;

    QMenu* textMenu;
    Command* chordNameAct; // add/remove a chord name

    Command* tripletAct;
    Command* irregularGroupingAct;

    Command* repeatEndingAct;
    Command* volumeSwellAct;

    Command* artificialHarmonicAct;

    QMenu* guitarMenu;
    Command* addGuitarAct;

    boost::scoped_ptr<QStackedWidget> mixerList;
    boost::scoped_ptr<QStackedWidget> playbackToolbarList;

#endif
};

#endif
