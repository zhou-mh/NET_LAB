#include "clientwidget.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    ClientWidget w;
    w.setAutoFillBackground(true);
    QPalette palette;
    QPixmap pixmap("../ILLW_SOURCE/back.png");
    palette.setBrush(QPalette::Window, QBrush(pixmap));
    w.setPalette(palette);
    w.show();


    return a.exec();
}
