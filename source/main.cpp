#include <QApplication>
#include "powertabeditor.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    PowerTabEditor w;
    w.show();

    return a.exec();
}
