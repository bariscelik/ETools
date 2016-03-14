#include "beam.h"
#include "ui_beam.h"
#include <QtMath>

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

beam::beam(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::beam)
{
    ui->setupUi(this);
    scene = new QGraphicsScene(this);
    ui->graphicsView->setScene(scene);
    QPen blackPen(Qt::black);
    blackPen.setWidth(2);

    realBeamL = 2;
    LFactor = beamL / realBeamL;

    scene->addRect(0,-15,beamL,15,blackPen,QBrush(QColor("#75BAD1")))->setFlag(QGraphicsItem::ItemIsSelectable);


    Support s;

    s.fx = 0;
    s.fy = 0;
    s.posX = 0;
    s.type = 2;
    addSupport(s);
    s.posX = realBeamL;
    addSupport(s);
    /*Moment m;
    m.mag = 500;
    m.directionCw = true;

    addMoment(m);*/
    reDrawCPanel();
    ui->forceMagInput->setValidator(new PointValidator(0,999999,4,this));
    ui->forceAngleInput->setValidator(new PointValidator(0,180,4,this));
    ui->forceXInput->setValidator(new PointValidator(0,realBeamL,4,this));

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
        QVariant data;
        data.setValue(forces.at(i));
        item->setText(0,QString::number(forces.at(i).mag) + " N");
        item->setData(0,Qt::UserRole,data);
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
        QVariant data;
        data.setValue(supports.at(i));
        item->setText(0,"Mesnet " + QString::number(i));
        item->setData(0,Qt::UserRole,data);
        allSupports->addChild(item);
    }
    allSupports->setExpanded(true);
    // ----- SUPPORTS -----
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
    blackPen.setWidth(2);
    QPainterPath path;
        path.arcMoveTo(640,-30,30,50,20);
        path.arcTo(640,-30,30,50,20,90);

    QGraphicsPathItem *start = scene->addPath(path,blackPen,QBrush(QColor("transparent")));
    start->setFlag(QGraphicsItem::ItemIsSelectable);
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
    return start;
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


    if(ui->directionUpChk->isChecked())
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

void beam::on_pushButton_2_clicked()
{
    ui->graphicsView->rotate(50);
}

void beam::on_addForceBtn_clicked()
{
    Force force;
    force.posX = ui->forceXInput->text().toDouble();
    force.mag = ui->forceMagInput->text().toDouble();
    force.angle = ui->forceAngleInput->text().toFloat();
    force.directionUp = ui->directionUpChk->isChecked();
    addSingleForce(force);

    reDrawCPanel();
}

bool sortResForces(const QPair<float,float>& e1, const QPair<float,float>& e2) {
    return e1.first < e2.first;
}

void beam::on_solveBtn_clicked()
{
    resForces.clear();
    const int fcount = forces.size();
    QVector<double> x(fcount+1),y(fcount+1);
    x[fcount] = 0;y[fcount] = 0;
    qDebug() << "-----KUVVETLER-----\n";
    for (int i = 0; i < fcount; ++i)
    {
        Force f = forces.at(i);
        float rad = qDegreesToRadians(float(f.angle));
        float fx,fy;

        fx = f.mag * qCos(rad);
        fy = f.mag * qSin(rad);

        if(f.directionUp==true)
        {
            fx = -fx;
            fy = -fy;
        }

        QPair<float,float> res;
        res.first = f.posX;
        res.second = fy;
        resForces.append(res);
    }


    /*float Ny = (fy * f.posX) / beamL;

            qDebug() << "Büyüklük:" << f.mag
                     << "\nKonum:" << f.posX
                     << "\nAçı:" << f.angle << rad << qCos(rad) << fx << fy << Ny;

            if(f.directionUp==true)
            {
                qDebug() << "Yön: Yukarı";
            }else{
                qDebug() << "Yön: Aşağı";
            }
*/

    qSort(resForces.begin(), resForces.end(), sortResForces); // :confused:



    qDebug() << "\n" << resForces.at(0).first << resForces.at(1).first << resForces.at(2).first
              << "\n" << resForces.at(0).second << resForces.at(1).second << resForces.at(2).second;


    //plotDiagrams(x,y);
    //ui->widget->show();
}



void beam::on_treeWidget_currentItemChanged(QTreeWidgetItem *current, QTreeWidgetItem *previous)
{
    scene->clearSelection();
    if(current){
        if(current->parent()){
            if(current->parent()->data(0,Qt::UserRole).value<int>() == 1){
                Force i = current->data(0,Qt::UserRole).value<Force>();
                i.item->setSelected(true);
            }else if(current->parent()->data(0,Qt::UserRole).value<int>() == 2){
                Support i = current->data(0,Qt::UserRole).value<Support>();
                i.item->setSelected(true);
            }


        }
    }
}

void beam::plotDiagrams(QVector<double> x,QVector<double> y)
{
    ui->widget->addGraph();
    ui->widget->graph(0)->setData(x, y);
    ui->widget->graph(0)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCircle, QPen(Qt::black), QBrush(Qt::white), 6));
    ui->widget->setInteraction(QCP::iRangeZoom, true);

    ui->widget->xAxis->setLabel("x(m)");
    ui->widget->yAxis->setLabel("V(N)");

    ui->widget->xAxis->setRange(-100, 100);
    ui->widget->yAxis->setRange(-100, 100 );
    ui->widget->graph(0)->setBrush(QBrush("#ccc"));
    ui->widget->replot();
}
