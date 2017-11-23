#include "tabletwindow.h"
#include "tabletcanvas.h"
#include <QDebug>
#include <QGuiApplication>
#include <QScreen>
#include <QQmlListProperty>

TabletWindow::TabletWindow(QWindow *parent) : QQuickWindow(parent)
{
//    setVisibility(QWindow::FullScreen);
//    setWidth(QGuiApplication::primaryScreen()->size().width());
//    setHeight(QGuiApplication::primaryScreen()->size().height());
}

QPointF rotate(QPointF other, qreal width, qreal height)
{
    return QPointF(other.y() / height * width, (1.0 - other.x() / width) * height);
}


void TabletWindow::tabletEvent(QTabletEvent *event)
{
//    qDebug() << event;

    QPointF rotated = rotate(event->posF(), width(), height());
    QPointF rotatedGlobal(rotate(event->globalPosF(), width(), height()));
    penEvent(rotated, event->pressure());

    if (m_canvas) {
        QPointF offset = rotated - m_canvas->position();
        QTabletEvent *fakeEvent = new QTabletEvent(event->type(), offset, rotatedGlobal, event->device(), event->pointerType(), event->pressure(), event->xTilt(), event->yTilt(), event->tangentialPressure(), event->rotation(), event->z(), event->modifiers(), event->uniqueId());
        m_canvas->tabletEvent(fakeEvent);
    }
}

TabletCanvas *TabletWindow::canvas() const
{
    return m_canvas;
}

void TabletWindow::setCanvas(TabletCanvas *canvas)
{
    if (m_canvas == canvas)
        return;

    m_canvas = canvas;
    emit canvasChanged(m_canvas);
}
