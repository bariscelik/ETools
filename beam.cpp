#include "beam.h"
#include "ui_beam.h"

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
    drawSupports(0,0);
    drawSupports(600,0);
}

beam::~beam()
{
    delete ui;
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

}

void beam::drawSingleForce(int posX,/*QPoint pos,*/ QString mag, int angle)
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
    group->setPos(posX,-15);
    group->setRotation(-angle);
    group->setToolTip(QString(mag+"N"));


}

void beam::on_pushButton_2_clicked()
{
    ui->graphicsView->rotate(50);
}

void beam::on_addForceBtn_clicked()
{
    drawSingleForce(ui->forceXInput->text().toInt(),ui->forceMagInput->text(),ui->forceAngleInput->text().toInt());
}
