#include "QtGLCore.h"
#include <elaWidget/ElaApplication.h>
#include <QtWidgets/QApplication>
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    ElaApplication::getInstance()->init();
    QtGLCore w;
    w.show();
    return a.exec();
}
