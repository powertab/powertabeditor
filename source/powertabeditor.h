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
    friend class ScorePage;

public:
    PowerTabEditor(QWidget *parent = 0);
    ~PowerTabEditor();
    static std::unique_ptr<UndoManager> undoManager;
    static ScoreArea* getCurrentScoreArea();
    static void getSelectedPositions(std::vector<Position*>& positions);
    static void getSelectedNotes(std::vector<Note*>& notes);

protected:
    void createActions();
    void createMenus();
    void createTabArea();
    void updateScoreAreaActions(bool disable);
    bool eventFilter(QObject *obj, QEvent *ev);
    void changePositionSpacing(int offset);
    void performSystemInsert(size_t index);
    int getCurrentPlaybackSpeed() const;

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
    void editSlide(int newSlideType);
    void editBarline(int position = -1);

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

    void cycleTab(int offset);

    void updateNoteDuration(int duration);
    void shiftTabNumber(int direction);

protected:
    bool isPlaying;
    const QString fileFilter;

    QString getApplicationName() const;
    void setupNewDocument();

    static QTabWidget* tabWidget;
    
    Toolbox* toolBox;
    QSplitter* vertSplitter;
    QSplitter* horSplitter;

    DocumentManager documentManager;
    QMenu* fileMenu;
    QAction* newFileAct;
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
    QSignalMapper* shiftTabNumberMapper;

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
    QSignalMapper* noteDurationMapper; // map note duration signals to a slot
    QActionGroup* noteDurationActGroup; // only one duration can be checked at a time
    QAction* wholeNoteAct; // actions for modifying the duration of a note/rest
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
    QAction* staccatoNoteAct; // set the notes of a position to be staccato
    QAction* marcatoAct; // set an accent
    QAction* sforzandoAct; // set a heavy accent

    QMenu* musicSymbolsMenu;
    QAction* rehearsalSignAct; // add/remove rehearsal signs
    QAction* barlineAct;
    QAction* repeatEndingAct;

    QMenu* tabSymbolsMenu;
    QAction* hammerPullAct; // add/remove hammer-on or pull-off
    QAction* naturalHarmonicAct; // add/remove natural harmonics
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
    QSignalMapper* slideOutMapper;

    QMenu* guitarMenu;
    QAction* addGuitarAct;

    QMenu* windowMenu;
    QAction* nextTabAct; // cycle to the next/previous tab
    QAction* prevTabAct;
    QSignalMapper* tabCycleMapper;

    QSignalMapper* togglePropertyMapper;

    QString previousDirectory; // previous directory that a file was opened in
    std::unique_ptr<QStackedWidget> mixerList;
    std::unique_ptr<QStackedWidget> playbackToolbarList;

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
        typedef std::function<void (std::vector<T*>&)>  ObjectsGetter;
        typedef std::function<bool (const T*)> PropertyGetter;
        typedef std::function<bool (T*, bool)> PropertySetter;

        ToggleablePropertyRecord(ObjectsGetter getObjects, PropertyGetter propertyGetter,
                                 PropertySetter propertySetter, const QString& propertyName) :
            getObjects(getObjects),
            propertyGetter(propertyGetter),
            propertySetter(propertySetter),
            propertyName(propertyName)
        {
        }

         // gets a list of the selected Notes, Positions, etc
        ObjectsGetter getObjects;

        // function to check if the property is set, e.g. Note::IsMuted
        PropertyGetter propertyGetter;

        // function to set the property as true/false, e.g. Note::SetMuted
        PropertySetter propertySetter;

        QString propertyName; // name of the property, for use with the undo menu
    };

    // Store all of the ToggleablePropertyRecords in one list, regardless of which type (e.g. Note, Position) that
    // the structure is instantiated for. Using boost::variant makes this possible, and allows to still retrieve the type
    // of the original object later on.
    // This allows us to use QSignalMapper to map all toggleable properties to a location in the toggleableProperties list,
    // and then perform an undo/redo action using the functions in the ToggleablePropertyRecord, after a signal is triggered
    typedef boost::variant< ToggleablePropertyRecord<Note>, ToggleablePropertyRecord<Position> > ToggleableProperty;
    QList<ToggleableProperty> toggleableProperties; 

    void connectTogglePropertyAction(QAction* action, const ToggleableProperty& propertyInfo);
    
    // Takes a ToggleablePropertyRecord from the 'toggleableProperties' list of boost::variant objects,
    // and performs an undo/redo action
    struct propertyEditor : public boost::static_visitor<>
    {
        template <typename T>
        void operator()(ToggleablePropertyRecord<T>& propertyRecord) const
        {
            std::vector<T*> objects;
            propertyRecord.getObjects(objects);  // get the object(s) we are modifying, such as Notes or Positions
            
            // Perform the undo/redo action
            undoManager->push(new ToggleProperty<T>(objects, propertyRecord.propertySetter,
                                                    propertyRecord.propertyGetter, propertyRecord.propertyName));
        }
    };
};

#endif // POWERTABEDITOR_H
