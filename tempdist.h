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
#include <cmath>
#include <vector>
#include <iostream>
#include <qtypetraits.h>
#include <qcustomplot.h>

struct Line{
    int type; // 1 => constantbcond, 2 => convectionbcond, 3 => notransferbcond
    int temp;
    int h;    // heat transfer coefficient
    QGraphicsLineItem *item;
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
    void denemee(int t);
private slots:
    void on_actionSolve_triggered();
    void grsel();
    void xAxisRangeChanged(const QCPRange &newRange, const QCPRange &oldRange);
    void yAxisRangeChanged(const QCPRange &newRange, const QCPRange &oldRange);
    void on_actionDisplay_Mesh_on_off_changed();

public slots:
        void test();
        void plotDistribution();
        void drawMesh();
private:
    Ui::tempdist *ui;
    void solve(double tLeft, double tRight, double tTop, double tBottom, unsigned int h, unsigned int l);
    QGraphicsScene *scene;
    lineForm *f;
    QGraphicsTextItem *lLineLabel;
    QGraphicsTextItem *rLineLabel;
    QGraphicsTextItem *bLineLabel;
    QGraphicsTextItem *tLineLabel;
    QGraphicsItemGroup *meshLines = new QGraphicsItemGroup;
    void solveAll(unsigned int h, unsigned int l, double epsilon, double k);
    std::vector< std::vector<double> > res;
    bool _aborted = false;
    void solveMonteCarlo(unsigned int h, unsigned int l);
    int it;
};

#endif // TEMPDIST_H
