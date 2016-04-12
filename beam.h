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
  double x1;
  double x2;
  QString f;
};

struct Moment : Component{
  float mag;
  bool directionCw; // clockwise direction
};

Q_DECLARE_METATYPE(Force)
Q_DECLARE_METATYPE(Support)
Q_DECLARE_METATYPE(Moment)
Q_DECLARE_METATYPE(DistLoad)

namespace Ui {
class beam;
}

class beam : public QMainWindow
{
    Q_OBJECT

public:
    explicit beam(QWidget *parent = 0);
    ~beam();

public slots:
    void showPointToolTip(QMouseEvent *event);
private slots:
    void on_solveBtn_clicked();
    void on_treeWidget_currentItemChanged(QTreeWidgetItem *current, QTreeWidgetItem *previous);
    void on_treeWidget_customContextMenuRequested(const QPoint &pos);
    void on_actionForce_triggered();
    void on_actionDistributed_Load_triggered();
    void on_actionMoment_triggered();
    void on_action_Save_triggered();
    void componentsActionDelete();
    void on_actionSave_as_triggered();

private:
    Ui::beam *ui;
    QGraphicsScene *scene;
    QList<Force> forces;
    QList<Support> supports;
    QList<DistLoad> distributedloads;
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
    QGraphicsItem *drawDistLoad(DistLoad dl);
    void addDistLoad(DistLoad dl);
};

#endif // BEAM_H
