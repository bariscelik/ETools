#include "beamview.h"

BeamView::BeamView(QWidget *parent) : QGraphicsView(parent)
{

}

void BeamView::wheelEvent(QWheelEvent *event)
{
    setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
    double scaleF = 1.07;

    if(event->delta() > 0)
    {
        scale(scaleF,scaleF);
    }else{
        scale(1/scaleF,1/scaleF);
    }
}

