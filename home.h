#ifndef HOME_H
#define HOME_H

#include <QMainWindow>
#include <beam.h>

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
    void on_commandLinkButton_7_clicked();

private:
    Ui::home *ui;
    beam beamSolver;
};

#endif // HOME_H
