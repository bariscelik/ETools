#ifndef TEMPDIST_H
#define TEMPDIST_H

#include <QMainWindow>
#include <QtCore>
#include <QtGui>
#include <QGraphicsScene>
#include <QGraphicsEllipseItem>
#include <QGraphicsItem>
#include <QGraphicsTextItem>
#include <QGraphicsLineItem>
#include <QGraphicsRectItem>
#include <QGraphicsItemGroup>
#include <QGraphicsPolygonItem>
#include <QGraphicsPathItem>
#include <QTreeWidgetItem>
#include <lineform.h>

struct Line{
    int type; // 1 => constantbcond, 2 => convectionbcond, 3 => notransferbcond
    int temp;
    int h;    // heat transfer coefficient
};

namespace Ui {
class tempdist;
}

class tempdist : public QMainWindow
{
    Q_OBJECT

public:
    explicit tempdist(QWidget *parent = 0);
    ~tempdist();
    QList<Line> lines;
private slots:
    void on_actionSolve_triggered();
    void on_graphicsview_selectChange();

public slots:
        void test();

private:
    Ui::tempdist *ui;
    void solve(double tLeft, double tRight, double tTop, double tBottom, unsigned int h, unsigned int l);
    QGraphicsScene *scene;
    lineForm *f;
    QGraphicsTextItem *lLineLabel;
    QGraphicsTextItem *rLineLabel;
    QGraphicsTextItem *bLineLabel;
    QGraphicsTextItem *tLineLabel;
    void solveConvection(double tLeft, double tRight, double tTop, double tBottom, double ht, double k, unsigned int h, unsigned int l);
    void solveAll(double tLeft, double tRight, double tTop, double tBottom, double ht, double k, unsigned int h, unsigned int l);
};

#endif // TEMPDIST_H
