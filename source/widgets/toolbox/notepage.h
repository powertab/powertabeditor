#ifndef NOTEPAGE_H
#define NOTEPAGE_H

#include <QFrame>

#include <memory>

class SkinManager;
class QPushButton;
class QGroupBox;
class QHBoxLayout;
class QVBoxLayout;
class PowerTabEditor;

class NotePage : public QFrame
{
    Q_OBJECT

public:
    NotePage(PowerTabEditor* mainWindow, std::shared_ptr<SkinManager> skinManager, QFrame *parent = 0);

private:
    PowerTabEditor* mainWindow;
    QVBoxLayout *layout;

    QHBoxLayout *noteLayout;
    QHBoxLayout *restLayout;
    QHBoxLayout *rhythmLayout;
    QHBoxLayout *slideLegatoLayout;
    QHBoxLayout *vibratoTremoloLayout;

    QGroupBox *noteGroup;
    QGroupBox *restGroup;
    QGroupBox *rhythmGroup;
    QGroupBox *slideLegatoGroup;
    QGroupBox *vibratoTremoloGroup;

    // note group
    QPushButton *noteButton[7];

    // rest group
    QPushButton *restButton[7];

    // rhythm group
    QPushButton *dottedButton;
    QPushButton *doubleDottedButton;
    QPushButton *tieButton;
    QPushButton *groupingButton;
    QPushButton *fermataButton;

    // slide and legato group
    QPushButton *slideInBelowButton;
    QPushButton *slideInAboveButton;
    QPushButton *slideOutBelowButton;
    QPushButton *slideOutAboveButton;
    QPushButton *shiftSlideButton;
    QPushButton *legatoSlideButton;
    QPushButton *legatoButton;

    // vibrato and tremolo group
    QPushButton *slightVibratoButton;
    QPushButton *wideVibratoButton;
    QPushButton *bendButton;
    QPushButton *trillButton;
    QPushButton *tremoloButton;

    void createNoteButtons();
    void createRestButtons();
    void createRhythmButtons();
    void createSlideLegatoButtons();
    void createVibratoTremoloButtons();

private slots:
    void resetNoteAndRestButtons();
    void resetDottedButtons();
    void resetSlideLegatoButtons();
};

#endif // NOTEPAGE_H
