#include "mainwindow.h"

#include <QApplication>

int main(int argc, char* argv[])
{
//#if (QT_VERSION >= QT_VERSION_CHECK(6, 0, 0))
//    //设置高分屏缩放舍入策略
//    QApplication::setHighDpiScaleFactorRoundingPolicy(Qt::HighDpiScaleFactorRoundingPolicy::Floor);
//#endif

    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return a.exec();
}
