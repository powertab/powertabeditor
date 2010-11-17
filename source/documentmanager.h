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
    void Remove(uint32_t index);
    PowerTabDocument* getCurrentDocument();
    void setCurrentDocumentIndex(uint32_t index);
    uint32_t getCurrentDocumentIndex() const
    {
        return currentDocumentIndex;
    }

private:
    QList<PowerTabDocument*> documentList;
    uint32_t currentDocumentIndex;
};

#endif // DOCUMENTMANAGER_H
