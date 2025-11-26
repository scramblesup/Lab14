#include "user.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    TMainWindow mw;
    mw.show();
    return a.exec();
}
