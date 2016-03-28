#include "home.h"
#include "ui_home.h"
#include <QDialog>

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
    beamSolver.show();
}

void home::on_commandLinkButton_clicked()
{
    tempDist.show();

    //setup all your labels and layout ...

    /*
    QDialog *about = new QDialog(0,0);
    about->setModal(true);

    QHBoxLayout *mainLayout = new QHBoxLayout;
    mainLayout->addWidget(new QPushButton("asdasd"));

    about->setLayout(mainLayout);
    about->show();
    */
}
