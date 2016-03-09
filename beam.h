#ifndef BEAM_H
#define BEAM_H

#include <QMainWindow>
#include <QHBoxLayout>
#include <QToolBar>
#include <QDialog>
#include <QtCore>
#include <QtGui>
#include <QGraphicsScene>
#include <QGraphicsEllipseItem>
#include <QGraphicsItem>
#include <QGraphicsLineItem>
#include <QGraphicsRectItem>
#include <QGraphicsItemGroup>
#include <QGraphicsPolygonItem>

namespace Ui {
class beam;
}

class beam : public QDialog
{
    Q_OBJECT

public:
    explicit beam(QWidget *parent = 0);
    ~beam();

private slots:
    void on_pushButton_2_clicked();

    void on_addForceBtn_clicked();

private:
    Ui::beam *ui;
    QGraphicsScene *scene;
    QGraphicsEllipseItem *ellipse;
    QGraphicsRectItem *rectangle;
    QGraphicsLineItem *xline;
    void drawSupports(int xpos, int ypos);
    void drawSingleForce(int posX, QString mag, int angle);
};

#endif // BEAM_H
