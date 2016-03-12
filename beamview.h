#ifndef BEAMVIEW_H
#define BEAMVIEW_H

#include <QGraphicsView>
#include <QtWidgets>
#include <QWidget>

class BeamView : public QGraphicsView
{
public:
    BeamView(QWidget *parent=0);
protected:
    virtual void wheelEvent(QWheelEvent *event);
};

#endif // BEAMVIEW_H
