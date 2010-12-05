#ifndef SCOREPAGE_H
#define SCOREPAGE_H

#include <QFrame>
#include <QPushButton>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QVBoxLayout>

#include "../../skinmanager.h"

class ScorePage : public QFrame
{
	Q_OBJECT

public:
	ScorePage(QFrame *parent = 0, SkinManager *skinManager = 0);

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
