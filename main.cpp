#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setOrganizationName("CodeThesis");
    a.setApplicationName("ODBC Query Tool");
    a.setWindowIcon(QIcon(":/db_icon.ico"));
    MainWindow w;
    w.show();

    return a.exec();
}
