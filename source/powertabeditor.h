#ifndef POWERTABEDITOR_H
#define POWERTABEDITOR_H

#include <QMainWindow>

#include "documentmanager.h"
#include "dialogs/preferencesdialog.h"

class QTabWidget;
class QUndoStack;
class ScoreArea;

class PowerTabEditor : public QMainWindow
{
    Q_OBJECT

public:
    PowerTabEditor(QWidget *parent = 0);
    static void RefreshCurrentDocument();
    static QTabWidget* tabWidget;
    static QUndoStack* undoStack;
    ScoreArea* getCurrentScoreArea();

private:
    void CreateActions();
    void CreateMenus();
    void CreateTabArea();

private slots:
    void OpenFile();
	void OpenPreferences();
    void RefreshOnUndoRedo(int);
    void closeTab(int index);
    void switchTab(int index);
    void moveCaretRight();
    void moveCaretLeft();
    void moveCaretDown();
    void moveCaretUp();
    void moveCaretToStart();
    void moveCaretToEnd();

    void moveCaretToFirstSection();
    void moveCaretToNextSection();
    void moveCaretToPrevSection();
    void moveCaretToLastSection();

private:
    DocumentManager documentManager;
    QMenu* fileMenu;
    QAction* openFileAct;
	QAction* preferencesAct;
    QAction* exitAppAct;

    QMenu* editMenu;
    QAction* undoAct;
    QAction* redoAct;

    QMenu* positionMenu;
    QMenu* positionSectionMenu; // menu options for navigating between sections
    // navigate to the first, next, previous, or last section in the score
    QAction* firstSectionAct;
    QAction* nextSectionAct;
    QAction* prevSectionAct;
    QAction* lastSectionAct;

    QMenu* positionStaffMenu;
    QAction* nextPositionAct; // navigate to the next position in the staff
    QAction* prevPositionAct; // navigate to the previous position in the staff
    QAction* startPositionAct; // navigate to the first position in the staff
    QAction* lastPositionAct; // navigate to the last position in the staff
    QAction* nextStringAct; // navigate to the next string in the staff
    QAction* prevStringAct; // navigate to the previous string in the staff

	PreferencesDialog* preferencesDialog;
};

#endif // POWERTABEDITOR_H
