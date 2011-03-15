#ifndef DOCUMENTMANAGER_H
#define DOCUMENTMANAGER_H

#include <QList>
#include <memory>

class QString;
class PowerTabDocument;

// Class for keeping track of the opened documents
// (this is necessary since we can have multiple tabs)

class DocumentManager
{
public:
    DocumentManager();
    ~DocumentManager();

    bool addDocument(const QString& fileName);
    void createDocument();
    void removeDocument(int index);
    std::shared_ptr<PowerTabDocument> getCurrentDocument();
    void setCurrentDocumentIndex(int index);
    int getCurrentDocumentIndex() const;

private:
    QList<std::shared_ptr<PowerTabDocument> > documentList;
    int currentDocumentIndex;
};

#endif // DOCUMENTMANAGER_H
