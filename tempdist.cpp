#include "tempdist.h"
#include "ui_tempdist.h"
#include <cmath>
#include <vector>
#include <iostream>
#include <qtypetraits.h>


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
            return QValidator::Acceptable;
        } else {
            return QValidator::Invalid;
        }
    }
};



void tempdist::on_graphicsview_selectChange()
{

    QList<QGraphicsItem *> selected= scene->selectedItems();

    if( selected.count() == 1){

        int lineId = selected.at(0)->data(1).toInt();
        if( lineId > -1 )
        {
            qDebug() << "data atanmış";
            f = new lineForm(lineId,lines.at(lineId).type,lines.at(lineId).h,lines.at(lineId).temp);
            connect(f, SIGNAL(accepted()), this, SLOT(test()));
            f->show();
        }
        /*selected.at(0)->setData(1,QVariant(0));*/  }


    /*lines.append(l);
    lines.count() - 1;
    */
}


void tempdist::test(){


    Line l;
    l.h = f->h;
    l.type = f->type;
    l.temp = f->temp;
    lines.replace(f->id,l);
    if(f->id==0) lLineLabel->setPlainText( QString::number(f->temp).append(" C°"));
    if(f->id==1) tLineLabel->setPlainText( QString::number(f->temp).append(" C°"));
    if(f->id==2) rLineLabel->setPlainText( QString::number(f->temp).append(" C°"));
    if(f->id==3) bLineLabel->setPlainText( QString::number(f->temp).append(" C°"));
    /*


    QPen arrowPen(Qt::black);
    arrowPen.setWidth(20);
    arrowPen.setColor(QColor("#5DCBF0"));

    QPolygonF Triangle;
    Triangle.append(QPointF(530,110));
    Triangle.append(QPointF(510,130));
    Triangle.append(QPointF(530,150));

    QGraphicsPolygonItem *arrowEnd = scene->addPolygon(Triangle,QPen(Qt::transparent),QBrush(QColor("#5DCBF0")));



    QGraphicsLineItem *arrowBar = scene->addLine(540,130,565,130,arrowPen);
    QList<QGraphicsItem*> supItms;
    supItms.append(arrowBar);
    supItms.append(arrowEnd);

    QGraphicsItemGroup *gr = scene->createItemGroup(supItms);
*/
    qDebug() << "type=" << f->type << "temp h =" << f->temp << f->h;

    delete f;

}


tempdist::tempdist(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::tempdist)
{
    ui->setupUi(this);
    /*ui->tLeft->setValidator(new PointValidator(0,999999,4,this));
    ui->tRight->setValidator(new PointValidator(0,999999,4,this));
    ui->tTop->setValidator(new PointValidator(0,999999,4,this));
    ui->tBottom->setValidator(new PointValidator(0,999999,4,this));
*/
    this->move(QApplication::desktop()->screen()->rect().center() - this->rect().center());

    scene = new QGraphicsScene(this);
    connect(scene, SIGNAL(selectionChanged()), this, SLOT(on_graphicsview_selectChange()));
    ui->graphicsView->setScene(scene);


    QPen blackPen(Qt::black);
    blackPen.setWidth(8);


    Line l;
    l.h = 0;
    l.type = 1;
    l.temp = 0;

    lines.append(l);
    lines.append(l);
    lines.append(l);
    lines.append(l);

    QGraphicsLineItem *lLine = scene->addLine(0,0,0,300,blackPen);
    lLine->setFlag(QGraphicsItem::ItemIsSelectable);
    lLine->setData(1,QVariant(0));

    QGraphicsLineItem *bLine = scene->addLine(0,0,500,0,blackPen);
    bLine->setFlag(QGraphicsItem::ItemIsSelectable);
    bLine->setData(1,QVariant(1));

    QGraphicsLineItem *rLine = scene->addLine(500,0,500,300,blackPen);
    rLine->setFlag(QGraphicsItem::ItemIsSelectable);
    rLine->setData(1,QVariant(2));

    QGraphicsLineItem *tLine = scene->addLine(0,300,500,300,blackPen);
    tLine->setFlag(QGraphicsItem::ItemIsSelectable);
    tLine->setData(1,QVariant(3));

    blackPen.setWidth(0);
    blackPen.setCosmetic(false);
    scene->addRect(0,0,500,300,blackPen,QBrush(QColor("#75BAD1")));

    QFont tempFont("Helvetica",12,QFont::Bold);
    lLineLabel= scene->addText("0 C°",tempFont);
    lLineLabel->setPos(-45,130);

    rLineLabel= scene->addText("0 C°",tempFont);
    rLineLabel->setPos(505,130);

    bLineLabel= scene->addText("0 C°",tempFont);
    bLineLabel->setPos(235,300);

    tLineLabel= scene->addText("0 C°",tempFont);
    tLineLabel->setPos(235,-30);

}

tempdist::~tempdist()
{
    delete ui;
}
using namespace std;
void print(vector< vector<double> > A) {

    int n = A.size();
    for (int i=0; i<n; i++) {
        for (int j=0; j<n+1; j++) {
            cout << A[i][j] << "\t";
            if (j == n-1) {
                cout << "| ";
            }
        }
        cout << "\n";
    }
    cout << endl;
}

void printv(vector< vector<double> > A) {

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

vector<double> gauss(vector< vector<double> > A) {
    int n = A.size();

    for (int i=0; i<n; i++) {

        double maxEl = abs(A[i][i]);
        int maxRow = i;
        for (int k=i+1; k<n; k++) {
            if (abs(A[k][i]) > maxEl) {
                maxEl = abs(A[k][i]);
                maxRow = k;
            }
        }

        for (int k=i; k<n+1;k++) {
            double tmp = A[maxRow][k];
            A[maxRow][k] = A[i][k];
            A[i][k] = tmp;
        }

        for (int k=i+1; k<n; k++) {
            double c = -A[k][i]/A[i][i];
            for (int j=i; j<n+1; j++) {
                if (i==j) {
                    A[k][j] = 0;
                } else {
                    A[k][j] += c * A[i][j];
                }
            }
        }
    }

    vector<double> x(n);
    for (int i=n-1; i>=0; i--) {
        x[i] = A[i][n]/A[i][i];
        for (int k=i-1;k>=0; k--) {
            A[k][n] -= A[k][i] * x[i];
        }
    }
    return x;
}

void tempdist::solve(double tLeft,double tRight,double tTop,double tBottom,unsigned int h,unsigned int l)
{

    using namespace std;

    unsigned int sizeH = h - 2, sizeL = l-2;

    unsigned int dof = sizeH * sizeL;

    vector< vector<double> > m(sizeH,vector<double>(sizeL,0));

    vector< vector<double> > resMatrix(dof,vector<double>(dof+1,0));



    int resRow=0;

    for (unsigned i = 0; i < sizeH; ++ i)
    {
        for (unsigned j = 0; j < sizeL; ++ j)
        {
            m[i][j] = sizeL * i + j;
        }
    }

    for (unsigned i = 0; i < sizeH; ++ i)
    {
        for (unsigned j = 0; j < sizeL; ++ j)
        {

            int resValue=0;
            bool tr=false,
                 tb=false,
                 tt=false,
                 tl=false;
            if(j == sizeL-1 ){resValue+=tRight;tr=true;}
            if(i == sizeH-1 ){resValue+=tBottom;tb=true;}
            if(i == 0 ){resValue+=tTop;tt=true;}
            if(j == 0 ){resValue+=tLeft;tl=true;}

            resMatrix[resRow][dof] = resValue;

            for(int k=0;k<dof;++k)
            {

                resMatrix[resRow][m[i][j]]=4;

                if(tr==false){resMatrix[resRow][m[i][j+1]]=-1;}
                if(tb==false){resMatrix[resRow][m[i+1][j]]=-1;}
                if(tt==false){resMatrix[resRow][m[i-1][j]]=-1;}
                if(tl==false){resMatrix[resRow][m[i][j-1]]=-1;}

            }

            ++resRow;
        }
    }

    vector<double> x(dof);
    x = gauss(resMatrix);

    QString s;
    s.append("Result:");
       for (unsigned int i=0; i<dof; i++) {
           s = s + "\n T_" + QString::number(i) + "= "+ QString::number(x[i]) + "C°";
       }
       ui->textEdit->setText(s);
}

void tempdist::solveConvection(double tLeft,double tRight,double tTop,double tBottom,double ht,double k,unsigned int h,unsigned int l)
{

    using namespace std;

    unsigned int sizeH = h - 2, sizeL = l-1;

    unsigned int dof = sizeH * sizeL;

    vector< vector<double> > m(sizeH,vector<double>(sizeL,0));

    vector< vector<double> > resMatrix(dof,vector<double>(dof+1,0));



    int resRow=0;

    for (unsigned i = 0; i < sizeH; ++ i)
    {
        for (unsigned j = 0; j < sizeL; ++ j)
        {
            m[i][j] = sizeL * i + j;
        }
    }

    for (unsigned i = 0; i < sizeH; ++ i)
    {
        for (unsigned j = 0; j < sizeL; ++ j)
        {
            int resValue=0;
            bool tr=false,
                 tb=false,
                 tt=false,
                 tl=false;
            if(j == sizeL-1 ){resValue+=2*ht*tRight/k;tr=true;}
            if(i == sizeH-1 ){resValue+=tBottom;tb=true;}
            if(i == 0 ){resValue+=tTop;tt=true;}
            if(j == 0 ){resValue+=tLeft;tl=true;}

            resMatrix[resRow][dof] = resValue;

            for(int k=0;k<dof;++k)
            {
                if(j==sizeL-1)
                {
                    resMatrix[resRow][m[i][j]]=4+2*ht/k;
                    resMatrix[resRow][m[i][j-1]]=2;
                }else{
                    resMatrix[resRow][m[i][j]]=4;
                    tl=true;
                }


                if(tr==false){resMatrix[resRow][m[i][j+1]]=-1;}
                if(tb==false){resMatrix[resRow][m[i+1][j]]=-1;}
                if(tt==false){resMatrix[resRow][m[i-1][j]]=-1;}
                if(tl==false){resMatrix[resRow][m[i][j-1]]=-1;}

            }

            ++resRow;
        }
    }

    vector<double> x(dof);
    x = gauss(resMatrix);

    QString s;
    s.append("Result:");
       for (unsigned int i=0; i<dof; i++) {
           s = s + "\n T_" + QString::number(i) + "= "+ QString::number(x[i]) + "C°";
       }
       ui->textEdit->setText(s);
}

void tempdist::on_actionSolve_triggered()
{
    solveConvection(lines.at(0).temp,
          lines.at(2).temp,
          lines.at(3).temp,
          lines.at(1).temp,
          lines.at(2).h,
          ui->lineEdit->text().toDouble(),
          ui->height->text().toInt(),
          ui->width->text().toInt());
}

