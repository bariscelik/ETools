#include "home.h"
#include "ui_home.h"

home::home(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::home)
{
    ui->setupUi(this);
    setFixedSize(this->geometry().width(),this->geometry().height());
}

home::~home()
{
    delete ui;
}

void home::on_commandLinkButton_7_clicked()
{

}

void home::on_commandLinkButton_6_clicked()
{
    beamSolver.setWindowFlags(Qt::Window);
    beamSolver.showFullScreen();
}
