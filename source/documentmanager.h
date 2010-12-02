#ifndef DOCUMENTMANAGER_H
#define DOCUMENTMANAGER_H

#include <QList>

class QString;
class PowerTabDocument;

// Class for keeping track of the opened documents
// (this is necessary since we can have multiple tabs)

class DocumentManager
{
public:
    DocumentManager();
    ~DocumentManager();

    bool Add(QString& fileName);
    void Remove(quint32 index);
    PowerTabDocument* getCurrentDocument();
    void setCurrentDocumentIndex(quint32 index);
    quint32 getCurrentDocumentIndex() const
    {
        return currentDocumentIndex;
    }

private:
    QList<PowerTabDocument*> documentList;
    quint32 currentDocumentIndex;
};

#endif // DOCUMENTMANAGER_H
