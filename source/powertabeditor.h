#ifndef POWERTABEDITOR_H
#define POWERTABEDITOR_H

#include <QMainWindow>

#include <boost/variant.hpp>
#include <memory>

#include <documentmanager.h>
#include <actions/undomanager.h>
#include <actions/toggleproperty.h>

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
class QKeyEvent;
class QEvent;
class Note;
class Position;

class PowerTabEditor : public QMainWindow
{
    Q_OBJECT

    friend class NotePage;

public:
    PowerTabEditor(QWidget *parent = 0);
    ~PowerTabEditor();
    static std::unique_ptr<UndoManager> undoManager;
    static ScoreArea* getCurrentScoreArea();
    static Position* getCurrentPosition();
    static Note* getCurrentNote();

protected:
    void createActions();
    void createMenus();
    void createTabArea();
    void updateScoreAreaActions(bool disable);
    bool eventFilter(QObject *obj, QEvent *ev);

protected slots:
    void updateActions();
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
    void editTiedNote();
    void editHammerPull();

    void cycleTab(int offset);

    void updateNoteDuration(int duration);
    void shiftTabNumber(int direction);

protected:
    bool isPlaying;
    const QString fileFilter;

    static QTabWidget* tabWidget;
    
    Toolbox* toolBox;
    QSplitter* vertSplitter;
    QSplitter* horSplitter;

    DocumentManager documentManager;
    QMenu* fileMenu;
    QAction* openFileAct;
    QAction* closeTabAct;
    QAction* saveFileAsAct;
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
    QAction* nextBarAct; // navigate to the first non-bar pos in the next bar
    QAction* prevBarAct; // navigate to the first non-bar pos in the prev bar

    QAction* shiftTabNumUp; // shift tab numbers up/down by a string
    QAction* shiftTabNumDown;
    QSignalMapper* shiftTabNumberMapper;

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
    QAction* tiedNoteAct; // sets a note to be tied to the previous note
    QAction* noteMutedAct; // sets the note to be muted
    QAction* ghostNoteAct; // set a note to be a ghost note
    QAction* fermataAct; // set a position as fermata
    QAction* staccatoNoteAct; // set the notes of a position to be staccato

    QMenu* musicSymbolsMenu;
    QAction* rehearsalSignAct; // add/remove rehearsal signs

    QMenu* tabSymbolsMenu;
    QAction* hammerPullAct; // add/remove hammer-on or pull-off
    QAction* naturalHarmonicAct; // add/remove natural harmonics

    QMenu* windowMenu;
    QAction* nextTabAct; // cycle to the next/previous tab
    QAction* prevTabAct;
    QSignalMapper* tabCycleMapper;

    QSignalMapper* togglePropertyMapper;

    QString previousDirectory; // previous directory that a file was opened in
    std::unique_ptr<QStackedWidget> mixerList;

    std::shared_ptr<SkinManager> skinManager;
    std::unique_ptr<MidiPlayer> midiPlayer;

protected slots:
    
    // Used for editing simple toggleable properties, such as ghost notes or staccato
    // applies the propertyEditor functor to the specified toggleableProperty
    void editProperty(int propertyIndex)
    {
        boost::apply_visitor(propertyEditor(), toggleableProperties[propertyIndex]);
    }
    
protected:
    
    // stores information about a toggleable property, for later use by the propertyEditor function
    template<typename T>
    struct ToggleablePropertyRecord
    {
        T* (*objectGetter)(); // pointer to a function for retrieving the object, e.g. getCurrentNote
        bool (T::*checkPropertySet)() const; // function to check if the property is set, e.g. Note::IsMuted
        bool (T::*propertySetter)(bool); // function to set the property as true/false, e.g. Note::SetMuted
        QString propertyName; // name of the property, for use with the undo menu
    };

    // Store all of the ToggleablePropertyRecords in one list, regardless of which type (e.g. Note, Position) that
    // the structure is instantiated for. Using boost::variant makes this possible, and allows to still retrieve the type
    // of the original object later on.
    // This allows us to use QSignalMapper to map all toggleable properties to a location in the toggleableProperties list,
    // and then perform an undo/redo action using the functions in the ToggleablePropertyRecord, after a signal is triggered
    typedef boost::variant< ToggleablePropertyRecord<Note>, ToggleablePropertyRecord<Position> > ToggleableProperty;
    QList<ToggleableProperty> toggleableProperties; 
    
    // Takes a ToggleablePropertyRecord from the 'toggleableProperties' list of boost::variant objects,
    // and performs an undo/redo action
    struct propertyEditor : public boost::static_visitor<>
    {
        template <typename T>
        void operator()(ToggleablePropertyRecord<T>& propertyRecord) const
        {
            T* object = propertyRecord.objectGetter(); // get the object we are modifying, such as a Note* or Position*
            if (object == NULL) return;
            
            const bool isPropertySet = (object->*(propertyRecord.checkPropertySet))();
            const QString text = isPropertySet ? "Remove " + propertyRecord.propertyName : 
                                 "Set " + propertyRecord.propertyName;
            
            // Perform the undo/redo action
            undoManager->push(new ToggleProperty<T>(object, propertyRecord.propertySetter, !isPropertySet, text));
        }
    };
};

#endif // POWERTABEDITOR_H
