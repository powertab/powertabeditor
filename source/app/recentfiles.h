#ifndef RECENTFILES_H
#define RECENTFILES_H

#include <QStringList>
class QMenu;

class RecentFiles : public QObject
{
    Q_OBJECT
public:
    RecentFiles(QMenu* recentFilesMenu, QObject* parent = 0);
    ~RecentFiles();

    void add(const QString& fileName);

signals:
    void fileSelected(const QString& fileName);

private slots:
    void clear();
    void handleFileSelection(const QString& fileName);

private:
    QMenu* recentFilesMenu;
    QStringList recentFiles;

    void updateMenu();
};

#endif // RECENTFILES_H
