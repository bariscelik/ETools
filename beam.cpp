#include "beam.h"
#include "ui_beam.h"
#include <QSvgGenerator>
#include <muParser.h>

class PointValidator : public QDoubleValidator
{
public:

    /**
     * @brief Point validator with '.' decimalpoint and bottom to top
     *
     * @param bottom - lower value
     * @param top - upper value
     * @param decimals - number of digits after point
     * @param parent - parent object
     */
    PointValidator(double bottom, double top, int decimals, QObject * parent) :
        QDoubleValidator(bottom, top, decimals, parent)
    {}

    QValidator::State validate(QString &s, int &i) const
    {
        if (s.isEmpty() || s == "-") {
            return QValidator::Intermediate;
        }

        QChar decimalPoint = '.';

        if(s.indexOf(decimalPoint) != -1) {
            int charsAfterPoint = s.length() - s.indexOf(decimalPoint) - 1;

            if (charsAfterPoint > decimals()) {
                return QValidator::Invalid;
            }
        }

        bool ok;
        double d = s.toDouble(&ok);

        if (ok && d >= bottom() && d <= top()) {
            qDebug() << d;
            return QValidator::Acceptable;
        } else {
            return QValidator::Invalid;
        }
    }
};

template <typename T>
double mathSolve(std::string exp,T xval)
{
    T y;

      try
      {
        T x;

        x = xval;

        mu::Parser p;

        p.DefineVar("x", &x);
        p.SetExpr(exp);

        y = p.Eval();

      }
      catch (mu::Parser::exception_type &e)
      {
        std::cout << e.GetMsg() << std::endl;
      }

   return y;
}

// rectangles  method
double calcIntegral(double a,double b,std::string f,int n)
{
    double sum=0, x, h;

    h = (b - a) / n;
    x = a;
    for(int i=0; i<n; ++i)
    {
        x = a + i * h;
        sum += h * mathSolve<double>(f, x + 0.5 * h);
    }

    return sum;
}

// find next support
int beam::findSupportInterval(double posX)
{
    int scount = supports.size();

    for(unsigned int i=0;i<scount;i++)
    {
        if(posX<supports[i].posX)
        {
            return i;
        }
    }

    return 0;
}

Material beam::calculateSectionProperties(Material m)
{

    // TODO: T & L profile calculation
    switch(m.profile.type)
    {
        case Profile::T:
            m.Ixx = m.profile.dims[0][0];
            m.Iyy = 123;
            m.A = m.profile.dims[2][0] * (m.profile.dims[0][0] + m.profile.dims[1][0] - m.profile.dims[2][0] );
        break;
        case Profile::I:
            m.Ixx = std::pow(m.profile.dims[2][0],3)*(m.profile.dims[0][0] + 2*(m.profile.dims[1][0]-m.profile.dims[2][0]))/3; // t^3 (2b + h_iç ) / 3 => t^3 (2b + l -2t ) / 3 => t^3 (l+2(b-t)) / 3
            m.Iyy = 123;
            m.A = m.profile.dims[2][0]*(m.profile.dims[0][0] + 2*(m.profile.dims[1][0]-m.profile.dims[2][0])); // t(l-2t+2b) => t(l+2(b-t))
            printv(m.profile.dims);
        break;
        case Profile::L:

        break;
    default:

        break;
    }

    return m;
}

void beam::grsel()
{
    if(scene->selectedItems().count() > 0)
    {
        if(!scene->selectedItems().at(0)->data(0).isNull())
        {
            ui->treeWidget->clearSelection();
            scene->selectedItems().at(0)->data(0).value<QTreeWidgetItem*>()->setSelected(true);

            ui->treeWidget->setFocus();
        }
    }else{
        ui->treeWidget->clearSelection();
    }
}

beam::beam(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::beam)
{
    ui->setupUi(this);

    scene = new QGraphicsScene(this);
    ui->graphicsView->setScene(scene);

    connect(scene, SIGNAL(selectionChanged()), this, SLOT(grsel()));

    variantManager = new QtVariantPropertyManager();
    connect(variantManager, SIGNAL(valueChanged(QtProperty *, const QVariant &)),
                this, SLOT(propertyValueChanged(QtProperty *, const QVariant &)));

    QtVariantEditorFactory *variantFactory = new QtVariantEditorFactory();
    variantEditor = ui->treeView_2;
    variantEditor->setFactoryForManager(variantManager, variantFactory);
    variantEditor->setPropertiesWithoutValueMarked(true);
    variantEditor->setRootIsDecorated(false);
    variantEditor->setAlternatingRowColors(true);

    // beam length
    realBeamL = 10;

    // length display factor
    LFactor = beamL / realBeamL;


    QPen blackPen(Qt::black);
    blackPen.setWidth(1);



    QGraphicsRectItem *bar = scene->addRect(0,-15,beamL,15,blackPen,QBrush(QColor("#fff")));
    bar->setFlag(QGraphicsItem::ItemIsSelectable);


    Support s;

    //s.fx = 0;
    s.posX = 0;
    s.type = Support::FIXED;
    addSupport(s);
    s.type = Support::ROLLER;

    s.posX = realBeamL;
    addSupport(s);
    reDrawCPanel();

    ui->treeWidget->setContextMenuPolicy(Qt::CustomContextMenu);

    ui->widget->hide();
    QShortcut *delAct = new QShortcut(QKeySequence::Delete,ui->treeWidget);
    connect(delAct, SIGNAL(activated()), this, SLOT(componentsActionDelete()));








    Profile p;
    p.dims = { {10,-10,120}, // l
               {5,154,285}, // b
               {6,325,34} }; // t
    p.image = QPixmap(":/files/images/profiles/profile_full_i.png");
    p.image_section = QPixmap(":/files/images/profiles/profile_i.png");
    p.type = Profile::I;
    profiles.append(p);

    p.dims = { {10,172,-15}, // b
               {50,-20,160}, // l
               {40,365,50} }; // t
    p.image = QPixmap(":/files/images/profiles/profile_full_t.png");

    p.image_section = QPixmap(":/files/images/profiles/profile_t.png");
    p.type = Profile::T;
    profiles.append(p);

    p.dims = { {10,6,166}, // l
               {5,70,10}}; // t
    p.image = QPixmap(":/files/images/profiles/profile_full_l.png");
    p.image_section = QPixmap(":/files/images/profiles/profile_l.png");
    p.type = Profile::L;
    profiles.append(p);

    materialprops = new QDialog(this);

    matui.setupUi(materialprops);

    activeMaterial.profile = profiles.at(0);

    matui.buttonBox->button(QDialogButtonBox::Ok)->setText(tr("Ok"));
    matui.buttonBox->button(QDialogButtonBox::Cancel)->setText(tr("Cancel"));


    Material m;
    m.name = tr("Carbon-fiber-reinforced polymer");
    m.E = 40;
    m.PR = 0.310;
    materials.append(m);
    m.name = tr("Titanium");
    m.E = 110.3;
    m.PR = 0.32;
    materials.append(m);
    m.name = tr("Aluminum");
    m.E = 69;
    m.PR = 0.335;
    materials.append(m);
    m.name = tr("Brass");
    m.E = 112.5;
    m.PR = 0.331;
    materials.append(m);
    m.name = tr("Steel (ASTM-A36)");
    m.E = 200;
    m.PR = 0.303;
    materials.append(m);
    m.name = tr("Graphen");
    m.E = 1050;
    m.PR = 0.17;
    materials.append(m);

    int mCount = materials.count();

    for(int i=0;i < mCount; i++)
    {
        matui.materialsComboBox->addItem(materials[i].name);
    }

    matui.materialsComboBox->addItem(tr("Custom"));
    matui.materialsComboBox->setCurrentIndex(mCount);

    bg = new QButtonGroup(materialprops);
    unsigned int psize = profiles.size();
    for(unsigned int i=0;i<psize;i++)
    {
        QPushButton *pb = new QPushButton(materialprops);
        pb->setIcon(QIcon(profiles.at(i).image));
        pb->setIconSize(QSize(110,60));
        pb->setCheckable(true);
        pb->setProperty("p", i);

        matui.horizontalLayout->addWidget(pb);
        bg->addButton(pb);
    }


    bg->setExclusive(true);

    QList<QLineEdit*> *lineEditGroup = new QList<QLineEdit*>();

    QGraphicsScene *sc = new QGraphicsScene(materialprops);

    connect(bg, static_cast<void(QButtonGroup::*)(QAbstractButton *, bool)>(&QButtonGroup::buttonToggled),
        [=](QAbstractButton *button, bool checked){

        if(checked){
            matui.elasticity->setText(QString::number(currentMaterial.E));

            int i = button->property("p").value<int>();

            Profile p = profiles.at(i);
            currentMaterial.profile = p;

            if(first)
            {
                currentMaterial = activeMaterial;
                first = false;
            }


            matui.graphicsView->setScene(sc);
            sc->clear();
            qDebug() << sc->addPixmap(currentMaterial.profile.image_section)->boundingRect();

            lineEditGroup->clear();

            unsigned int dsize = currentMaterial.profile.dims.size();
            for(unsigned int k=0;k<dsize;k++)
            {
                QLineEdit *edit = new QLineEdit(QString::number(currentMaterial.profile.dims[k][0]));
                edit->setValidator(new PointValidator(0,999999999,4,this));
                edit->setFixedWidth(30);
                QGraphicsWidget *ed = sc->addWidget(edit);
                ed->setPos(currentMaterial.profile.dims[k][1],currentMaterial.profile.dims[k][2]);
                lineEditGroup->append(edit);
                connect(edit,&QLineEdit::textChanged,[=](QString s){

                    unsigned int dimssize=currentMaterial.profile.dims.size();

                    for(unsigned int i=0;i<dimssize;i++)
                        currentMaterial.profile.dims[i][0] = lineEditGroup->at(i)->text().toDouble();


                    currentMaterial = calculateSectionProperties(currentMaterial);

                    matui.resultsLabel->setText(QString("<strong>Area</strong> = %1 m<sup>2</sup><br>"
                                                        "<strong>I<sub>xx</sub></strong> = %2 m<sup>4</sup><br>"
                                                        "<strong>I<sub>yy</sub></strong> = %3 m<sup>4</sup>")
                                                            .arg(QString::number(currentMaterial.A))
                                                            .arg(QString::number(currentMaterial.Ixx))
                                                            .arg(QString::number(currentMaterial.Iyy)));
                });
            }
            emit lineEditGroup->at(0)->textChanged(QString());
        }


    });

    connect(matui.materialsComboBox,static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged),[=](int index){

        matui.elasticity->setDisabled(true);
        matui.poissonRatio->setDisabled(true);

        if(index == mCount)
        {
            matui.elasticity->setDisabled(false);
            matui.poissonRatio->setDisabled(false);
        }else{
            matui.elasticity->setText(QString::number(materials[index].E));
            matui.poissonRatio->setText(QString::number(materials[index].PR));
        }


    });

    connect(matui.elasticity,&QLineEdit::textChanged,[=](QString s){
        currentMaterial.E = s.toDouble();
    });

    connect(matui.poissonRatio,&QLineEdit::textChanged,[=](QString s){
        currentMaterial.PR = s.toDouble();
    });

   connect(matui.buttonBox,&QDialogButtonBox::accepted,
   [=](){
        activeMaterial = currentMaterial;
        materialprops->accept();
        printv(activeMaterial.profile.dims);
        currentMaterialTab = bg->checkedButton()->property("p").toInt();
        redrawProfile();
   });

   connect(matui.buttonBox,&QDialogButtonBox::rejected,
   [=](){
          if (QMessageBox::question( this, "ETools",
                                     tr("All data will lost. Are you sure?\n"),
                                     QMessageBox::No |
                                     QMessageBox::Yes, QMessageBox::Yes) == QMessageBox::Yes)
          {
              materialprops->reject();
          }
   });
   redrawProfile();

   QSlider *slider = new QSlider(Qt::Horizontal, this);
   slider->setFixedWidth(200);
   slider->setTickInterval(10);
   slider->setMaximum(50000);
   slider->setMinimum(0);

   connect(slider, SIGNAL(valueChanged(int)), this, SLOT(scaleChanged(int)));
   ui->toolBar->addWidget(slider);
}

beam::~beam()
{
    delete ui;
}

void beam::scaleChanged(int sf)
{
    scaleFactor = sf;
    drawElasticCurve(disp_u,disp_nodes);
    drawSlopeDiagram(disp_u,disp_nodes);
}

void beam::redrawComponents()
{
    int supcount = supports.size();
    int fcount = forces.size();
    int dlcount = distributedloads.size();

    for(int i=0;i<supcount;i++){ delete supports[i].item;supports[i].item = drawSupport(supports[i]); }
    for(int i=0;i<fcount;i++){ delete forces[i].item;forces[i].item = drawSingleForce(forces[i]); }
    for(int i=0;i<dlcount;i++){ delete distributedloads[i].item;distributedloads[i].item = drawDistLoad(distributedloads[i]); }

}

void beam::redrawProfile()
{
    QGraphicsScene *profileScene = new QGraphicsScene(this);
    profileScene->clear();
    ui->profileView->setParent(ui->graphicsView);
    ui->profileView->setScene(profileScene);

    QGraphicsPixmapItem *pix = profileScene->addPixmap(activeMaterial.profile.image_section);
    unsigned int dsize = activeMaterial.profile.dims.size();
    for(unsigned int i=0;i<dsize;i++)
    {
        profileScene->addText(QString::number(activeMaterial.profile.dims[i][0]))->setPos(activeMaterial.profile.dims[i][1],activeMaterial.profile.dims[i][2]);
    }


    QRectF bounds = profileScene->itemsBoundingRect();
    qDebug() << bounds;

    //ui->profileView->fitInView(pix);

}

void beam::reDrawCPanel()
{
    ui->treeWidget->clear();


    // ----- FORCES -----
    QTreeWidgetItem *allForces = new QTreeWidgetItem();
    allForces->setText(0,tr("Forces"));
    allForces->setIcon(0,QIcon(":/files/images/force.svg"));
    allForces->setData(0,Qt::UserRole,QVariant(1));
    ui->treeWidget->addTopLevelItem(allForces);
    int itemCount = forces.count();

    for(int i=0; i<itemCount; ++i)
    {
        QTreeWidgetItem *item = new QTreeWidgetItem();
        item->setText(0,QString::number(forces.at(i).mag) + " N");
        item->setData(0,Qt::UserRole,QVariant(i));
        allForces->addChild(item);
        const QVariant var1 = QVariant::fromValue<QTreeWidgetItem*>(item);
        forces.at(i).item->setData(0,var1);
    }
    allForces->setExpanded(true);

    // ----- FORCES -----

    // ----- SUPPORTS -----
    QTreeWidgetItem *allSupports = new QTreeWidgetItem();
    allSupports->setText(0,tr("Supports"));
    allSupports->setIcon(0,QIcon(":/files/images/support.svg"));
    allSupports->setData(0,Qt::UserRole,QVariant(2));
    ui->treeWidget->addTopLevelItem(allSupports);
    itemCount = supports.count();
    for(int i=0; i<itemCount; ++i)
    {
        QTreeWidgetItem *item = new QTreeWidgetItem();
        item->setText(0,tr("Support ") + QString::number(i));
        item->setData(0,Qt::UserRole,QVariant(i));
        allSupports->addChild(item);
        const QVariant var1 = QVariant::fromValue<QTreeWidgetItem*>(item);
        supports.at(i).item->setData(0,var1);
    }
    allSupports->setExpanded(true);
    // ----- SUPPORTS -----

    // ----- FORCES -----
    QTreeWidgetItem *allDistLoads = new QTreeWidgetItem();
    allDistLoads->setText(0,tr("Distributed Loads"));
    allDistLoads->setIcon(0,QIcon(":/files/images/distload.png"));
    allDistLoads->setData(0,Qt::UserRole,QVariant(3));
    ui->treeWidget->addTopLevelItem(allDistLoads);
    itemCount = distributedloads.count();

    for(int i=0; i<itemCount; ++i)
    {
        QTreeWidgetItem *item = new QTreeWidgetItem();
        item->setText(0,QString::number(distributedloads.at(i).x1) + " - " + QString::number(distributedloads.at(i).x2));
        item->setData(0,Qt::UserRole,QVariant(i));
        allDistLoads->addChild(item);
        const QVariant var1 = QVariant::fromValue<QTreeWidgetItem*>(item);
        distributedloads.at(i).item->setData(0,var1);
    }
    allDistLoads->setExpanded(true);

    // ----- FORCES -----
}

/*!
 * \brief draws a support by given data
 * \param s - Support
 * \return QGraphicsItem
 */
QGraphicsItem *beam::drawSupport(Support s)
{
    int ypos=0;
    float xpos = LFactor * s.posX;
    QPen blackPen(Qt::black);
    blackPen.setWidth(2);

QList<QGraphicsItem*> supItms;


    QPolygonF Triangle;
    Triangle.append(QPointF(xpos-20.,ypos+40));
    Triangle.append(QPointF(xpos+0.,ypos+0));
    Triangle.append(QPointF(xpos+20.,ypos+40));

    if(s.type == Support::FIXED)
    {
        blackPen.setWidth(4);
        QGraphicsLineItem *line = scene->addLine(xpos,40,xpos,-55,blackPen);

        blackPen.setBrush(QBrush(Qt::BDiagPattern));
        blackPen.setWidth(16);
        blackPen.setCapStyle(Qt::FlatCap);
        QGraphicsLineItem *brush = scene->addLine(xpos-10,40,xpos-10,-55,blackPen);
        supItms.append(line);
        supItms.append(brush);
    }else if(s.type == Support::PINNED)
    {
        QGraphicsPolygonItem *tri = scene->addPolygon(Triangle,blackPen,QBrush(Qt::BDiagPattern));
        QGraphicsRectItem *rect;
        rect = scene->addRect(xpos-40,ypos+40,80,10,QPen(),QBrush(Qt::Dense3Pattern));
        supItms.append(rect);
        supItms.append(tri);
    }else if(s.type == Support::ROLLER)
    {
        QGraphicsPolygonItem *tri = scene->addPolygon(Triangle,blackPen,QBrush(Qt::BDiagPattern));
        QGraphicsRectItem *rect;
        rect = scene->addRect(xpos-40,ypos+50,80,10,QPen(),QBrush(Qt::Dense3Pattern));
        supItms.append(scene->addEllipse(xpos-15,ypos+40,10,10));
        supItms.append(scene->addEllipse(xpos+5,ypos+40,10,10));
        supItms.append(rect);
        supItms.append(tri);
    }



    QGraphicsItemGroup *group = scene->createItemGroup(supItms);
    group->setFlag(QGraphicsItem::ItemIsSelectable);

    reDrawCPanel();
    return group;
}



void beam::addSingleForce(Force force)
{
    force.item = drawSingleForce(force);

    forces.append(force);

    qDebug() << "[EKLE] Yeni kuvvet eklendi\nBüyüklük:" << force.mag
             << "\nKonum:" << force.posX
             << "\nAçı:" << force.angle;

    if(force.directionUp==true)
    {
        qDebug() << "Yön: Yukarı";
    }else{
        qDebug() << "Yön: Aşağı";
    }

}

void beam::addDistLoad(DistLoad dl)
{
    dl.item = drawDistLoad(dl);
    distributedloads.append(dl);
}

bool supportLessThan(const Support &s1,const Support &s2)
{
    return s1.posX < s2.posX;
}

void beam::addSupport(Support support)
{
    support.item = drawSupport(support);
    supports.append(support);
    qSort(supports.begin(), supports.end(), supportLessThan);
    supports;
}

void beam::addMoment(Moment moment)
{
    moment.item = drawMoment(moment);
    moments.append(moment);
}

QGraphicsItem *beam::drawMoment(Moment m)
{
    QPen blackPen(Qt::black);
    blackPen.setWidth(3);

    int cw = 1;
    if(m.directionCw) {cw = -1;}
    QPainterPath path;
    path.moveTo(0,0);
    path.arcTo(-20,-50,50,50,-90,cw*180);

    QPolygonF Triangle;
    Triangle.append(QPointF(path.currentPosition().x(),path.currentPosition().y()-7));
    Triangle.append(QPointF(path.currentPosition().x() - cw * 15,path.currentPosition().y()));
    Triangle.append(QPointF(path.currentPosition().x(),path.currentPosition().y()+7));

    QGraphicsPolygonItem *end = scene->addPolygon(Triangle,QPen(),QBrush(QColor("#000")));
    QGraphicsPathItem *start = scene->addPath(path,blackPen,QBrush(QColor("transparent")));
    QList<QGraphicsItem*> itms;
    itms.append(start);
    itms.append(end);

    QGraphicsItemGroup *moment = scene->createItemGroup(itms);

    moment->setFlag(QGraphicsItem::ItemIsSelectable);
    moment->setPos(820,20);

    /*QGraphicsPolygonItem *end = scene->addPolygon(Triangle,QPen(),QBrush(QColor("#000")));

    QList<QGraphicsItem*> arrowItms;
    arrowItms.append(start);
    arrowItms.append(end);


    QGraphicsItemGroup *group = scene->createItemGroup(arrowItms);
    group->setFlag(QGraphicsItem::ItemIsSelectable);
    group->setPos(force.posX,-15);
    group->setRotation(-force.angle);
    group->setToolTip(QString::number(force.mag)+"N");
*/
    return moment;
}

QGraphicsItem *beam::drawSingleForce(Force force)
{
    float xpos = LFactor * force.posX;

    QPen blackPen(Qt::black);
    blackPen.setWidth(3);

    QPolygonF Triangle;
    Triangle.append(QPointF(20,-5));
    Triangle.append(QPointF(0,0));
    Triangle.append(QPointF(20,+5));
    QLineF line(20,0,50,0);


    if(force.directionUp)
    {
        Triangle.clear();
        Triangle.append(QPointF(30,-5));
        Triangle.append(QPointF(50,0));
        Triangle.append(QPointF(30,+5));
        line.setLine(0,0,30,0);
    }

    QGraphicsLineItem *start = scene->addLine(line,blackPen);
    QGraphicsPolygonItem *end = scene->addPolygon(Triangle,QPen(),QBrush(QColor("#000")));

    QList<QGraphicsItem*> arrowItms;
    arrowItms.append(start);
    arrowItms.append(end);


    QGraphicsItemGroup *group = scene->createItemGroup(arrowItms);
    group->setFlag(QGraphicsItem::ItemIsSelectable);
    group->setPos(xpos,-15);
    group->setRotation(-force.angle);
    group->setToolTip(QString::number(force.mag)+"N");

    return group;
    /*QGraphicsTextItem *title= scene->addText("dad");
    title->setFlags(QGraphicsItem::ItemIsSelectable | QGraphicsItem::ItemIsFocusable | QGraphicsItem::ItemIsMovable);
    title->setTextInteractionFlags(Qt::TextEditorInteraction);
    title->setPos(100, 200);
*/
}

QGraphicsItem *beam::drawDistLoad(DistLoad dl)
{
    QPen blackPen(Qt::black);
    blackPen.setWidth(2);

    double HFactor = LFactor * 0.1;
    double x1pos = LFactor * dl.x1;

    double x2pos = LFactor * dl.x2;

    double arrowxPercent = 0.1;

    QList<QGraphicsItem*> allItems;

    QPainterPath path(QPointF(x1pos,-15-HFactor * dl.data[0]));

    if(dl.type == DistLoad::UNIFORM)
    {
        path.moveTo(x1pos,-50);
        path.lineTo(x2pos,-50);
        allItems.append(scene->addPath(path,blackPen));
    }else if(dl.type == DistLoad::TRAPEZOIDAL)
    {
        path.lineTo(x2pos,-15-HFactor * dl.data[1]);
        allItems.append(scene->addPath(path,blackPen));
    }


    // drawing arrows
    for(qreal i=0;i <= 1;i = i + arrowxPercent)
    {
        QPointF cp = path.pointAtPercent(i); // current point

        QPolygonF Triangle;
        Triangle.append(QPointF(cp.x()-3,-25));
        Triangle.append(QPointF(cp.x(),-15));
        Triangle.append(QPointF(cp.x()+3,-25));
        QLineF line(cp, QPointF(cp.x(),-25));

        allItems.append(scene->addLine(line,blackPen));
        allItems.append(scene->addPolygon(Triangle,QPen(),QBrush(QColor("#000"))));
    }

    QGraphicsTextItem *title= scene->addText( QString::number(dl.data[0]) + " N/m" );
    title->setPos(path.boundingRect().center().x(),path.boundingRect().center().y() - 30);

    allItems.append(title);

    QGraphicsItemGroup *group = scene->createItemGroup(allItems);
    group->setPos(0,0);
    group->setFlag(QGraphicsItem::ItemIsSelectable);
    return group;
}

bool sortResForces(const QPair<float,float>& e1, const QPair<float,float>& e2) {
    return e1.first < e2.first;
}

using namespace arma;

mat stfMatrix(double L,double E,double A,double I)
{
    mat stf(4,4,fill::zeros);


    stf << 12  << 6*L   << -12  << 6*L << endr
        << 6*L << 4*L*L << -6*L << 2*L*L << endr
        << -12 << -6*L  << 12   << -6*L << endr
        << 6*L << 2*L*L << -6*L << 4*L*L << endr;

    stf *= E*I*std::pow(L,-3);

    return stf;
}

void beam::on_solveBtn_clicked()
{

    resForces.clear();

    if(supports.size()<2){QMessageBox::warning(this,tr("Incompleted Data"),tr("There are no enough supports to solution.Please add a support and try again!")); return;}

    int divs = 1000;

    const int fcount = forces.size();
    const int dlcount = distributedloads.size();
    const int supCount = supports.size();

    vec nodes( supCount + fcount + dlcount*divs ); // nodes(xpos)

    mat boundconds(0,3);

    mat loads(0,3); // initial loads(nodenum, dof, force)

    unsigned int n = 0;

    for (int i = 0; i < supCount; i++){
        nodes(n) = supports[i].posX;
        n++;
    }

    for (int i = 0; i < fcount; i++){
        nodes(n) = forces[i].posX;
        n++;
    }

    for (int i = 0; i < dlcount; i++){

        double l = distributedloads[i].x2 - distributedloads[i].x1;
        double delta = l /divs;

        for(int j=0;j<divs;j++)
        {
            nodes(n) = distributedloads[i].x1 + delta*0.5 + j*delta;
            n++;
        }
    }


    nodes = sort(nodes);

    for (int i = 0; i < supCount; i++){

        uvec t = arma::find(nodes == supports[i].posX);

        boundconds.insert_rows(boundconds.n_rows,((rowvec){t(0),1,0}));

        if(supports[i].type == Support::FIXED)
        {
            boundconds.insert_rows(boundconds.n_rows,((rowvec){t(0),2,0}));
        }
    }

    for (int i = 0; i < fcount; i++){

        uvec t = arma::find(nodes == forces[i].posX);

        loads.insert_rows(loads.n_rows,((rowvec){t(0),1,forces[i].mag}));
        loads.insert_rows(loads.n_rows,((rowvec){t(0),2,0}));
    }

    for (int i = 0; i < dlcount; i++){

        double l = distributedloads[i].x2 - distributedloads[i].x1;
        double delta = l /divs;

        double f = distributedloads[i].data[0] * l / divs ;
        double tanTheta = (distributedloads[i].data[1]-distributedloads[i].data[0]) / l;

        for(int j=0;j<divs;j++)
        {
            uvec t = arma::find(nodes == (distributedloads[i].x1 + delta*0.5 + j*delta));

            if(distributedloads[i].type == DistLoad::TRAPEZOIDAL)
            {
                double f = tanTheta * (delta*0.5 + j*delta) + distributedloads[i].data[0];
            }

            loads.insert_rows(loads.n_rows,((rowvec){t(0),1,f}));
            loads.insert_rows(loads.n_rows,((rowvec){t(0),2,0}));
        }
    }

    // global stiffness matrix (zero)
    mat Kglobal(nodes.n_rows*2,nodes.n_rows*2,fill::zeros);

    // Force-Moment, Displacement-TwistAngle, specified dofs, unknown dofs vector
    colvec P(nodes.n_rows*2,fill::zeros);
    colvec u(nodes.n_rows*2,fill::zeros);
    ucolvec sdofs(boundconds.n_rows,fill::zeros);
    ucolvec udofs = linspace<ucolvec>(0,nodes.n_rows*2-1,nodes.n_rows*2);



    //nodes.print("Nodes:");
    //loads.print("Loads:");
    //boundconds.print("Boundary Conditions:");
    //E.print("Elasticity Modulus:");
    //A.print("Area:");
    //I.print("Second Moment of Mass");
    //Kglobal.print("Global Stiffness Matrix(initial):");
    //sdofs.print("sdofs(initial):");
    //udofs.print("udofs:");

    int init = 0;
    for(unsigned int i=0; i < nodes.n_rows-1; i++)
    {
        Kglobal(init,init,SizeMat(4,4)) += stfMatrix( ( nodes(i+1,0) - nodes(i,0)  ),activeMaterial.E,1,activeMaterial.Ixx);
        init += 2;
    }

    for(unsigned int i=0;i<boundconds.n_rows;i++)
    {
      int thisdof = 2*boundconds(i,0) + boundconds(i,1) - 1;
      sdofs(i) = thisdof;
      u(thisdof) = boundconds(i,2);
    }

    //sdofs.print("ben sdofs:");
    for(int i=sdofs.n_rows-1; i >=0; i--)
    {
        udofs.shed_row(sdofs.at(i));
    }

    for (unsigned int i=0;i<loads.n_rows;i++)
    {
      P(2*loads(i,0) + loads(i,1) - 1) = loads(i,2);
    }

    //P.print("P Vector:");

    //udofs.shed_rows(sdofs);
    //udofs.print("dvxbcnvmjh");

    //Kglobal.print("Kglobal");
    //Kglobal(udofs,udofs).print("udofff");


    u(udofs) = solve( Kglobal(udofs,udofs), (P(udofs)-Kglobal(udofs,sdofs)*u(sdofs) ), solve_opts::equilibrate);
    u.print("uuu:::");
    P(sdofs) = Kglobal.rows(sdofs)*u;

    P.print("PPPPP:");

    drawElasticCurve(u,nodes);
    drawSlopeDiagram(u,nodes);
    drawReactions(P,nodes);
    drawShearDiagram(P,nodes);

    disp_nodes = nodes;
    disp_u = u;
    disp_P =P;

    double umax=0;
    int umaxpos=0;

    for(int i = 0; i<u.n_rows;i=i+2)
    {

        if(qAbs(u(i))>qAbs(umax))
        {
            umax=u(i);
            umaxpos=i;
        }
    }

    double tmax=0;
    int tmaxpos=0;

    for(int i = 1; i<u.n_rows;i=i+2)
    {

        if(qAbs(u(i))>qAbs(tmax))
        {
            tmax=u(i);
            tmaxpos=i;
        }
    }



    ui->statusbar->showMessage("Maksimum Eğim (x,θ):" + QString::number(nodes((tmaxpos-1)/2)) + " m, " + QString::number(tmax) + " rad"
                               " | Maksimum Sehim (x,γ):" + QString::number(nodes(umaxpos/2)) + " m, " + QString::number(umax) + " m");

}



void beam::on_treeWidget_currentItemChanged(QTreeWidgetItem *current, QTreeWidgetItem *previous)
{

    scene->clearSelection();
    variantManager->clear();
    topItem = variantManager->addProperty(QtVariantPropertyManager::groupTypeId(), tr("Properties"));

    if(current){

        if(current->parent()){
            int type = current->parent()->data(0,Qt::UserRole).value<int>();
            int pos = current->data(0,Qt::UserRole).value<int>();
            switch(type)
            {
                case 1 :
                {
                    Force f = forces.at(pos);
                    f.item->setSelected(true);
                    break;
                }
                case 2 :
                {
                    Support s = supports.at(pos);
                    s.item->setSelected(true);
                    QtVariantProperty *item = variantManager->addProperty(QVariant::Bool, QLatin1String("Bool Property"));

/*
                    item->setValue(true);
                    topItem->addSubProperty(item);

*/


                    item = variantManager->addProperty(QVariant::String, tr("Position"));
                    item->setValue(s.posX);
                    topItem->addSubProperty(item);

                    item = variantManager->addProperty(QtVariantPropertyManager::enumTypeId(),
                                    tr("Type"));
                    QStringList enumNames;
                    enumNames << tr("FIXED") << tr("ROLLER") << tr("PINNED");
                    item->setAttribute(QLatin1String("enumNames"), enumNames);
                    item->setValue(2);
                    topItem->addSubProperty(item);

                    variantEditor->addProperty(topItem);
                    break;
                }
                case 3 :
                {
                    DistLoad dl = distributedloads.at(pos);
                    dl.item->setSelected(true);
                    break;
                }
            default:

            break;
            }

        }
    }



}

void beam::propertyValueChanged(QtProperty *property, const QVariant &value)
{
    /*if (property->propertyName() == "Position")
    {
        if(!value.isNull()){
        supports[ui->treeWidget->selectedItems().at(0)->data(0,Qt::UserRole).toInt()].posX = value.toDouble();
        redrawComponents();
        }
    }*/
        if (property->propertyName() == "Brush Color") {
                qgraphicsitem_cast<QAbstractGraphicsShapeItem*>(scene->selectedItems().at(0)->childItems().at(0))->setBrush(QBrush(value.value<QColor>()));
        }else if (property->propertyName() == "Pen Color") {
                qgraphicsitem_cast<QAbstractGraphicsShapeItem*>(scene->selectedItems().at(0)->childItems().at(0))->setPen(QPen(value.value<QColor>()));
        }
}

void beam::drawElasticCurve(arma::vec u, arma::vec nodes)
{
    scene->removeItem(elasticCurve);

    QPainterPath path(QPointF(0,0));

    for(int i = 0;i<u.n_rows;i+=2)
    {
        path.lineTo(nodes(i/2)*LFactor,u(i)*scaleFactor+2);
    }

    QPen ppen(Qt::DashLine);
    ppen.setColor(QColor("#2096BD"));
    ppen.setWidth(3);


    elasticCurve = scene->addPath(path,ppen,QBrush(QColor(143,217,242,80)));

}

void beam::drawSlopeDiagram(arma::vec u,arma::vec nodes)
{
    scene->removeItem(slopeDiagram);

    QPainterPath path(QPointF(0,0));

    for(int i = 1;i<u.n_rows;i+=2)
    {
        path.lineTo(nodes((i-1)/2)*LFactor,u(i)*scaleFactor-2);
    }

    QPen ppen(Qt::DashLine);
    ppen.setColor(QColor("#E0CB26"));
    ppen.setWidth(3);


    slopeDiagram = scene->addPath(path,ppen,QBrush(QColor(252,234,96,80)));

}

void beam::drawMomentDiagram(arma::vec p,arma::vec nodes)
{
    scene->removeItem(momentDiagram);

    QPainterPath path(QPointF(0,0));

    double moment = 0;

    for(int i = 1;i<p.n_rows;i+=2)
    {

        moment += p(i);
        if(i>1){
            moment += p(i-1);
        }

        path.lineTo(nodes((i-1)/2)*LFactor,moment*0.1);
    }

    QPen ppen(Qt::DashLine);
    ppen.setColor(QColor("#FA5700"));
    ppen.setWidth(3);

    momentDiagram = scene->addPath(path,ppen,QBrush(QColor(255,124,54,80)));
}

void beam::drawShearDiagram(arma::vec p,arma::vec nodes)
{
    scene->removeItem(shearDiagram);

    QPainterPath path(QPointF(0,0));

    double y=0;

    for(int i = 0;i<p.n_rows;i+=2)
    {
        y += p(i);
        path.lineTo(nodes(i/2)*LFactor,y);
        p(i) = y;
    }

    QPen ppen(Qt::DashLine);
    ppen.setColor(QColor("#26AD00"));
    ppen.setWidth(3);

    shearDiagram = scene->addPath(path,ppen,QBrush(QColor(67,204,29,80)));

    drawMomentDiagram(p,nodes);
}

void beam::drawReactions(arma::vec p,arma::vec nodes)
{
    QList<QGraphicsItem*> reactionList;

    foreach(QGraphicsItem *item,reactions->children())
    {
        scene->removeItem(item);
    }

    QPen bluePen(Qt::blue);
    bluePen.setWidth(3);

    int supCount = supports.size();

    for (int i = 0; i < supCount; i++){

        uvec t = arma::find(nodes == supports[i].posX);

        QGraphicsTextItem *txt = scene->addText(QString::number(p(2*t(0))).append(" N"));

        double supPos = supports[i].item->boundingRect().bottom();
        QPolygonF Triangle;
        Triangle.append(QPointF(supports[i].posX*LFactor,supPos));
        Triangle.append(QPointF(supports[i].posX*LFactor-10,supPos+15));
        Triangle.append(QPointF(supports[i].posX*LFactor+10,supPos+15));
        QGraphicsLineItem *line = scene->addLine(supports[i].posX*LFactor,supPos+15,supports[i].posX*LFactor,supPos+45,bluePen);
        QGraphicsPolygonItem *tri = scene->addPolygon(Triangle,bluePen,QBrush(QColor("blue")));
        txt->setDefaultTextColor(Qt::blue);
        txt->setPos(line->boundingRect().bottomRight());
        reactionList.append(line);
        reactionList.append(tri);
        reactionList.append(txt);
    }
    reactions = scene->createItemGroup(reactionList);
}


void beam::on_treeWidget_customContextMenuRequested(const QPoint &pos)
{
    QTreeWidget *tree = ui->treeWidget;

    QTreeWidgetItem *nd = tree->itemAt( pos );

    if(nd){

        if(nd->parent())
        {
            qDebug()<<pos<<nd->data(0,Qt::UserRole);

            QAction *delAct = new QAction(QIcon(":/files/images/delete.svg"), tr("&Delete"), this);
            delAct->setShortcut(QKeySequence::Delete);
            connect(delAct, SIGNAL(triggered()), this, SLOT(componentsActionDelete()));

            QMenu menu(this);
            menu.addAction(delAct);

            menu.exec( tree->mapToGlobal(pos) );
        }
    }
}

void beam::componentsActionDelete()
{

    if(ui->treeWidget->selectedItems().count() > 0){

        QTreeWidgetItem *selected = ui->treeWidget->selectedItems().at(0);

            if(selected->parent()){
                int type = selected->parent()->data(0,Qt::UserRole).value<int>();
                int pos = selected->data(0,Qt::UserRole).value<int>();
                switch(type)
                {
                    case 1 :
                    {
                        Force f = forces.at(pos);
                        scene->removeItem(f.item);
                        forces.removeAt(pos);
                        break;
                    }
                    case 2 :
                    {
                        Support s = supports.at(pos);
                        scene->removeItem(s.item);
                        supports.removeAt(pos);
                        break;
                    }
                    case 3 :
                    {
                        DistLoad dl = distributedloads.at(pos);
                        scene->removeItem(dl.item);
                        distributedloads.removeAt(pos);
                        break;
                    }
                default:

                break;
                }

            }

        reDrawCPanel();
    }
}

void beam::testt(QGraphicsSceneHoverEvent *ev)
{
    qDebug("test");
}

void beam::on_actionForce_triggered()
{
    QDialog dialog(this);
    dialog.setWindowTitle(tr("Add Force"));
    QFormLayout form(&dialog);

    QLineEdit *forceMagInput = new QLineEdit(&dialog);
    QLineEdit *forceXInput = new QLineEdit(&dialog);
    QLineEdit *forceAngleInput = new QLineEdit(&dialog);
    QCheckBox *directionUpChk = new QCheckBox(&dialog);


    forceMagInput->setValidator(new PointValidator(0,999999,4,this));
    forceAngleInput->setValidator(new PointValidator(0,180,4,this));
    forceXInput->setValidator(new PointValidator(0,realBeamL,4,this));

    form.addRow(tr("Magnitude (N):"),forceMagInput);
    form.addRow(tr("Position [x] (m):"),forceXInput);
    form.addRow(tr("Angle (°):"),forceAngleInput);
    form.addRow(tr("Up:"),directionUpChk);



    QDialogButtonBox buttonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel,
                               Qt::Horizontal, &dialog);
    buttonBox.button(QDialogButtonBox::Ok)->setText(tr("Ok"));
    buttonBox.button(QDialogButtonBox::Cancel)->setText(tr("Cancel"));
    form.addRow(&buttonBox);
    QObject::connect(&buttonBox, SIGNAL(accepted()), &dialog, SLOT(accept()));
    QObject::connect(&buttonBox, SIGNAL(rejected()), &dialog, SLOT(reject()));


    dialog.setFixedSize(form.sizeHint());

    if (dialog.exec() == QDialog::Accepted) {
            Force force;
            force.posX = forceXInput->text().toDouble();
            force.mag = forceMagInput->text().toDouble();
            force.angle = forceAngleInput->text().toFloat();
            force.directionUp = directionUpChk->isChecked();
            addSingleForce(force);
            reDrawCPanel();
    }
}


void beam::on_actionDistributed_Load_triggered()
{
    distloaddialog = new QDialog(this);
    distui.setupUi(distloaddialog);

    distloaddialog->setFixedSize(distloaddialog->size());

    distui.un_q->setValidator(new PointValidator(0,999999,4,this));

    distui.buttonBox->button(QDialogButtonBox::Ok)->setText(tr("Ok"));
    distui.buttonBox->button(QDialogButtonBox::Cancel)->setText(tr("Cancel"));

    if (distloaddialog->exec() == QDialog::Accepted) {

            DistLoad dl;

            if(distui.tabWidget->currentIndex()==0){
                dl.type = DistLoad::UNIFORM;
                dl.data[0] = distui.un_q->text().toDouble();
                dl.x1 = distui.un_x1->text().toDouble();
                dl.x2 = distui.un_x2->text().toDouble();
                //distui.un_q
            }
            else if(distui.tabWidget->currentIndex()==1){
                dl.type = DistLoad::TRAPEZOIDAL;
                dl.data[0] = distui.tr_q1->text().toDouble();
                dl.data[1] = distui.tr_q2->text().toDouble();
                dl.x1 = distui.tr_x1->text().toDouble();
                dl.x2 = distui.tr_x2->text().toDouble();
            }

            addDistLoad(dl);
            reDrawCPanel();
    }else{

    }

    delete distloaddialog;
}

void beam::on_actionMoment_triggered()
{
    QDialog dialog(this);
    dialog.setWindowTitle(tr("Add Moment"));
    QFormLayout form(&dialog);

    QLineEdit *mag = new QLineEdit(&dialog);
    QCheckBox *cw = new QCheckBox(&dialog);
    mag->setValidator(new PointValidator(0,999999999,4,this));

    form.addRow(tr("Magnitude (Nm):"),mag);
    form.addRow(tr("Clock-Wise Direction :"),cw);

    QDialogButtonBox buttonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel,
                               Qt::Horizontal, &dialog);
    buttonBox.button(QDialogButtonBox::Ok)->setText(tr("Ok"));
    buttonBox.button(QDialogButtonBox::Cancel)->setText(tr("Cancel"));
    form.addRow(&buttonBox);

    QObject::connect(&buttonBox, SIGNAL(accepted()), &dialog, SLOT(accept()));
    QObject::connect(&buttonBox, SIGNAL(rejected()), &dialog, SLOT(reject()));

    dialog.setFixedSize(form.sizeHint());

    if (dialog.exec() == QDialog::Accepted) {
            Moment m;
            m.mag = mag->text().toDouble();
            m.directionCw = cw->isChecked();
            addMoment(m);
            reDrawCPanel();
    }
}

void beam::on_action_Save_triggered()
{

}

void beam::on_actionSave_as_triggered()
{
    QFileDialog a(this);

    QString fileName = a.getSaveFileName(this,tr("Save as"),"~","Images (*.png *.xpm *.jpg);;Text files (*.txt);;XML files (*.xml)");

    if(QFileInfo(fileName).suffix() == "svg")
    {
        QSvgGenerator svgGen;

        svgGen.setFileName(fileName);
        svgGen.setSize(scene->sceneRect().size().toSize());

        QPainter painter( &svgGen );
        scene->render( &painter );
    }else{
        QPixmap pixMap = QPixmap::grabWidget(ui->graphicsView);
        pixMap.save(fileName);

    }
}

void beam::on_actionExit_triggered()
{
    this->hide();
}

void beam::printv(std::vector< std::vector<double> > A) {
    int m = A.size();
    int n = A.at(0).size();
    for (int i=0; i<m; i++) {
        for (int j=0; j<n; j++) {
            cout << A[i][j] << "\t";

        }
        cout << "\n";
    }
    cout << endl;
}

void beam::on_actionMaterial_Properties_triggered()
{
    materialprops->show();
    first = true;
    bg->buttons().at(currentMaterialTab)->setChecked(true);
}

void beam::on_actionShowElasticCurve_toggled(bool checked)
{
    if(checked)
    {
        elasticCurve->show();
    }else{
        elasticCurve->hide();
    }
}

void beam::on_actionShow_Reactions_toggled(bool checked)
{
    if(checked)
    {
        reactions->show();
    }else{
        reactions->hide();
    }

}

void beam::on_actionShowMomentDiagram_toggled(bool checked)
{
    if(checked)
    {
        momentDiagram->show();
    }else{
        momentDiagram->hide();
    }
}

void beam::on_actionShowShearDiagram_toggled(bool checked)
{
    if(checked)
    {
        shearDiagram->show();
    }else{
        shearDiagram->hide();
    }
}

void beam::on_actionShowSlopeDiagram_toggled(bool checked)
{
    if(checked)
    {
        slopeDiagram->show();
    }else{
        slopeDiagram->hide();
    }
}
