#ifndef SCOREPAGE_H
#define SCOREPAGE_H

#include <QFrame>

#include "../../skinmanager.h"

class ScorePage : public QFrame
{
	Q_OBJECT

public:
	ScorePage(QFrame *parent = 0, SkinManager *skinManager = 0);
};

#endif // SCOREPAGE_H
