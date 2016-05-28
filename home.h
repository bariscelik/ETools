#ifndef HOME_H
#define HOME_H

#include <QMainWindow>
#include <beam.h>
#include <tempdist.h>

namespace Ui {
class home;
}

class home : public QMainWindow
{
    Q_OBJECT

public:
    explicit home(QWidget *parent = 0);
    ~home();

private slots:
    void on_actionAbout_triggered();
    void on_beamAnalysis_clicked();
    void on_tempDist_clicked();

private:
    Ui::home *ui;
    beam beamSolver;
    tempdist tempDist;
};

#endif // HOME_H
