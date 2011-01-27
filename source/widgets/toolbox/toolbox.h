#ifndef TOOLBOX_H
#define TOOLBOX_H

#include <QTabWidget>

#include <memory>

class NotePage;
class ScorePage;
class SkinManager;
class PowerTabEditor;

class Toolbox : public QTabWidget
{
    Q_OBJECT
public:
    Toolbox(PowerTabEditor* mainWindow, std::shared_ptr<SkinManager> skinManager);

private:
    static ScorePage *scorePage;
    static NotePage *notePage;

signals:

public slots:

};

#endif // TOOLBOX_H
