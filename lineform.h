#ifndef LINEFORM_H
#define LINEFORM_H

#include <QDialog>

namespace Ui {
class lineForm;
}

class lineForm : public QDialog
{
    Q_OBJECT

public:
    explicit lineForm(int _id, int _type, int _h, int _temp, QWidget *parent = 0);
    ~lineForm();
    void selectType(int t = 1);
    int id;
    int type;
    int temp;
    int h;
private slots:
    void on_radioButton_clicked();
    void on_radioButton_2_clicked();
    void on_radioButton_3_clicked();
    //void accept();
    void on_lineEdit_2_textChanged(const QString &arg1);

    void on_lineEdit_textChanged(const QString &arg1);

private:
    Ui::lineForm *ui;
};

#endif // LINEFORM_H
