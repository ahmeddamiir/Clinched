#include "mainwindow.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setApplicationName("Clinched");
    MainWindow w;
    w.show();
    return QApplication::exec();
}
