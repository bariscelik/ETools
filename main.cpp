#include "home.h"
#include <QApplication>
#include <QTranslator>
#include <QSplashScreen>
#include <QTimer>
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    home w;


    QTranslator *ts = new QTranslator;
    if(!ts->load(":/files/lang.qm"))
    {
        delete ts;
    }else{
        qApp->installTranslator(ts);
    }
    QPixmap pixmap(":/files/images/splash.jpg");
    QSplashScreen splash(pixmap);

    splash.show();

    a.processEvents(QEventLoop::AllEvents);

    QTimer::singleShot(2000,&splash,SLOT(close()));
    QTimer::singleShot(2000,&w,SLOT(show()));

    //w.show();


    return a.exec();
}
