#include "acinjectorui.h"

#include <QApplication>
#include <QPalette>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    ACInjectorUI w;

    //Set the background color to white
    QPalette pal;
    pal.setColor(QPalette::Background, Qt::white);
    w.setAutoFillBackground(true);
    w.setPalette(pal);

    //Show and execute func's
    w.show();
    return a.exec();
}
