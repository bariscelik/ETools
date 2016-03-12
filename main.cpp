#include "home.h"
#include <QApplication>
#include <QTranslator>
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QTranslator *ts = new QTranslator;
    if(!ts->load(":/files/lang.qm"))
    {
        delete ts;
    }else{
        qApp->installTranslator(ts);
    }
    home w;
    w.show();

    return a.exec();
}
