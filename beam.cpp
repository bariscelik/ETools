#include "beam.h"
#include "ui_beam.h"
#include <QtMath>
#include <QSvgGenerator>
#include <exprtk.hpp>

class PointValidator : public QDoubleValidator
{
public:
    PointValidator(double bottom, double top, int decimals, QObject * parent) :
        QDoubleValidator(bottom, top, decimals, parent)
    {
    }

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
   typedef exprtk::symbol_table<T> symbol_table_t;
   typedef exprtk::expression<T> expression_t;
   typedef exprtk::parser<T> parser_t;

   std::string expression_string = exp;

   T x;

   symbol_table_t symbol_table;
   symbol_table.add_variable("x",x);
   symbol_table.add_constants();

   expression_t expression;
   expression.register_symbol_table(symbol_table);

   parser_t parser;
   parser.compile(expression_string,expression);

   x = xval;

   T y = expression.value();

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

beam::beam(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::beam)
{
    ui->setupUi(this);

    scene = new QGraphicsScene(this);
    ui->graphicsView->setScene(scene);
    QPen blackPen(Qt::black);
    blackPen.setWidth(2);

    realBeamL = 2;
    LFactor = beamL / realBeamL;

    QGraphicsRectItem *bar = scene->addRect(0,-15,beamL,15,blackPen,QBrush(QColor("#75BAD1")));
    bar->setFlag(QGraphicsItem::ItemIsSelectable);
    bar->setAcceptHoverEvents(true);



    Support s;

    s.fx = 0;
    s.fy = 0;
    s.posX = 0;
    s.type = 2;
    addSupport(s);
    s.posX = realBeamL;
    addSupport(s);

    reDrawCPanel();

    ui->treeWidget->setContextMenuPolicy(Qt::CustomContextMenu);

    ui->widget->hide();

}

beam::~beam()
{
    delete ui;
}

void beam::reDrawCPanel()
{
    ui->treeWidget->clear();


    // ----- FORCES -----
    QTreeWidgetItem *allForces = new QTreeWidgetItem();
    allForces->setText(0,"Forces");
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
    }
    allForces->setExpanded(true);

    // ----- FORCES -----

    // ----- SUPPORTS -----
    QTreeWidgetItem *allSupports = new QTreeWidgetItem();
    allSupports->setText(0,"Supports");
    allSupports->setIcon(0,QIcon(":/files/images/support.svg"));
    allSupports->setData(0,Qt::UserRole,QVariant(2));
    ui->treeWidget->addTopLevelItem(allSupports);
    itemCount = supports.count();
    for(int i=0; i<itemCount; ++i)
    {
        QTreeWidgetItem *item = new QTreeWidgetItem();
        item->setText(0,"Mesnet " + QString::number(i));
        item->setData(0,Qt::UserRole,QVariant(i));
        allSupports->addChild(item);
    }
    allSupports->setExpanded(true);
    // ----- SUPPORTS -----

    // ----- FORCES -----
    QTreeWidgetItem *allDistLoads = new QTreeWidgetItem();
    allDistLoads->setText(0,"Distributed Loads");
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
    blackPen.setWidth(3);

    QPolygonF Triangle;
    Triangle.append(QPointF(xpos-20.,ypos+40));
    Triangle.append(QPointF(xpos+0.,ypos+0));
    Triangle.append(QPointF(xpos+20.,ypos+40));
    QGraphicsPolygonItem *tri = scene->addPolygon(Triangle,blackPen,QBrush(QColor("#5DCBF0")));
    QGraphicsRectItem *rect = scene->addRect(xpos-40,ypos+40,80,10,QPen(),QBrush(QColor("#F0825D")));
    QList<QGraphicsItem*> supItms;
    supItms.append(tri);
    supItms.append(rect);

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

void beam::addSupport(Support support)
{
    support.item = drawSupport(support);
    supports.append(support);

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
    int HFactor = LFactor * 0.5;
    QPen blackPen(Qt::black);
    blackPen.setWidth(3);
    float x1pos = HFactor * dl.x1;

    QPainterPath path;
    path.moveTo(x1pos, - 15 -HFactor * mathSolve<double>(dl.f.toStdString(),x1pos));

    QList<QGraphicsItem*> arrowItms;
    QList<QGraphicsItem*> allArrowItms;

    for(double x=dl.x1; x<=dl.x2;x+=0.2)
    {
        double y = mathSolve<double>(dl.f.toStdString(),x);
        double xpos = HFactor * x;
        path.lineTo(xpos, - 15 - HFactor * y);

        QPolygonF Triangle;
        Triangle.append(QPointF(20,-5));
        Triangle.append(QPointF(0,0));
        Triangle.append(QPointF(20,+5));
        QLineF line(20,0,HFactor * y - 1,0);


        arrowItms.append(scene->addLine(line,blackPen));
        arrowItms.append(scene->addPolygon(Triangle,QPen(),QBrush(QColor("#000"))));

        QGraphicsItemGroup *gr = scene->createItemGroup(arrowItms);
        gr->setRotation(-90);
        gr->setPos(xpos,-15);

        allArrowItms.append(gr);
        arrowItms.clear();
    }

    QGraphicsPathItem *curve = scene->addPath(path,blackPen);


    allArrowItms.append(curve);

    QGraphicsItemGroup *group = scene->createItemGroup(allArrowItms);
    group->setPos(0,0);
    group->setFlag(QGraphicsItem::ItemIsSelectable);
    return group;
    /*QGraphicsTextItem *title= scene->addText("dad");
    title->setFlags(QGraphicsItem::ItemIsSelectable | QGraphicsItem::ItemIsFocusable | QGraphicsItem::ItemIsMovable);
    title->setTextInteractionFlags(Qt::TextEditorInteraction);
    title->setPos(100, 200);
*/
}

bool sortResForces(const QPair<float,float>& e1, const QPair<float,float>& e2) {
    return e1.first < e2.first;
}

void beam::on_solveBtn_clicked()
{
    resForces.clear();

    const int fcount = forces.size();
    const int dlcount = distributedloads.size();

    float totalX=0,totalY=0,totalMoment=0;



    qDebug() << "-----KUVVETLER-----\n";

    for (int i = 0; i < dlcount; ++i)
    {
        DistLoad dl = distributedloads.at(i);

        double dlArea = calcIntegral(dl.x1,dl.x2,dl.f.toStdString(),200);
        totalY += -dlArea;

        double centerofGravityMoment = calcIntegral(dl.x1,dl.x2,dl.f.toStdString().append("*x"),200);
        totalMoment += -centerofGravityMoment;

        QPair<float,float> res;
        res.first = centerofGravityMoment / dlArea;
        res.second = -dlArea;
        resForces.append(res);
    }


    for (int i = 0; i < fcount; ++i)
    {
        Force f = forces.at(i);
        float rad = qDegreesToRadians(float(f.angle));
        float fx,fy;

        fx = f.mag * qCos(rad);
        fy = f.mag * qSin(rad);

        if(f.directionUp==false)
        {
            fx = -fx;
            fy = -fy;
        }

        totalX += fx;
        totalY += fy;

        totalMoment += fy * f.posX;

        QPair<float,float> res;
        res.first = f.posX;
        res.second = fy;
        resForces.append(res);
    }


    qDebug() << "Mesnet Tepkisi 1(Ny):" << totalMoment / realBeamL -totalY
             << "\nMesnet Tepkisi 2(Ny):" << -totalMoment / realBeamL;

    Support s1=supports.at(0),s2=supports.at(1);
    s1.fy = totalMoment / realBeamL -totalY;
    s2.fy = -totalMoment / realBeamL;
    supports.replace(0,s1);
    supports.replace(1,s2);

    QPair<float,float> res;
    res.first = supports.at(0).posX;
    res.second = supports.at(0).fy;
    resForces.append(res);

    res.first = supports.at(1).posX;
    res.second = supports.at(1).fy;
    resForces.append(res);

    qSort(resForces.begin(), resForces.end(), sortResForces);



    qDebug() << "\n" << resForces.at(0).first << resForces.at(1).first << resForces.at(2).first
              << "\n" << resForces.at(0).second << resForces.at(1).second << resForces.at(2).second;

    plotDiagrams(resForces);
    ui->widget->show();
}



void beam::on_treeWidget_currentItemChanged(QTreeWidgetItem *current, QTreeWidgetItem *previous)
{
    scene->clearSelection();

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

void beam::plotDiagrams(QList< QPair<float,float> > points)
{
    int pointCount = points.count();
    QVector<double> x(pointCount),y(pointCount);

    int totalY=0;
    for(int i=0;i<pointCount;++i)
    {

        x.append(points.at(i).first);
        y.append(totalY);
        totalY += points.at(i).second;
        qDebug() << x.last() << y.last();
        x.append(points.at(i).first);
        y.append(totalY);
        qDebug() << x.last() << y.last();

    }

    QCPCurve *newCurve = new QCPCurve(ui->widget->xAxis, ui->widget->yAxis);
    newCurve->setData(x, y);
    newCurve->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCircle, QPen(Qt::black), QBrush(Qt::white), 5));
    newCurve->setBrush(QBrush("#F2FF00"));
    ui->widget->addPlottable(newCurve);
    ui->widget->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectAxes |
                                QCP::iSelectLegend | QCP::iSelectPlottables);

    ui->widget->xAxis->setLabel("x(m)");
    ui->widget->yAxis->setLabel("V(N)");

    ui->widget->xAxis->setRange(-100, 100);
    ui->widget->yAxis->setRange(-100, 100 );
    ui->widget->axisRect()->setupFullAxesBox();
    ui->widget->rescaleAxes();
    ui->widget->replot();

    connect(ui->widget, SIGNAL(mouseMove(QMouseEvent*)),this,SLOT(showPointToolTip(QMouseEvent*)));

}

void beam::showPointToolTip(QMouseEvent *event)
{

    double x = ui->widget->xAxis->pixelToCoord(event->pos().x());
    double y = ui->widget->yAxis->pixelToCoord(event->pos().y());

    setToolTip(QString(tr("Position: (%1 , %2)")).arg(x).arg(y));

}
void beam::on_treeWidget_customContextMenuRequested(const QPoint &pos)
{
    QTreeWidget *tree = ui->treeWidget;

    QTreeWidgetItem *nd = tree->itemAt( pos );

    if(nd->parent())
        {
        qDebug()<<pos<<nd->data(0,Qt::UserRole);

        QAction *delAct = new QAction(QIcon(":/files/images/delete.svg"), tr("&Delete"), this);
        connect(delAct, SIGNAL(triggered()), this, SLOT(componentsActionDelete()));

        QMenu menu(this);
        menu.addAction(delAct);

        menu.exec( tree->mapToGlobal(pos) );
    }
}

void beam::componentsActionDelete()
{
    QTreeWidgetItem *selected = ui->treeWidget->selectedItems().at(0);
    int pos = selected->data(0,Qt::UserRole).value<int>();

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
    QDialog dialog(this);
    dialog.setWindowTitle(tr("Add Distributed Load"));
    QFormLayout form(&dialog);

    QLineEdit *x1 = new QLineEdit(&dialog);
    QLineEdit *x2 = new QLineEdit(&dialog);
    QLineEdit *f = new QLineEdit(&dialog);

    x1->setValidator(new PointValidator(0,realBeamL,4,this));
    x2->setValidator(new PointValidator(0,realBeamL,4,this));

    form.addRow(tr("Position [x1] (m):"),x1);
    form.addRow(tr("Position [x2] (m):"),x2);
    form.addRow(tr("Formula f(x) : "),f);

    QDialogButtonBox buttonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel,
                               Qt::Horizontal, &dialog);
    form.addRow(&buttonBox);
    QObject::connect(&buttonBox, SIGNAL(accepted()), &dialog, SLOT(accept()));
    QObject::connect(&buttonBox, SIGNAL(rejected()), &dialog, SLOT(reject()));

    dialog.setFixedSize(form.sizeHint());

    if (dialog.exec() == QDialog::Accepted) {
            DistLoad dl;
            dl.x1 = x1->text().toDouble();
            dl.x2 = x2->text().toDouble();
            dl.f = f->text();
            addDistLoad(dl);
            reDrawCPanel();
    }
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
        svgGen.setSize(ui->graphicsView->size());
        svgGen.setViewBox(scene->sceneRect());

        QPainter painter( &svgGen );
        scene->render( &painter );
    }else{
        QPixmap pixMap = QPixmap::grabWidget(ui->graphicsView);
        pixMap.save(fileName);

    }
}
