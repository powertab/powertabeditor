#ifndef POWERTABEDITOR_H
#define POWERTABEDITOR_H

#include <QMainWindow>
#include <QMultiMap>

#include "documentmanager.h"

class QTabWidget;
class QUndoStack;
class ScoreArea;
class Mixer;
class QStackedWidget;
class SkinManager;
class RtMidiWrapper;
class PreferencesDialog;
class Toolbox;
class QSignalMapper;
class QSplitter;

class PowerTabEditor : public QMainWindow
{
    Q_OBJECT

public:
    PowerTabEditor(QWidget *parent = 0);
    ~PowerTabEditor();
    static void RefreshCurrentDocument();
    static QTabWidget* tabWidget;
    static QUndoStack* undoStack;
    static QSplitter* vertSplitter;
    static QSplitter* horSplitter;
    static Toolbox* toolBox;
    ScoreArea* getCurrentScoreArea();

private:
    void CreateActions();
    void CreateMenus();
    void CreateTabArea();

private slots:
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

    void moveCaretToFirstSection();
    bool moveCaretToNextSection();
    bool moveCaretToPrevSection();
    void moveCaretToLastSection();

    void playNotesAtCurrentPosition(int system);
    void playbackSong(int system);

private:
    QSignalMapper* signalMapper;
    bool isPlaying;
    struct NoteHistory
    {
        unsigned int pitch; unsigned int stringNum;
        bool operator==(const NoteHistory& history) { return (pitch == history.pitch) && (stringNum == history.stringNum); }
    };
    QMultiMap<unsigned int, NoteHistory> oldNotes; // map channels to pitches
    QTimer* songTimer[8];

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

    PreferencesDialog* preferencesDialog;
    QString previousDirectory; // previous directory that a file was opened in
    QStackedWidget* mixerList;

    SkinManager* skinManager;

    RtMidiWrapper* rtMidiWrapper;
    QMap<quint32, quint32> previousPositionInStaff;
};

#endif // POWERTABEDITOR_H
