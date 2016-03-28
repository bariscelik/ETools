#ifndef BEAM_H
#define BEAM_H

#include <QMainWindow>
#include <QHBoxLayout>
#include <QToolBar>
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

unsigned const int beamL = 800;

struct Component
{
    QGraphicsItem *item;
};

struct Force : Component{
  float posX;
  float mag;
  float angle;
  bool directionUp;
};

struct Support : Component{
  float posX;
  int type;
  float fx;
  float fy;
};

struct DistLoad : Component{
  float x;
};

struct Moment : Component{
  float mag;
  bool directionCw; // clockwise direction
};

Q_DECLARE_METATYPE(Force)
Q_DECLARE_METATYPE(Support)
Q_DECLARE_METATYPE(Moment)

namespace Ui {
class beam;
}

class beam : public QMainWindow
{
    Q_OBJECT

public:
    explicit beam(QWidget *parent = 0);
    ~beam();
    enum ForceTypes
    {
        SingleForce = 1,
        DistributedLoad =2
    };

public slots:
    void showPointToolTip(QMouseEvent *event);
private slots:
    void on_pushButton_2_clicked();
    void on_addForceBtn_clicked();
    void on_solveBtn_clicked();
    void on_treeWidget_currentItemChanged(QTreeWidgetItem *current, QTreeWidgetItem *previous);
    void on_treeWidget_customContextMenuRequested(const QPoint &pos);
private:
    Ui::beam *ui;
    QGraphicsScene *scene;
    QList<Force> forces;
    QList<Support> supports;
    QList<Moment> moments;
    QList< QPair<float,float> > resForces;
    QGraphicsItem *drawSupport(Support s);
    QGraphicsItem *drawSingleForce(Force f);
    QGraphicsItem *drawMoment(Moment m);
    void addSingleForce(Force force);
    void addSupport(Support support);
    void addMoment(Moment moment);
    void reDrawCPanel();
    void reDrawScene();
    void plotDiagrams(QList<QPair<float, float> > points);
    float realBeamL;
    float LFactor;
};

#endif // BEAM_H
