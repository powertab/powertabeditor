#ifndef TOOLBOX_H
#define TOOLBOX_H

#include <QTabWidget>

class NotePage;
class ScorePage;
class SkinManager;
class PowerTabEditor;

class Toolbox : public QTabWidget
{
    Q_OBJECT
public:
    explicit Toolbox(PowerTabEditor* mainWindow, SkinManager *skinManager = 0, QWidget *parent = 0);

private:
    static ScorePage *scorePage;
    static NotePage *notePage;

signals:

public slots:

};

#endif // TOOLBOX_H
