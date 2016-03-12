#include "beam.h"
#include "ui_beam.h"
#include <QtMath>


beam::beam(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::beam)
{
    ui->setupUi(this);
    scene = new QGraphicsScene(this);

    ui->graphicsView->setScene(scene);

    QPen blackPen(Qt::black);
    blackPen.setWidth(2);

    scene->addRect(0,-15,600,15,blackPen,QBrush(QColor("#75BAD1")))->setFlag(QGraphicsItem::ItemIsSelectable);
    //scene->selectedItems();
    Support s;

    s.fx = 500;
    s.fy = 400;
    s.posX = 200;
    s.type = 2;
    supports.append(s);
    s.fx = 100;
    supports.append(s);

    drawSupports(0,0);
    drawSupports(600,0);
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
    allForces->setText(0,"Kuvvetler");
    allForces->setIcon(0,QIcon(":/files/images/force.svg"));
    allForces->setData(0,Qt::UserRole,QVariant(1));
    ui->treeWidget->addTopLevelItem(allForces);
    int itemCount = forces.count();
    for(int i=0; i<itemCount; ++i)
    {
        QVariant data;
        data.setValue(forces.at(i));
        QTreeWidgetItem *item = new QTreeWidgetItem();
        item->setText(0,QString::number(forces.at(i).mag) + " N");
        item->setData(0,Qt::UserRole,data);
        allForces->addChild(item);
    }
    allForces->setExpanded(true);
    // ----- FORCES -----

    // ----- SUPPORTS -----
    QTreeWidgetItem *allSupports = new QTreeWidgetItem();
    allSupports->setText(0,"Mesnetler");
    allSupports->setIcon(0,QIcon(":/files/images/support.svg"));
    ui->treeWidget->addTopLevelItem(allSupports);
    itemCount = supports.count();
    for(int i=0; i<itemCount; ++i)
    {
        QTreeWidgetItem *item = new QTreeWidgetItem();
        item->setText(0,"Mesnet " + QString::number(i));
        allSupports->addChild(item);
    }
    allSupports->setExpanded(true);
    // ----- SUPPORTS -----
}

void beam::drawSupports(int xpos, int ypos)
{
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
}

void beam::addSingleForce(Force force)
{
    forces.append(force);

    drawSingleForce(force);

    qDebug() << "[EKLE] Yeni kuvvet eklendi\nBüyüklük:" << force.mag
             << "\nKonum:" << force.posX
             << "\nAçı:" << force.angle;

    if(force.directionUp==true)
    {
        qDebug() << "Yön: Yukarı";
    }else{
        qDebug() << "Yön: Aşağı";
}
    reDrawCPanel();
}

void beam::addSupport(Support support)
{

}

void beam::drawSingleForce(Force force)
{
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
    group->setPos(force.posX,-15);
    group->setRotation(-force.angle);
    group->setToolTip(QString::number(force.mag)+"N");

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
    force.posX = ui->forceXInput->text().toFloat();
    force.mag = ui->forceMagInput->text().toFloat();
    force.angle = ui->forceAngleInput->text().toFloat();
    force.directionUp = ui->directionUpChk->isChecked();

    addSingleForce(force);
}

void beam::on_solveBtn_clicked()
{
    const int fcount = forces.size();
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

        float Ny = (fy * f.posX) / 600;

                qDebug() << "Büyüklük:" << f.mag
                         << "\nKonum:" << f.posX
                         << "\nAçı:" << f.angle << rad << qCos(rad) << fx << fy << Ny;

                if(f.directionUp==true)
                {
                    qDebug() << "Yön: Yukarı";
                }else{
                    qDebug() << "Yön: Aşağı";
                }


    }
}

void beam::on_treeWidget_currentItemChanged(QTreeWidgetItem *current, QTreeWidgetItem *previous)
{
    scene->items().at(1)->setSelected(true);/*
    if(current->parent()->data(0,Qt::UserRole).value<int>() == 1){
        Force f = current->data(0,Qt::UserRole).value<Force>();

    }*/
}
