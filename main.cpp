#include "home.h"
#include <QApplication>
#include <QTranslator>
#include <QSplashScreen>
#include <QTimer>
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);



    QTranslator *ts = new QTranslator;
    if(!ts->load(":/files/lang.qm"))
    {
        delete ts;
    }else{
        a.installTranslator(ts);
    }

    home w;

    QPixmap pixmap(":/files/images/splash.jpg");
    QSplashScreen splash(pixmap);

    splash.show();

    a.processEvents(QEventLoop::AllEvents);

    QTimer::singleShot(1000,&splash,SLOT(close()));
    QTimer::singleShot(1000,&w,SLOT(show()));

    return a.exec();
}
