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

#include <app/pubsub/instrumentpubsub.h>
#include <app/pubsub/playerpubsub.h>
#include <memory>
#include <score/position.h>
#include <string>
#include <vector>

class Caret;
class Command;
class DocumentManager;
class FileFormatManager;
class InstrumentPanel;
class MidiPlayer;
class Mixer;
class PlaybackWidget;
class QActionGroup;
class RecentFiles;
class ScoreArea;
class ScoreLocation;
class SettingsManager;
class TuningDictionary;
class UndoManager;

class PowerTabEditor : public QMainWindow
{
    Q_OBJECT

public:
    PowerTabEditor();
    ~PowerTabEditor();

    /// Opens the given list of files.
    void openFiles(const QStringList &files);

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

    /// Saves the current document.
    /// @return True if the file was successfully saved.
    bool saveFile();

    /// Saves the current document to a new filename.
    /// @return True if the file was successfully saved.
    bool saveFileAs();

    /// Prints the current document.
    void printDocument();

    /// Displays a preview of the current document.
    void printPreview();

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

    /// Cuts the selected positions and places them on the clipboard.
    void cutSelectedNotes();
    /// Copies the selected positions to the clipboard.
    void copySelectedNotes();
    /// Paste notes from the clipboard at the current location.
    void pasteNotes();

    /// Runs the score polisher.
    void polishScore();
    /// Runs the score polisher on the current system.
    void polishSystem();
    /// Opens the file information dialog.
    void editFileInformation();

    /// Starts or stops playback of the score.
    void startStopPlayback(bool from_measure_start = false);

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
    /// Deletes the current note.
    void removeNote();
    /// Deletes the currently-selected positions.
    void removeSelectedPositions();
    /// Moves the caret to a specific barline.
    void gotoBarline();
    /// Moves the caret to a specific rehearsal sign.
    void gotoRehearsalSign();

    /// Adds or removes a chord name at the current position.
    void editChordName();
    /// Adds or removes a text item at the current position.
    void editTextItem();

    /// Inserts a new system at the end of the score.
    void insertSystemAtEnd();
    /// Inserts a new system before the current system.
    void insertSystemBefore();
    /// Inserts a new system after the current system.
    void insertSystemAfter();
    /// Deletes the current system.
    void removeCurrentSystem();
    /// Inserts a new staff before the current staff.
    void insertStaffBefore();
    /// Inserts a new staff after the current staff.
    void insertStaffAfter();
    /// Deletes the current staff.
    void removeCurrentStaff();

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
    /// Adds or removes an irregular grouping for the selected notes.
    void editIrregularGrouping(bool setAsTriplet = false);

    /// Inserts a rest at the current location.
    void addRest();
    /// Adds or removes a multibar rest at the current location.
    void editMultiBarRest();

    /// Adds or removes a rehearsal sign at the current barline.
    void editRehearsalSign();
    /// Adds or removes a tempo marker at the current position.
    void editTempoMarker();
    /// Adds or removes an accel/rit symbol at the current position.
    void editAlterationOfPace();
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
    /// Adds or removes a repeat ending at the current position.
    void editRepeatEnding();
    /// Adds or removes a dynamic at the current location.
    void editDynamic();

    /// Adds or removes a hammeron/pulloff for the current note.
    void editHammerPull();
    /// Adds or removes an artificial harmonic for the current note.
    void editArtificialHarmonic();
    /// Adds or removes a tapped harmonic for the current note.
    void editTappedHarmonic();
	/// Adds or removes a bend for the current note.
	void editBend();
    /// Adds or removes a trill for the current note.
    void editTrill();

    /// Adds a new player to the score.
    void addPlayer();
    /// Adds a new instrument to the score.
    void addInstrument();
    /// Adds or removes a player change at the current location.
    void editPlayerChange();
    /// Edits the properties of a player.
    void editPlayer(int playerIndex, const Player &player, bool undoable);
    /// Removes the specified player.
    void removePlayer(int index);
    /// Edits the properties of an instrument.
    void editInstrument(int index, const Instrument &instrument);
    /// Removes the specified instrument.
    void removeInstrument(int index);
    /// Shows a dialog to view or edit the tuning dictionary.
    void showTuningDictionary();
    /// Shows a dialog to edit the score's view filters.
    void editViewFilters();

protected:
    /// Handle key presses for 0-9 when entering tab numbers.
    virtual bool eventFilter(QObject *object, QEvent *event) override;

    /// Performs some final actions before exiting.
    virtual void closeEvent(QCloseEvent*) override;

    /// Accept drag events.
    virtual void dragEnterEvent(QDragEnterEvent *event) override;

    /// Open files that have been dropped.
    virtual void dropEvent(QDropEvent *event) override;

private:
    /// Returns the application name & version (e.g. 'Power Tab Editor 2.0').
    QString getApplicationName() const;
    /// Updates the window title with the file path of the active document.
    void updateWindowTitle();

    /// Create all of the commands for the application.
    void createCommands();
    /// Build the mixer widget.
    void createMixer();
    /// Build the instrument panel.
    void createInstrumentPanel();

    /// Load any custom keyboard shortcuts.
    void loadKeyboardShortcuts();
    /// Save any custom keyboard shortcuts.
    void saveKeyboardShortcuts() const;
    /// Return a list of all actions.
    std::vector<const Command *> getCommands() const;
    std::vector<Command *> getCommands();

    /// Helper function to create a wrapper around QAction that supports
    /// customizable shortcuts.
    static Command *createCommandWrapper(QAction *action, const QString &id,
                                         const QKeySequence &defaultShortcut,
                                         QObject *parent);
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

    /// Moves the caret back to the start, and restarts playback if necessary.
    void rewindPlaybackToStart();
    /// Stops playback and returns to the initial position.
    void stopPlayback();
    /// Toggles the metronome on or off.
    void toggleMetronome();
    /// Sets the current voice that is being edited.
    void updateActiveVoice(int);
    /// Sets the current score filter.
    void updateActiveFilter(int);
    /// Updates the zoom level.
    void updateZoom(double percent);
    /// Updates the playback widget with the caret's current location.
    void updateLocationLabel();

    /// Saves the current document to the specified path.
    /// @return True if the file was successfully saved.
    bool saveFile(QString path);

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
    /// Helper function to insert a staff at the given index in a system.
    void insertStaff(int index);
    /// Edits the number of strings for the current staff.
    void editStaff(int system, int staff);
    /// Increases or decreases the line spacing by the given amount.
    void adjustLineSpacing(int amount);

    /// Returns the score area for the active document.
    ScoreArea *getScoreArea();
    /// Returns the caret for the active document.
    Caret &getCaret();
    /// Returns the location of the caret within the active document.
    ScoreLocation &getLocation();

    std::unique_ptr<SettingsManager> mySettingsManager;
    std::unique_ptr<DocumentManager> myDocumentManager;
    std::unique_ptr<FileFormatManager> myFileFormatManager;
    std::unique_ptr<UndoManager> myUndoManager;
    std::unique_ptr<MidiPlayer> myMidiPlayer;
    std::unique_ptr<TuningDictionary> myTuningDictionary;
    PlayerEditPubSub myPlayerEditPubSub;
    PlayerRemovePubSub myPlayerRemovePubSub;
    InstrumentEditPubSub myInstrumentEditPubSub;
    InstrumentRemovePubSub myInstrumentRemovePubSub;
    /// Tracks whether we are currently in playback mode.
    bool myIsPlaying;
    /// Tracks the last directory that a file was opened from.
    QString myPreviousDirectory;
    RecentFiles *myRecentFiles;
    Position::DurationType myActiveDurationType;

    QTabWidget *myTabWidget;
    Mixer *myMixer;
    QDockWidget *myMixerDockWidget;
    InstrumentPanel *myInstrumentPanel;
    QDockWidget *myInstrumentDockWidget;
    PlaybackWidget *myPlaybackWidget;
    QWidget *myPlaybackArea;

    QMenu *myFileMenu;
    Command *myNewDocumentCommand;
    Command *myOpenFileCommand;
    Command *myCloseTabCommand;
    Command *mySaveCommand;
    Command *mySaveAsCommand;
    Command *myPrintCommand;
    Command *myPrintPreviewCommand;
    QMenu *myRecentFilesMenu;
    Command *myEditShortcutsCommand;
    Command *myEditPreferencesCommand;
    Command *myExitCommand;

    QMenu *myEditMenu;
    QAction *myUndoAction;
    QAction *myRedoAction;
    Command *myCutCommand;
    Command *myCopyCommand;
    Command *myPasteCommand;
    Command *myPolishCommand;
    Command *myPolishSystemCommand;
    Command *myFileInfoCommand;

    QMenu *myPlaybackMenu;
    Command *myPlayPauseCommand;
    Command *myPlayFromStartOfMeasureCommand;
    Command *myStopCommand;
    Command *myRewindCommand;
    Command *myMetronomeCommand;

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
    Command *myRemoveNoteCommand;
    Command *myRemovePositionCommand;
    Command *myGoToBarlineCommand;
    Command *myGoToRehearsalSignCommand;

    QMenu *myTextMenu;
    Command *myChordNameCommand;
    Command *myTextCommand;

    QMenu *mySectionMenu;
    Command *myInsertSystemAtEndCommand;
    Command *myInsertSystemBeforeCommand;
    Command *myInsertSystemAfterCommand;
    Command *myRemoveCurrentSystemCommand;
    Command *myInsertStaffBeforeCommand;
    Command *myInsertStaffAfterCommand;
    Command *myRemoveCurrentStaffCommand;
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
    Command *myTripletCommand;
    Command *myIrregularGroupingCommand;

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
    Command *myMultibarRestCommand;

    QMenu *myMusicSymbolsMenu;
    Command *myRehearsalSignCommand;
    Command *myTempoMarkerCommand;
    Command *myAlterationOfPaceCommand;
    Command *myKeySignatureCommand;
    Command *myTimeSignatureCommand;
    Command *myStandardBarlineCommand;
    Command *myBarlineCommand;
    Command *myDirectionCommand;
    Command *myRepeatEndingCommand;
    Command *myDynamicCommand;

    QMenu *myTabSymbolsMenu;
    QMenu *myHammerOnMenu;
    Command *myHammerPullCommand;
    Command *myHammerOnFromNowhereCommand;
    Command *myPullOffToNowhereCommand;

    Command *myNaturalHarmonicCommand;
    Command *myArtificialHarmonicCommand;
    Command *myTappedHarmonicCommand;
	Command *myBendCommand;

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
    Command *myAddPlayerCommand;
    Command *myAddInstrumentCommand;
    Command *myPlayerChangeCommand;
    Command *myShowTuningDictionaryCommand;
    Command *myEditViewFiltersCommand;

    QMenu *myWindowMenu;
    Command *myNextTabCommand;
    Command *myPrevTabCommand;
    Command *myMixerDockWidgetCommand;
    Command *myInstrumentDockWidgetCommand;

    QMenu *myHelpMenu;
    Command *myReportBugCommand;

#if 0

private:
    void changePositionSpacing(int offset);
    void shiftTabNumber(int direction);

private slots:
    void editVolumeSwell();
    void toggleGuitarVisible(uint32_t trackIndex, bool isVisible);

private:
    Toolbox* toolBox;
    QSplitter* vertSplitter;
    QSplitter* horSplitter;

    Command* shiftTabNumUp; // shift tab numbers up/down by a string
    Command* shiftTabNumDown;

    Command* volumeSwellAct;

#endif
};

#endif
