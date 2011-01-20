#ifndef POWERTABEDITOR_H
#define POWERTABEDITOR_H

#include <QMainWindow>

#include <documentmanager.h>
#include <actions/undomanager.h>

class QTabWidget;
class ScoreArea;
class Mixer;
class QStackedWidget;
class SkinManager;
class PreferencesDialog;
class Toolbox;
class QSplitter;
class MidiPlayer;
class QSignalMapper;
class QActionGroup;

class PowerTabEditor : public QMainWindow
{
    Q_OBJECT

    friend class NotePage;

public:
    PowerTabEditor(QWidget *parent = 0);
    ~PowerTabEditor();
    static void RefreshCurrentDocument();
    static QTabWidget* tabWidget;
    static UndoManager* undoManager;
    static QSplitter* vertSplitter;
    static QSplitter* horSplitter;
    static Toolbox* toolBox;
    static ScoreArea* getCurrentScoreArea();

protected:
    void CreateActions();
    void CreateMenus();
    void CreateTabArea();
    void updateScoreAreaActions(bool disable);

private slots:
    void updateActions();
    void OpenFile();
    void OpenPreferences();
    void RefreshOnUndoRedo(int);
    void closeTab(int index);
    void switchTab(int index);
    void startStopPlayback();
    bool moveCaretRight();
    bool moveCaretLeft();
    void moveCaretDown();
    void moveCaretUp();
    void moveCaretToStart();
    void moveCaretToEnd();

    bool moveCaretToNextStaff();
    bool moveCaretToPrevStaff();

    void moveCaretToFirstSection();
    bool moveCaretToNextSection();
    bool moveCaretToPrevSection();
    void moveCaretToLastSection();

    void editChordName();

    void editNoteDuration(int duration);

private:
    bool isPlaying;

    DocumentManager documentManager;
    QMenu* fileMenu;
    QAction* openFileAct;
    QAction* preferencesAct;
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

    QMenu* positionStaffMenu;
    QAction* nextPositionAct; // navigate to the next position in the staff
    QAction* prevPositionAct; // navigate to the previous position in the staff
    QAction* startPositionAct; // navigate to the first position in the staff
    QAction* lastPositionAct; // navigate to the last position in the staff
    QAction* nextStringAct; // navigate to the next string in the staff
    QAction* prevStringAct; // navigate to the previous string in the staff
    QAction* nextStaffAct; // navigate to the next staff in the system
    QAction* prevStaffAct; // navigate to the previous staff in the system

    QMenu* textMenu;
    QAction* chordNameAct; // add/remove a chord name

    QMenu* notesMenu;
    QSignalMapper* noteDurationMapper; // map note duration signals to a slot
    QActionGroup* noteDurationActGroup; // only one duration can be checked at a time
    QAction* wholeNoteAct; // actions for modifying the duration of a note/rest
    QAction* halfNoteAct;
    QAction* quarterNoteAct;
    QAction* eighthNoteAct;
    QAction* sixteenthNoteAct;
    QAction* thirtySecondNoteAct;
    QAction* sixtyFourthNoteAct;

    PreferencesDialog* preferencesDialog;
    QString previousDirectory; // previous directory that a file was opened in
    QStackedWidget* mixerList;

    SkinManager* skinManager;
    MidiPlayer* midiPlayer;
};

#endif // POWERTABEDITOR_H
