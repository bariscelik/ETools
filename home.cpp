#include "home.h"
#include "ui_home.h"
#include <QDialog>
#include <QMessageBox>

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

void home::on_actionAbout_triggered()
{
    QMessageBox::about(this,tr("About ETools"),tr("<p>ETools is an open source software to solve basic engineering problems.</p> <p>It can calculate reactions, elastic deformation, slope, deflection of isostatic(statically determinate) and hyperstatic(statically indeterminate) systems, and temperature distrubition on known formed metal plate.</p> If you found a bug, please tell me.<br><a href='mailto:bariscelikweb@gmail.com'>bariscelikweb@gmail.com</a><br><a href='http://www.bariscelik.com.tr'>Web Site</a><br><a href='https://github.com/bariscelik/ETools'>Github Page</a>"));
}

void home::on_beamAnalysis_clicked()
{
    beamSolver.setWindowFlags(Qt::Window);
    beamSolver.show();
}

void home::on_tempDist_clicked()
{

    tempDist.show();
}
