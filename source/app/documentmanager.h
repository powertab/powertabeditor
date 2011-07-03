#ifndef DOCUMENTMANAGER_H
#define DOCUMENTMANAGER_H

#include <vector>
#include <memory>

class QString;
class PowerTabDocument;

/// Class for managing opened documents
class DocumentManager
{
public:
    DocumentManager();

    bool addDocument(const QString& fileName);
    bool addImportedDocument(std::shared_ptr<PowerTabDocument> doc);

    void createDocument();
    void removeDocument(int index);
    std::shared_ptr<PowerTabDocument> getCurrentDocument() const;
    void setCurrentDocumentIndex(int index);
    int getCurrentDocumentIndex() const;

private:
    std::vector<std::shared_ptr<PowerTabDocument> > documentList;
    int currentDocumentIndex;
};

#endif // DOCUMENTMANAGER_H
