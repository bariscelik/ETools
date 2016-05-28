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
#include "qtpropertymanager.h"
#include "qtvariantproperty.h"
#include "qttreepropertybrowser.h"
#include "ui_materialproperties.h"
#include "ui_distributedloaddialog.h"
#include <armadillo>

const double beamL = 800;

struct Component
{
    QGraphicsItem *item;
};

struct Force : Component{
  float posX;
  float mag;
  double angle;
  bool directionUp;
};

struct Support : Component{
  double posX;
  enum t {FIXED,ROLLER,PINNED};
  t type;
};

struct DistLoad : Component{
  double x1;
  double x2;

  std::vector< double > data = {1,2};

  enum LoadTypes { UNIFORM,      ///< <tt>UNIFORM</tt> uniform full or short load
                   TRAPEZOIDAL   ///< <tt>TRAPEZOIDAL</tt> trapezoidal load
                 };
  LoadTypes type = UNIFORM;
};



struct Moment : Component{
  float mag;
  bool directionCw; // clockwise direction
};

struct Profile{
    std::vector< std::vector<double> > dims; // length, x, y
    QPixmap image;
    QPixmap image_section;
    enum ProfileTypes { T,L,I };
    ProfileTypes type;
};

struct Material {
    QString name;
    Profile profile;
    double E = 120; // in GPa
    double PR = 0.3; // poisson's ratio
    double A;
    double Ixx = 1250;
    double Iyy;
};

Q_DECLARE_METATYPE(Force)
Q_DECLARE_METATYPE(Support)
Q_DECLARE_METATYPE(Moment)
Q_DECLARE_METATYPE(DistLoad)
Q_DECLARE_METATYPE(Profile)
Q_DECLARE_METATYPE(Material)
Q_DECLARE_METATYPE(QTreeWidgetItem*)

namespace Ui {
class beam;
}

class beam : public QMainWindow
{
    Q_OBJECT

public:
    explicit beam(QWidget *parent = 0);
    ~beam();

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
    void propertyValueChanged(QtProperty *property, const QVariant &value);
    void grsel();
    void on_actionExit_triggered();
    void testt(QGraphicsSceneHoverEvent *ev);
    void on_actionMaterial_Properties_triggered();
    void on_actionShowElasticCurve_toggled(bool checked);

    void on_actionShow_Reactions_toggled(bool checked);

    void on_actionShowMomentDiagram_toggled(bool checked);

    void on_actionShowShearDiagram_toggled(bool checked);

    void on_actionShowSlopeDiagram_toggled(bool checked);

    void scaleChanged(int sf);
private:
    Ui::beam *ui;
    QGraphicsScene *scene;
    QList<Force> forces;
    QList<Support> supports;
    QList<DistLoad > distributedloads;
    QList<Moment> moments;
    QList<Profile> profiles;
    QList< QPair<double,double> > resForces;
    QGraphicsItem *drawSupport(Support s);
    QGraphicsItem *drawSingleForce(Force f);
    QGraphicsItem *drawMoment(Moment m);
    void addSingleForce(Force force);
    void addSupport(Support support);
    void addMoment(Moment moment);
    void reDrawCPanel();
    void reDrawScene();
    void drawElasticCurve(arma::vec u, arma::vec nodes);
    void drawReactions(arma::vec p,arma::vec nodes);
    QGraphicsPathItem *elasticCurve = new QGraphicsPathItem;
    QGraphicsPathItem *slopeDiagram = new QGraphicsPathItem;
    QGraphicsPathItem *momentDiagram = new QGraphicsPathItem;
    QGraphicsPathItem *shearDiagram = new QGraphicsPathItem;
    QGraphicsItemGroup *reactions = new QGraphicsItemGroup;
    double realBeamL;
    double LFactor;
    QGraphicsItem *drawDistLoad(DistLoad dl);
    void addDistLoad(DistLoad dl);
    QtVariantPropertyManager *variantManager;
    QtProperty *topItem;
    QtTreePropertyBrowser *variantEditor;
    void printv(std::vector<std::vector<double> > A);

    QDialog *materialprops;
    Ui::materialproperties matui;

    QDialog *distloaddialog;
    Ui::distributedloaddialog distui;

    QButtonGroup *bg;
    Profile activeProfile;
    Material activeMaterial;
    Material currentMaterial;
    Material calculateSectionProperties(Material m);
    int currentMaterialTab = 0;
    bool first=true;
    void redrawProfile();
    //void distLoadEquivalent();
    Support findPreviousSupport(double posX);
    Support findNextSupport(double posX);
    int findSupportInterval(double posX);
    void redrawComponents();
    void drawMomentDiagram(arma::vec p, arma::vec nodes);
    void drawShearDiagram(arma::vec p, arma::vec nodes);
    void drawSlopeDiagram(arma::vec u, arma::vec nodes);
    int scaleFactor=1;
    arma::vec disp_nodes;
    arma::colvec disp_P;
    arma::colvec disp_u;
    QList<Material> materials;
};

#endif // BEAM_H
