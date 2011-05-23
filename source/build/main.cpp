#include <QApplication>
#include <app/powertabeditor.h>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    PowerTabEditor w;
    w.show();

    return a.exec();
}
