#ifndef TOOLBOX_H
#define TOOLBOX_H

#include <QTabWidget>
#include <QToolBox>
#include <QVBoxLayout>

class NoteTab : public QWidget
{
	Q_OBJECT

public:
	NoteTab(QWidget *parent = 0);
};

class OrnamentTab : public QWidget
{
	Q_OBJECT

public:
	OrnamentTab(QWidget *parent = 0);
};

class ScoreTab : public QWidget
{
	Q_OBJECT

public:
	ScoreTab(QWidget *parent = 0);
};


class Toolbox : public QTabWidget
{
    Q_OBJECT
public:
    explicit Toolbox(QWidget *parent = 0);

signals:

public slots:

};

#endif // TOOLBOX_H
