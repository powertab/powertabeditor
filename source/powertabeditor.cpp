#include <QFileDialog>
#include <QDebug>
#include <QAction>
#include <QMenuBar>
#include <QMenu>
#include <QListView> // TODO - remove

#include <QTabWidget>
#include <QUndoStack>

#include <QFileInfo>
#include <QFontDatabase>

#include "powertabeditor.h"
#include "scorearea.h"
#include "painters/caret.h"

QTabWidget* PowerTabEditor::tabWidget = NULL;
QUndoStack* PowerTabEditor::undoStack = NULL;
QSplitter* PowerTabEditor::vertSplitter = NULL;
QSplitter* PowerTabEditor::horSplitter = NULL;

PowerTabEditor::PowerTabEditor(QWidget *parent) :
        QMainWindow(parent)
{
    // load fonts from the resource file
    QFontDatabase::addApplicationFont(":fonts/emmentaler-13.otf"); // used for music notation
    QFontDatabase::addApplicationFont(":fonts/LiberationSans-Regular.ttf"); // used for tab notes

    undoStack = new QUndoStack();
    connect(undoStack, SIGNAL(indexChanged(int)), this, SLOT(RefreshOnUndoRedo(int)));

    CreateActions();
    CreateMenus();
    CreateTabArea();

	preferencesDialog = new PreferencesDialog();

	setMinimumSize(800, 600);
	setWindowState(Qt::WindowMaximized);
	setWindowTitle(tr("Power Tab Editor"));

	horSplitter = new QSplitter();
	horSplitter->setOrientation(Qt::Horizontal);

	QTabWidget *tmp1 = new QTabWidget;
	tmp1->setMaximumWidth(300);
	horSplitter->addWidget(tmp1);
	horSplitter->addWidget(tabWidget);

	vertSplitter = new QSplitter();
	vertSplitter->setOrientation(Qt::Vertical);

	vertSplitter->addWidget(horSplitter);

	QListView *tmp2 = new QListView;
	tmp2->setMaximumHeight(200);
	vertSplitter->addWidget(tmp2);

	setCentralWidget(vertSplitter);
}

// Redraws the *entire* document upon undo/redo
// TODO - notify the appropriate painter to redraw itself, instead
// of redrawing the whole score
void PowerTabEditor::RefreshOnUndoRedo(int index)
{
    Q_UNUSED(index);
    RefreshCurrentDocument();
}

void PowerTabEditor::CreateActions()
{
    // File-related actions
    openFileAct = new QAction(tr("&Open..."), this);
    openFileAct->setShortcuts(QKeySequence::Open);
    openFileAct->setStatusTip(tr("Open an existing document"));
    connect(openFileAct, SIGNAL(triggered()), this, SLOT(OpenFile()));

	preferencesAct = new QAction(tr("&Preferences..."), this);
	preferencesAct->setShortcuts(QKeySequence::Preferences);
	connect(preferencesAct, SIGNAL(triggered()), this, SLOT(OpenPreferences()));

    // Exit the application
    exitAppAct = new QAction(tr("&Quit"), this);
    exitAppAct->setShortcuts(QKeySequence::Quit);
    exitAppAct->setStatusTip(tr("Exit the application"));
    connect(exitAppAct, SIGNAL(triggered()), this, SLOT(close()));

    // Redo / Undo actions
    undoAct = undoStack->createUndoAction(this, tr("&Undo"));
    undoAct->setShortcuts(QKeySequence::Undo);

    redoAct = undoStack->createRedoAction(this, tr("&Redo"));
    redoAct->setShortcuts(QKeySequence::Redo);

    // Section navigation actions
    firstSectionAct = new QAction(tr("First Section"), this);
    firstSectionAct->setShortcuts(QKeySequence::MoveToStartOfDocument);
    connect(firstSectionAct, SIGNAL(triggered()), this, SLOT(moveCaretToFirstSection()));

    nextSectionAct = new QAction(tr("Next Section"), this);
    nextSectionAct->setShortcuts(QKeySequence::MoveToNextPage);
    connect(nextSectionAct, SIGNAL(triggered()), this, SLOT(moveCaretToNextSection()));

    prevSectionAct = new QAction(tr("Previous Section"), this);
    prevSectionAct->setShortcuts(QKeySequence::MoveToPreviousPage);
    connect(prevSectionAct, SIGNAL(triggered()), this, SLOT(moveCaretToPrevSection()));

    lastSectionAct = new QAction(tr("Last Section"), this);
    lastSectionAct->setShortcuts(QKeySequence::MoveToEndOfDocument);
    connect(lastSectionAct, SIGNAL(triggered()), this, SLOT(moveCaretToLastSection()));

    // Position-related actions
    startPositionAct = new QAction(tr("Move to &Start"), this);
    startPositionAct->setShortcuts(QKeySequence::MoveToStartOfLine);
    connect(startPositionAct, SIGNAL(triggered()), this, SLOT(moveCaretToStart()));

    nextPositionAct = new QAction(tr("&Next Position"), this);
    nextPositionAct->setShortcuts(QKeySequence::MoveToNextChar);
    connect(nextPositionAct, SIGNAL(triggered()), this, SLOT(moveCaretRight()));

    prevPositionAct = new QAction(tr("&Previous Position"), this);
    prevPositionAct->setShortcuts(QKeySequence::MoveToPreviousChar);
    connect(prevPositionAct, SIGNAL(triggered()), this, SLOT(moveCaretLeft()));

    nextStringAct = new QAction(tr("Next String"), this);
    nextStringAct->setShortcuts(QKeySequence::MoveToNextLine);
    connect(nextStringAct, SIGNAL(triggered()), this, SLOT(moveCaretDown()));

    prevStringAct = new QAction(tr("Previous String"), this);
    prevStringAct->setShortcuts(QKeySequence::MoveToPreviousLine);
    connect(prevStringAct, SIGNAL(triggered()), this, SLOT(moveCaretUp()));

    lastPositionAct = new QAction(tr("Move to &End"), this);
    lastPositionAct->setShortcuts(QKeySequence::MoveToEndOfLine);
    connect(lastPositionAct, SIGNAL(triggered()), this, SLOT(moveCaretToEnd()));
}

void PowerTabEditor::CreateMenus()
{
    // File Menu
    fileMenu = menuBar()->addMenu(tr("&File"));
    fileMenu->addAction(openFileAct);
    fileMenu->addSeparator();
	fileMenu->addAction(preferencesAct);
	fileMenu->addSeparator();
    fileMenu->addAction(exitAppAct);

    // Edit Menu
    editMenu = menuBar()->addMenu(tr("&Edit"));
    editMenu->addAction(undoAct);
    editMenu->addAction(redoAct);

    // Position Menu
    positionMenu = menuBar()->addMenu(tr("&Position"));

    positionSectionMenu = positionMenu->addMenu(tr("&Section"));
    positionSectionMenu->addAction(firstSectionAct);
    positionSectionMenu->addAction(nextSectionAct);
    positionSectionMenu->addAction(prevSectionAct);
    positionSectionMenu->addAction(lastSectionAct);

    positionStaffMenu = positionMenu->addMenu(tr("&Staff"));
    positionStaffMenu->addAction(startPositionAct);
    positionStaffMenu->addAction(nextPositionAct);
    positionStaffMenu->addAction(prevPositionAct);
    positionStaffMenu->addAction(nextStringAct);
    positionStaffMenu->addAction(prevStringAct);
    positionStaffMenu->addAction(lastPositionAct);
}

void PowerTabEditor::CreateTabArea()
{
    tabWidget = new QTabWidget;
    tabWidget->setTabsClosable(true);

	QFile data(":stylesheets/tab.txt");
	QString style;

	if(data.open(QFile::ReadOnly))
	{
		QTextStream styleIn(&data);
		style = styleIn.readAll();
		data.close();
		tabWidget->setStyleSheet(style);
	}

    // creates a new document by default
    /*ScoreArea* score = new ScoreArea;
    score->RenderDocument(documentManager.getCurrentDocument());
    tabWidget->addTab(score, tr("Untitled"));*/

    connect(tabWidget, SIGNAL(tabCloseRequested(int)), this, SLOT(closeTab(int)));
    connect(tabWidget, SIGNAL(currentChanged(int)), this, SLOT(switchTab(int)));
}

// Open a new file
void PowerTabEditor::OpenFile()
{
    QString fileFilter;
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open"), QDir::homePath(), fileFilter);

    if (fileName.isEmpty())
    {
        qDebug() << "No file selected";
    }
    else
    {
        qDebug() << "Opening file: " << fileName;
        bool success = documentManager.Add(fileName); // add the file to the document manager

        // draw the score if it was successful
        if (success)
        {
            ScoreArea* score = new ScoreArea;
            score->RenderDocument(documentManager.getCurrentDocument());
            QFileInfo fileInfo(fileName);
			tabWidget->addTab(score, fileInfo.fileName());
        }

        // switch to the new document
        tabWidget->setCurrentIndex(documentManager.getCurrentDocumentIndex());
    }


}

// Opens the preferences dialog
void PowerTabEditor::OpenPreferences()
{
	preferencesDialog->exec();
}

// Redraws the whole score of the current document
void PowerTabEditor::RefreshCurrentDocument()
{
    ScoreArea* currentDoc = reinterpret_cast<ScoreArea *>(tabWidget->currentWidget());
    currentDoc->RenderDocument();
}

// Close a document and clean up
void PowerTabEditor::closeTab(int index)
{
    documentManager.Remove(index);
    delete tabWidget->widget(index);
    tabWidget->removeTab(index);

    documentManager.setCurrentDocumentIndex(tabWidget->currentIndex());
}

// When the tab is switched, switch the current document in the document manager
void PowerTabEditor::switchTab(int index)
{
    documentManager.setCurrentDocumentIndex(index);
}

ScoreArea* PowerTabEditor::getCurrentScoreArea()
{
    return reinterpret_cast<ScoreArea*>(tabWidget->currentWidget());
}

void PowerTabEditor::moveCaretRight()
{
    getCurrentScoreArea()->getCaret()->moveCaretHorizontal(1);
}

void PowerTabEditor::moveCaretLeft()
{
    getCurrentScoreArea()->getCaret()->moveCaretHorizontal(-1);
}

void PowerTabEditor::moveCaretDown()
{
    getCurrentScoreArea()->getCaret()->moveCaretVertical(1);
}

void PowerTabEditor::moveCaretUp()
{
    getCurrentScoreArea()->getCaret()->moveCaretVertical(-1);
}

void PowerTabEditor::moveCaretToStart()
{
    getCurrentScoreArea()->getCaret()->moveCaretToStart();
}

void PowerTabEditor::moveCaretToEnd()
{
    getCurrentScoreArea()->getCaret()->moveCaretToEnd();
}

void PowerTabEditor::moveCaretToFirstSection()
{
    getCurrentScoreArea()->getCaret()->moveCaretToFirstSection();
}

void PowerTabEditor::moveCaretToNextSection()
{
    getCurrentScoreArea()->getCaret()->moveCaretSection(1);;
}

void PowerTabEditor::moveCaretToPrevSection()
{
    getCurrentScoreArea()->getCaret()->moveCaretSection(-1);
}

void PowerTabEditor::moveCaretToLastSection()
{
    getCurrentScoreArea()->getCaret()->moveCaretToLastSection();
}
