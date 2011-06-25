#ifndef SKINMANAGER_H
#define SKINMANAGER_H

#include <QString>

class SkinManager
{
public:
    SkinManager(const QString& filename);

    QString getDocumentTabStyle() const;
    QString getToolboxTabStyle() const;
    QString getToolboxPageStyle() const;
    QString getMixerStyle() const;

private:
    QString documentTabStyle;
    QString toolboxTabStyle, toolboxPageStyle;
    QString mixerStyle;

    QString readSegment(const QString& skinname, const QString& filename);
};

#endif // SKINMANAGER_H
