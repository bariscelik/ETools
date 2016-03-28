#include "lineform.h"
#include "ui_lineform.h"
#include <iostream>

lineForm::lineForm(int _id, int _type, int _h, int _temp, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::lineForm)
{
    ui->setupUi(this);
    selectType(_type);
    id = _id;
    temp = _temp;
    h = _h;
    ui->lineEdit->setText(QString::number(temp));
    ui->lineEdit_2->setText(QString::number(h));
}

lineForm::~lineForm()
{
    delete ui;
}

void lineForm::selectType(int t)
{

    ui->label->setDisabled(true);
    ui->label_2->setDisabled(true);
    ui->lineEdit->setDisabled(true);
    ui->lineEdit_2->setDisabled(true);

    ui->radioButton_3->setChecked(true);
    if(t==1)
    {
        ui->radioButton->setChecked(true);
        ui->label->setEnabled(true);
        ui->lineEdit->setEnabled(true);
    }else if(t==2)
    {
        ui->radioButton_2->setChecked(true);
        ui->label->setEnabled(true);
        ui->lineEdit->setEnabled(true);
        ui->label_2->setEnabled(true);
        ui->lineEdit_2->setEnabled(true);
    }
    type = t;
}

void lineForm::on_radioButton_clicked()
{
    selectType(1);
}

void lineForm::on_radioButton_2_clicked()
{
    selectType(2);
}

void lineForm::on_radioButton_3_clicked()
{
    selectType(3);
}
/*
void lineForm::accept()
{
    temp = ui->lineEdit->text().toInt();
    h = ui->lineEdit_2->text().toInt();
    this->setResult(QDialog::Accepted);
    this->close();
}
*/
void lineForm::on_lineEdit_2_textChanged(const QString &arg1)
{
    h = arg1.toInt();
}

void lineForm::on_lineEdit_textChanged(const QString &arg1)
{
    temp = arg1.toInt();
}
