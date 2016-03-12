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
#include <QGraphicsTextItem>
#include <QGraphicsLineItem>
#include <QGraphicsRectItem>
#include <QGraphicsItemGroup>
#include <QGraphicsPolygonItem>
#include <QTreeWidgetItem>

struct Force{
  float posX;
  float mag;
  float angle;
  bool directionUp;
};

struct Support{
  float posX;
  int type;
  float fx;
  float fy;
};

Q_DECLARE_METATYPE(Force)
Q_DECLARE_METATYPE(Support)

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

    void on_solveBtn_clicked();
    void on_treeWidget_currentItemChanged(QTreeWidgetItem *current, QTreeWidgetItem *previous);

private:
    Ui::beam *ui;
    QGraphicsScene *scene;
    QGraphicsEllipseItem *ellipse;
    QGraphicsRectItem *rectangle;
    QGraphicsLineItem *xline;
    QList<Force> forces;
    QList<Support> supports;
    void drawSupports(int xpos, int ypos);
    void addSingleForce(Force force);
    void drawSingleForce(Force force);
    void reDrawCPanel();
    void addSupport(Support support);
};

#endif // BEAM_H
