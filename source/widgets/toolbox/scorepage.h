#ifndef SCOREPAGE_H
#define SCOREPAGE_H

#include <QFrame>

#include <memory>

class SkinManager;
class QPushButton;
class QGroupBox;
class QHBoxLayout;
class QVBoxLayout;
class PowerTabEditor;

class ScorePage : public QFrame
{
    Q_OBJECT

public:
    ScorePage(PowerTabEditor* mainWindow, std::shared_ptr<SkinManager> skinManager, QFrame *parent = 0);

protected:
    PowerTabEditor* mainWindow;

    QVBoxLayout *layout;

    QHBoxLayout *songLayout;
    QHBoxLayout *sectionLayout;
    //QHBoxLayout *staffLayout;
    QHBoxLayout *flowDynamicsLayout;

    QGroupBox *songGroup;
    QGroupBox *sectionGroup;
    //QGroupBox *staffGroup;
    QGroupBox *flowDynamicsGroup;

    // song group
    QPushButton *addInstrumentButton;
    QPushButton *addPercussionButton;
    QPushButton *increaseHeightButton;
    QPushButton *decreaseHeightButton;

    // section group
    QPushButton *insertSectionBeforeButton;
    QPushButton *insertSectionAfterButton;
    QPushButton *removeSectionButton;
    QPushButton *addBarButton;
    QPushButton *increaseWidthButton;
    QPushButton *decreaseWidthButton;
    QPushButton *justifyButton;

    // staff group
    //QPushButton *

    // flow and dynamics group
    QPushButton *addRehearsalSignButton;
    QPushButton *addDirectionButton;
    QPushButton *addTempoMarkerButton;
    QPushButton *addVolumeMarkerButton;

    void createSongButtons();
    void createSectionButtons();
    //void createStaffButtons();
    void createFlowDynamicsButtons();
};

#endif // SCOREPAGE_H
