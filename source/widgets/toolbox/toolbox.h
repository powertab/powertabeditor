#ifndef TOOLBOX_H
#define TOOLBOX_H

#include <QTabWidget>

#include "skinmanager.h"
#include "widgets/toolbox/notepage.h"
#include "widgets/toolbox/scorepage.h"

class Toolbox : public QTabWidget
{
    Q_OBJECT
public:
	explicit Toolbox(QWidget *parent = 0, SkinManager *skinManager=0);

private:
	static ScorePage *scorePage;
	static NotePage *notePage;

signals:

public slots:

};

#endif // TOOLBOX_H
