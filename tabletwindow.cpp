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

    for (TabletCanvas *canvas : m_canvases) {
        QPointF offset = rotated - canvas->position();
        QTabletEvent *fakeEvent = new QTabletEvent(event->type(), offset, rotatedGlobal, event->device(), event->pointerType(), event->pressure(), event->xTilt(), event->yTilt(), event->tangentialPressure(), event->rotation(), event->z(), event->modifiers(), event->uniqueId());
        canvas->tabletEvent(fakeEvent);
    }
}

bool TabletWindow::event(QEvent *event)
{
    if (dynamic_cast<QTimerEvent*>(event)) {
        return QQuickWindow::event(event);
    }
//    qDebug() << "Event!" << event;
    return QQuickWindow::event(event);
}

void TabletWindow::mousePressEvent(QMouseEvent *event)
{
    QQuickWindow::mousePressEvent(event);
    qDebug() << "Press event!" << event;
}

void TabletWindow::touchEvent(QTouchEvent *event)
{
    qDebug() << "Touch event!" << event;
    QQuickWindow::touchEvent(event);
}

bool TabletWindow::nativeEvent(const QByteArray &eventType, void *message, long *result)
{
    qDebug() << "Native event!" << eventType;
    return QQuickWindow::nativeEvent(eventType, message, result);
}

QQmlListProperty<TabletCanvas> TabletWindow::canvases()
{
    return QQmlListProperty<TabletCanvas>(this, m_canvases);
}
