#ifndef FILEINFORMATIONDIALOG_H
#define FILEINFORMATIONDIALOG_H

#include <QDialog>
#include <boost/shared_ptr.hpp>

class PowerTabDocument;
class PowerTabFileHeader;

namespace Ui {
class FileInformationDialog;
}

class FileInformationDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit FileInformationDialog(boost::shared_ptr<PowerTabDocument> doc, QWidget *parent = 0);
    ~FileInformationDialog();
    
private:
    Ui::FileInformationDialog *ui;

    QString getFileVersionString(const PowerTabFileHeader& header);
};

#endif // FILEINFORMATIONDIALOG_H
