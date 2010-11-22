#ifndef TOOLBOX_H
#define TOOLBOX_H

#include <QTabWidget>
#include <QToolBox>
#include <QPushButton>
#include <QVBoxLayout>

class NoteTab : public QWidget
{
	Q_OBJECT

public:
	NoteTab(QWidget *parent = 0);

private:
	QPushButton *note1_button;
	QPushButton *note2_button;
	QPushButton *note4_button;
	QPushButton *note8_button;
	QPushButton *note16_button;
	QPushButton *note32_button;

	QPushButton *rest1_button;
	QPushButton *rest2_button;
	QPushButton *rest4_button;
	QPushButton *rest8_button;
	QPushButton *rest16_button;
	QPushButton *rest32_button;

private slots:
	void ResetNoteAndRestButtons();
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
