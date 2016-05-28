#include "tempdist.h"
#include "ui_tempdist.h"
#include <QTime>
#include <QtConcurrent>

void delay( int millisecondsToWait )
{
    QTime dieTime = QTime::currentTime().addMSecs( millisecondsToWait );
    while( QTime::currentTime() < dieTime )
    {
        QCoreApplication::processEvents( QEventLoop::AllEvents, 100 );
    }
}

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



void tempdist::grsel()
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
            scene->clearSelection();
        }
        /*selected.at(0)->setData(1,QVariant(0));*/  }


    /*lines.append(l);
    lines.count() - 1;
    */
}


void tempdist::test(){

    QPen arrowPen(Qt::black);
    arrowPen.setWidth(4);

    Line l;
    l.h = f->h;
    l.type = f->type;
    l.temp = f->temp;
    l.item = lines.at(f->id).item;

    l.item->setZValue(6);

    /*
    QGraphicsTextItem *txt = scene->addText("Zırro:)");
    txt->setPos(0,320);
    txt->setParentItem(l.item);*/

    if (l.type == 2)
    {


        QPixmap pix = QPixmap(":/files/images/force.svg");
        QBrush br = QBrush(Qt::black,pix);
        br.setStyle(Qt::TexturePattern);
        arrowPen.setBrush(br);
        arrowPen.setWidth(20);
        arrowPen.setCapStyle(Qt::FlatCap);
        l.item->setZValue(4);

/*
        arrowPen.setBrush(Qt::DiagCrossPattern);
        arrowPen.setWidth(40);
        arrowPen.setCapStyle(Qt::FlatCap);
        l.item->setZValue(4);*/

    }else if(l.type == 3)
    {
        arrowPen.setBrush(QBrush(Qt::BDiagPattern));
        arrowPen.setWidth(20);
        arrowPen.setCapStyle(Qt::FlatCap);
        l.item->setZValue(4);
    }

    l.item->setPen(arrowPen);



    lines.replace(f->id,l);

    if(f->id==0) lLineLabel->setPlainText( QString::number(f->temp).append(" C°") );
    if(f->id==1) tLineLabel->setPlainText( QString::number(f->temp).append(" C°") );
    if(f->id==2) rLineLabel->setPlainText( QString::number(f->temp).append(" C°") );
    if(f->id==3) bLineLabel->setPlainText( QString::number(f->temp).append(" C°") );

    qDebug() << "type=" << f->type << "temp h =" << f->temp << f->h;

    delete f;

}

void tempdist::xAxisRangeChanged( const QCPRange &newRange, const QCPRange &oldRange ){
    if( newRange.lower < 0 ){
        ui->tempGraph->xAxis->setRangeLower( 0 );
        ui->tempGraph->xAxis->setRangeUpper( oldRange.upper );
    }else{
        ui->tempGraph->xAxis->setRangeUpper( newRange.upper );
    }
}

void tempdist::yAxisRangeChanged( const QCPRange &newRange, const QCPRange &oldRange ){
    if( newRange.lower < 0 ){
        ui->tempGraph->yAxis->setRangeLower( 0 );
        ui->tempGraph->yAxis->setRangeUpper( oldRange.upper );
    }else{
        ui->tempGraph->yAxis->setRangeUpper( newRange.upper );
    }
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
    QList<int> sizes;
    sizes << ui->graphicsView->size().width() << ui->tempGraph->size().width();
    ui->splitter->setSizes(sizes);
    ui->textEdit->hide();
    this->move(QApplication::desktop()->screen()->rect().center() - this->rect().center());

    scene = new QGraphicsScene(this);
    connect(scene, SIGNAL(selectionChanged()), this, SLOT(grsel()));
    ui->graphicsView->setScene(scene);

    ui->tempGraph->hide();

    QPen blackPen(Qt::black);
    blackPen.setWidth(4);

    connect(ui->tempGraph->xAxis,SIGNAL(rangeChanged(QCPRange,QCPRange)), this, SLOT(xAxisRangeChanged(QCPRange,QCPRange)) );
    connect(ui->tempGraph->yAxis,SIGNAL(rangeChanged(QCPRange,QCPRange)), this, SLOT(yAxisRangeChanged(QCPRange,QCPRange)) );



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

    lLine->setZValue(6);
    bLine->setZValue(6);
    rLine->setZValue(6);
    tLine->setZValue(6);

    blackPen.setWidth(0);
    blackPen.setCosmetic(false);




    Line l;
    l.h = 0;
    l.type = 1;
    l.temp = 80;
    l.item = lLine;
    lines.append(l);

    l.h = 0;
    l.type = 1;
    l.temp = 50;
    l.item = bLine;
    lines.append(l);

    l.h = 0;
    l.type = 1;
    l.temp = 150;
    l.item = rLine;
    lines.append(l);

    l.h = 0;
    l.type = 1;
    l.temp = 80;
    l.item = tLine;
    lines.append(l);

    QRadialGradient gr(250,150,100,250,150);
    gr.setColorAt(0, QColor("#F2F2F2"));
    gr.setColorAt(1, QColor("#DBDBDB"));

    QBrush br(gr);
    br.setStyle(Qt::RadialGradientPattern);
    scene->addRect(-2,-2,504,304,QPen(Qt::NoPen),br)->setZValue(5);

    QFont tempFont("Helvetica",12,QFont::Bold);
    lLineLabel= scene->addText(QString::number(lines.at(0).temp)+" C°",tempFont);
    lLineLabel->setPos(-60,130);

    rLineLabel= scene->addText(QString::number(lines.at(2).temp)+" C°",tempFont);
    rLineLabel->setPos(505,130);

    bLineLabel= scene->addText(QString::number(lines.at(3).temp)+" C°",tempFont);
    bLineLabel->setPos(235,300);

    tLineLabel= scene->addText(QString::number(lines.at(1).temp)+" C°",tempFont);
    tLineLabel->setPos(235,-30);

    std::cout.precision(12);


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

QString matrixToString(vector< vector<double> > A) {
    QString ret;

    int m = A.size();
    int n = A.at(0).size();
    for (int i=0; i<m; i++) {
        for (int j=0; j<n; j++) {
            //cout << A[i][j] << "\t";
            ret.append(QString::number(A[i][j],'f',12)).append(" ");
        }
        ret.append("<br>");
    }

    return ret;
}

vector<float> gauss(vector< vector<float> > A) {
    int n = A.size();

    for (int i=0; i<n; i++) {

        float maxEl = abs(A[i][i]);
        int maxRow = i;
        for (int k=i+1; k<n; k++) {
            if (abs(A[k][i]) > maxEl) {
                maxEl = abs(A[k][i]);
                maxRow = k;
            }
        }

        for (int k=i; k<n+1;k++) {
            float tmp = A[maxRow][k];
            A[maxRow][k] = A[i][k];
            A[i][k] = tmp;
        }

        for (int k=i+1; k<n; k++) {
            float c = -A[k][i]/A[i][i];
            for (int j=i; j<n+1; j++) {
                if (i==j) {
                    A[k][j] = 0;
                } else {
                    A[k][j] += c * A[i][j];
                }
            }
        }
    }

    vector<float> x(n);
    for (int i=n-1; i>=0; i--) {
        x[i] = A[i][n]/A[i][i];
        for (int k=i-1;k>=0; k--) {
            A[k][n] -= A[k][i] * x[i];
        }
    }
    return x;
}

/*
 * 0 < x <= 0.25 North
 * 0.25 < x <= 0.50 East
 * 0.50 < x <= 0.75 South
 * 0.75 < x <= 1.0 West
 */
void tempdist::solveMonteCarlo(unsigned int h, unsigned int l)
{
    double tTop   = lines.at(3).temp,
           tLeft  = lines.at(0).temp,
           tRight = lines.at(2).temp,
           tBottom  = lines.at(1).temp;


    res = vector< vector<double> >(h,vector<double>(l));
    for(unsigned int i=0;i<h;i++)
    {
        for(unsigned int j=0;j<l;j++)
        {
            if(i==0 && lines.at(1).type == 1){res[i][j] = tTop;}
            if(i==h-1 && lines.at(3).type == 1){res[i][j] = tBottom;}
            if(j==l-1 && lines.at(2).type == 1){res[i][j] = tRight;}
            if(j==0 && lines.at(0).type == 1){res[i][j] = tLeft;}
        }
    }



    for(unsigned int i=1;i<h-1;i++)
    {
        for(unsigned int j=1;j<l-1;j++)
        {
            double total=0;

                int tn=0; // test number

            while(tn < 10000)
            {
                int a=i,
                    b=j;

                while(true)
                {
                    if(a == h-1 || b == l-1 || a == 0 || b == 0) {res[i][j] += res[a][b] / 10000; break;}


                    std::random_device rd;
                    std::mt19937_64 gen(rd());
                    std::uniform_real_distribution<> dis(0, 1);
                    double rn = dis(gen);
                    //qDebug() << rn;

                    if(rn<=0.25 ) {b--;} // N
                    else if(rn<=0.5 ) {a++;} // E
                    else if(rn<=0.75 ) {b++;} // S
                    else if(rn<=1 ) {a--;} // W

                    if(_aborted) { return; }
                }
                tn++;
            }
        }
    }

    printv(res);
}

void tempdist::solveAll(unsigned int h, unsigned int l, double epsilon,double k)
{

    double maxerr = 10000;

    double tTop   = lines.at(3).temp,
           tLeft  = lines.at(0).temp,
           tRight = lines.at(2).temp,
           tBottom  = lines.at(1).temp;

    double old;

    double delta = 1;

    double tInit = (tTop+tLeft+tRight+tBottom)*0.25;



    res = vector< vector<double> >(h,vector<double>(l,tInit));

    for(unsigned int i=0;i<h;i++)
    {
        for(unsigned int j=0;j<l;j++)
        {
            if(i==0 && lines.at(1).type == 1){res[i][j] = tTop;}
            if(i==h-1 && lines.at(3).type == 1){res[i][j] = tBottom;}
            if(j==l-1 && lines.at(2).type == 1){res[i][j] = tRight;}
            if(j==0 && lines.at(0).type == 1){res[i][j] = tLeft;}
        }
    }

    it=0;

        while(maxerr > epsilon)
        {

            double iterr=0;
            for (unsigned i = 1; i < h-1; ++ i)
            {
                for (unsigned j = 1; j < l-1; ++ j)
                {
                    if(_aborted) break;
                    /*if(j==l-1 && i!=h-1)// && lines.at(2).type == 2)
                    {
                        res[i][j] = (res[i-1][j]+res[i+1][j]+2*res[i][j-1]+2*lines.at(2).h*delta*tRight/k)/(4 + 2*lines.at(2).h*delta/k);
                    }else if(j==l-1 && lines.at(2).type == 3)
                    {
                        res[i][j] = (2*res[i][j-1]+res[i-1][j]+res[i+1][j])*0.25;
                    }if(j==0 && lines.at(0).type == 2)
                    {
                        res[i][j] = (res[i-1][j]+res[i+1][j]+2*res[i][j+1]+2*lines.at(0).h*tLeft/k)/(4 + 2*lines.at(0).h/k);
                    }if(j==0 && lines.at(0).type == 3)
                    {
                        res[i][j] = (2*res[i][j+1]+res[i-1][j]+res[i+1][j])*0.25;
                    }if(i==h-1 && lines.at(3).type == 2)
                    {
                        res[i][j] = (res[i][j-1]+res[i][j+1]+2*res[i-1][j]+2*lines.at(3).h*tBottom/k)/(4 + 2*lines.at(3).h/k);
                    }else if(i==h-1 && lines.at(3).type == 3)
                    {
                        res[i][j] = (2*res[i-1][j]+res[i][j+1]+res[i][j-1])*0.25;
                    }if(i==0 && lines.at(1).type == 2)
                    {
                        res[i][j] = (res[i][j-1]+res[i][j+1]+2*res[i+1][j]+2*lines.at(1).h*tTop/k)/(4 + 2*lines.at(1).h/k);
                    }else if(i==0 && lines.at(1).type == 3)
                    {
                        res[i][j] = (2*res[i+1][j]+res[i][j-1]+res[i][j+1])*0.25;
                    }*/
                    if(j==0 && lines.at(0).type == 3)
                    {
                        res[i][j] = (2*res[i][j+1]+res[i-1][j]+res[i+1][j])*0.25;
                    }else if(i<h-1 && j<l-1){

                        old = res[i][j];
                        res[i][j] = (res[i][j+1] + res[i][j-1] + res[i+1][j] + res[i-1][j])*0.25;

                        if(std::abs(old-res[i][j])>iterr)
                        {
                            iterr = std::abs(old-res[i][j]);
                        }

                    }
                }
            }
            maxerr = iterr;
            it++;
        }

    //printv(res);
    qDebug() << "İterasyon Sayısı:" << it;
}

void tempdist::plotDistribution()
{
    ui->tempGraph->clearPlottables();
    ui->tempGraph->clearGraphs();
    ui->tempGraph->clearItems();
    ui->tempGraph->plotLayout()->removeAt(1);
    ui->tempGraph->plotLayout()->setAntialiased(true);

    ui->tempGraph->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectAxes |
                                QCP::iSelectLegend | QCP::iSelectPlottables);
    ui->tempGraph->xAxis->setLabel("x");

    ui->tempGraph->axisRect()->setupFullAxesBox(true);
    ui->tempGraph->yAxis->setLabel("y");


    QCPColorMap *colorMap = new QCPColorMap(ui->tempGraph->xAxis, ui->tempGraph->yAxis);
    ui->tempGraph->addPlottable(colorMap);

    int h=res.size();
    int l=res[0].size();

    colorMap->data()->setSize(l, h);
    colorMap->data()->setRange(QCPRange(0.54, l), QCPRange(0.56, h)); // and span the coordinate range
    for (int xIndex=0; xIndex<h; ++xIndex)
    {
      for (int yIndex=0; yIndex<l; ++yIndex)
      {
        colorMap->data()->setCell(yIndex, xIndex, res[xIndex][yIndex]);
        //qDebug() << xIndex << yIndex << res[xIndex][yIndex];
      }
    }


    QCPColorScale *colorScale = new QCPColorScale(ui->tempGraph);
    ui->tempGraph->plotLayout()->addElement(0, 1, colorScale);
    colorScale->setType(QCPAxis::atRight);
    colorMap->setColorScale(colorScale);
    colorScale->axis()->setLabel("Sıcaklık");


    colorMap->setGradient(QCPColorGradient::gpThermal);

    colorMap->rescaleDataRange();

    QCPMarginGroup *marginGroup = new QCPMarginGroup(ui->tempGraph);
    ui->tempGraph->axisRect()->setMarginGroup(QCP::msBottom|QCP::msTop, marginGroup);
    colorScale->setMarginGroup(QCP::msBottom|QCP::msTop, marginGroup);

    ui->tempGraph->rescaleAxes();
    ui->tempGraph->replot();
    ui->tempGraph->show();
}

void tempdist::drawMesh()
{

    if(!meshLines->childItems().isEmpty()) {scene->removeItem(meshLines);}

    QList<QGraphicsItem*> itemList;

    int h=res.size();
    int l=res[0].size();

    double deltal = 500/l;
    double deltah = 300/h;

    for(int i=l;i>-1;i--)
    {
        itemList.append(scene->addLine(i*deltal+1,0,i*deltal+1,300,QPen(QColor(Qt::blue))));
    }

    for(int i=h;i>-1;i--)
    {
        itemList.append(scene->addLine(0,i*deltah+1,500,i*deltah+1,QPen(QColor(Qt::blue))));
    }

    meshLines = scene->createItemGroup(itemList);
    meshLines->setZValue(6);

    ui->actionDisplay_Mesh_on_off->setChecked(true);



}

/*
void tempdist::solve(double tLeft,double tRight,double tTop,double tBottom,unsigned int h,unsigned int l)
{

    using namespace std;

    double Told = 1000;
    double Tnew = 0;
    int ac=1;
    double epsilon = 0.1;
    int divC = 2; // dividing coefficient


    unsigned int sizeH = h - 2,
                 sizeL = l-2;

    unsigned int dof = sizeH * sizeL;

    Eigen::VectorXd x(dof);

/*
    std::cout << "Here is the matrix A:\n" << A << std::endl;
    std::cout << "Here is the vector b:\n" << b << std::endl;
    Eigen::VectorXd x = A.ldlt().solve(b);
    std::cout << "The solution is:\n" << x << std::endl;
    //VectorXd x(dof);


    Eigen::MatrixXd m = Eigen::MatrixXd::Zero(sizeH,sizeL);
    Eigen::MatrixXd resMatrix = Eigen::MatrixXd::Zero(dof,dof);
    Eigen::VectorXd resVector(dof);

    while(std::abs(Told-Tnew) > epsilon)
    {

        x.setZero(dof);
        m.setZero(sizeH,sizeL);
        resMatrix.setZero(dof,dof);
        resVector.setZero(dof);

        int resRow=0;

        for (unsigned i = 0; i < sizeH; ++ i)
        {
            for (unsigned j = 0; j < sizeL; ++ j)
            {
                m(i,j) = sizeL * i + j;
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

                resVector(resRow) = resValue;

                for(unsigned int k=0;k<dof;++k)
                {

                    resMatrix(resRow,m(i,j))=4;

                    if(tr==false){resMatrix(resRow,m(i,j+1))=-1;}
                    if(tb==false){resMatrix(resRow,m(i+1,j))=-1;}
                    if(tt==false){resMatrix(resRow,m(i-1,j))=-1;}
                    if(tl==false){resMatrix(resRow,m(i,j-1))=-1;}

                }

                ++resRow;
            }
        }

        Told = Tnew;



        x = resMatrix.ldlt().solve(resVector);
        //std::cout << "Here is the matrix m:\n" << m << std::endl;
        //std::cout << "Here is the matrix resMatrix:\n" << resMatrix << std::endl;
        //std::cout << "Here is the vector resVector:\n" << resVector << std::endl;
        //std::cout << "Here is the vector results:\n" << x << std::endl;

        int med = (sizeL * 2 + 1)*ac;

        Tnew = x(med);

        qDebug() << "DOF" << dof;

        qDebug() << "ESKİ DEĞER = " << Told << "YENİ DEĞER" << Tnew;

        qDebug() << "SEÇİLEN ELEMAN" << med << "SizeH,SizeL =" << sizeH << sizeL ;

        sizeH *= divC;
        sizeL *= divC;
        dof = sizeH*sizeL;
        ac *= divC;
    }




    QString s;
    s.append("Result:");
       for (unsigned int i=0; i<dof/(divC*divC); i++) {
           s = s + "\n T_" + QString::number(i) + "= "+ QString::number(x(i)) + "C°";
       }
       ui->textEdit->setText(s);

       ui->textEdit->show();

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


void tempdist::solveAll(double tLeft,double tRight,double tTop,double tBottom,double ht,double k,unsigned int h,unsigned int l)
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
            if(j == sizeL-1 ){resValue+=2*ht*0.5*tRight/k;tr=true;}
            if(i == sizeH-1 ){resValue+=tBottom;tb=true;}
            if(i == 0 ){resValue+=tTop;tt=true;}
            if(j == 0 ){resValue+=tLeft;tl=true;}

            resMatrix[resRow][dof] = resValue;

            for(int k=0;k<dof;++k)
            {
                if(j==sizeL-1)
                {
                    qDebug("asdsadsadsadsad");
                    resMatrix[resRow][m[i][j]]=4+2*ht*0.5/k;
                    resMatrix[resRow][m[i][j-1]]=-2;
                    tr==true;
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

    vector<double> results(dof);
    results = gauss(resMatrix);

    vector< vector<double> > results_matrix(h,vector<double>(l,0));

    for(int i=0;i<h;i++)
    {
        for(int j=0;j<l;j++)
        {
            if(i==0){results_matrix[i][j] = tTop;}
            if(i==h-1){results_matrix[i][j] = tBottom;}
            if(j==0){results_matrix[i][j] = tLeft;}
        }
    }

    for (unsigned i = 0; i < sizeH; ++ i)
    {
        for (unsigned j = 0; j < l-1; ++ j)
        {

            results_matrix[i+1][j+1] = results[sizeL * i + j];

        }
    }

    printv(results_matrix);

    print(resMatrix);

    ui->tempGraph->clearPlottables();
    ui->tempGraph->clearGraphs();
    ui->tempGraph->clearItems();

    ui->tempGraph->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectAxes |
                                QCP::iSelectLegend | QCP::iSelectPlottables);
    ui->tempGraph->xAxis->setLabel("x");
    ui->tempGraph->axisRect()->setupFullAxesBox(true);
    ui->tempGraph->yAxis->setLabel("y");


    QCPColorMap *colorMap = new QCPColorMap(ui->tempGraph->xAxis, ui->tempGraph->yAxis);
    ui->tempGraph->addPlottable(colorMap);
    int nx = h;
    int ny = l;
    colorMap->data()->setSize(nx, ny);
    colorMap->data()->setRange(QCPRange(0, nx), QCPRange(0, ny));


    double x, y, z;
    for (int xIndex=0; xIndex<nx; ++xIndex)
    {
      for (int yIndex=0; yIndex<ny; ++yIndex)
      {

        colorMap->data()->cellToCoord(xIndex, yIndex, &x, &y);

        z=results_matrix[xIndex][yIndex];
        qDebug() << "data" << xIndex << yIndex << x << y << z;
        colorMap->data()->setCell(xIndex, yIndex, z);
      }
    }


    QCPColorScale *colorScale = new QCPColorScale(ui->tempGraph);
    ui->tempGraph->plotLayout()->addElement(0, 1, colorScale);

    //colorScale->setType(QCPAxis::atRight);
    colorMap->setColorScale(colorScale);
    colorScale->axis()->setLabel("Sıcaklık");


    colorMap->setGradient(QCPColorGradient::gpThermal);

    colorMap->rescaleDataRange();

    QCPMarginGroup *marginGroup = new QCPMarginGroup(ui->tempGraph);
    ui->tempGraph->axisRect()->setMarginGroup(QCP::msBottom|QCP::msTop, marginGroup);
    colorScale->setMarginGroup(QCP::msBottom|QCP::msTop, marginGroup);

    ui->tempGraph->rescaleAxes();
    ui->tempGraph->replot();

    ui->textEdit->show();

    //delete colorScale,colorMap,marginGroup;

    ui->textEdit->show();

    QString s;
    s.append("Result:");
       for (unsigned int i=0; i<dof; i++) {
           s = s + "\n T_" + QString::number(i) + "= "+ QString::number(results[i]) + "C°";
       }
       ui->textEdit->setText(s);


}*/

void tempdist::denemee(int t)
{
    if(t==1)
    {
        solveAll(ui->height->text().toInt(),
             ui->width->text().toInt(),
             ui->epsilon->text().toDouble(),
             ui->kvalue->text().toDouble());
    }else{
        solveMonteCarlo(ui->height->text().toInt(),
                        ui->width->text().toInt());
    }



}

void tempdist::on_actionSolve_triggered()
{
    _aborted =false;
    QProgressDialog dialog;
    dialog.setLabelText(QString("Progressing using full thread(s)..."));

    QFutureWatcher<void> futureWatcher;
    QObject::connect(&futureWatcher, SIGNAL(finished()), &dialog, SLOT(reset()));
    QObject::connect(&futureWatcher, SIGNAL(finished()), this, SLOT(plotDistribution()));
    QObject::connect(&futureWatcher, SIGNAL(finished()), this, SLOT(drawMesh()));
    QObject::connect(&dialog, SIGNAL(canceled()), &futureWatcher, SLOT(cancel()));
    QObject::connect(&dialog, &QProgressDialog::canceled,[=](){
         _aborted = true;
         qDebug("cancelllllll");
    });
    QObject::connect(&futureWatcher, SIGNAL(progressRangeChanged(int,int)), &dialog, SLOT(setRange(int,int)));
    QObject::connect(&futureWatcher, SIGNAL(progressValueChanged(int)), &dialog, SLOT(setValue(int)));


    QTime myTimer;
    myTimer.start();

    // Start the computation.
    QFuture<void> t1 = QtConcurrent::run(this,&tempdist::denemee,1);
    futureWatcher.setFuture(t1);

    // Display the dialog and start the event loop.
    dialog.exec();

    futureWatcher.waitForFinished();



    int nMilliseconds = myTimer.elapsed();

    ui->textEdit->show();
    if(_aborted)
    {
        ui->textEdit->setText(ui->textEdit->toHtml() + "<strong style='color:red;'>İşlem kullanıcı tarafından iptal edildi</strong>");
    }else{
        ui->textEdit->setText(ui->textEdit->toHtml() + "<h3>İteratif Yöntem ( Süre = " + QString::number(nMilliseconds) + "ms | " + QString::number(it) + " iterasyon )</h3><p>" + matrixToString(res) + "</p>");
    }

    ui->textEdit->moveCursor(QTextCursor::End);


    // Start the computation.
    QFuture<void> t2 = QtConcurrent::run(this,&tempdist::denemee,0);
    futureWatcher.setFuture(t2);

    // Display the dialog and start the event loop.
    dialog.exec();

    futureWatcher.waitForFinished();

    nMilliseconds = myTimer.elapsed();

    ui->textEdit->show();

    if(_aborted)
    {
        ui->textEdit->setText(ui->textEdit->toHtml() + "<strong style='color:red;'>İşlem kullanıcı tarafından iptal edildi</strong>");
    }else{
        ui->textEdit->setText(ui->textEdit->toHtml() + "<h3>Monte Carlo Yöntemi ( Süre = " + QString::number(nMilliseconds) + "ms )</h3><p>" + matrixToString(res) + "</p>");
    }


    ui->textEdit->moveCursor(QTextCursor::End);

   /* solveAll(lines.at(0).temp,
             lines.at(2).temp,
             lines.at(1).temp,
             lines.at(3).temp,
             lines.at(2).h,
             ui->lineEdit->text().toDouble(),
             ui->height->text().toInt(),
             ui->width->text().toInt());

    solveAll(lines.at(0).temp,
             lines.at(2).temp,
             lines.at(1).temp,
             lines.at(3).temp,
             lines.at(2).h,
             ui->lineEdit->text().toDouble(),
             ui->height->text().toInt(),
             ui->width->text().toInt());*/
}

void tempdist::on_actionDisplay_Mesh_on_off_changed()
{

    if(ui->actionDisplay_Mesh_on_off->isChecked())
    {
       meshLines->show();
    }else{
       meshLines->hide();
    }

}
