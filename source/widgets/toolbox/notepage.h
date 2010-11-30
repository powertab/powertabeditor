#ifndef NOTEPAGE_H
#define NOTEPAGE_H

#include <QFrame>
#include <QPushButton>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QVBoxLayout>

#include "../../skinmanager.h"

class NotePage : public QFrame
{
	Q_OBJECT

public:
	NotePage(QFrame *parent = 0, SkinManager *skinManager = 0);

private:
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

	QPushButton *noteButton[7];

	QPushButton *restButton[7];

	QPushButton *dottedButton;
	QPushButton *doubleDottedButton;
	QPushButton *tieButton;
	QPushButton *groupingButton;
	QPushButton *fermataButton;

	QPushButton *slideInBelowButton;
	QPushButton *slideInAboveButton;
	QPushButton *slideOutBelowButton;
	QPushButton *slideOutAboveButton;
	QPushButton *shiftSlideButton;
	QPushButton *legatoSlideButton;
	QPushButton *legatoButton;

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
