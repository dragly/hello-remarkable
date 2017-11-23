#ifndef TABLETWINDOW_H
#define TABLETWINDOW_H

#include <QQuickWindow>
#include "tabletcanvas.h"

class TabletWindow : public QQuickWindow
{
    Q_OBJECT
    Q_PROPERTY(QQmlListProperty<TabletCanvas> canvases READ canvases)

public:
    explicit TabletWindow(QWindow *parent = nullptr);

    QQmlListProperty<TabletCanvas> canvases();
signals:
    void penEvent(QPointF position, qreal pressure);

    void canvasChanged(TabletCanvas* canvas);

protected:
    virtual void tabletEvent(QTabletEvent *) override;

    virtual bool event(QEvent *event) override;

    // QWindow interface
protected:
    virtual void mousePressEvent(QMouseEvent *) override;
    virtual void touchEvent(QTouchEvent *) override;
    virtual bool nativeEvent(const QByteArray &eventType, void *message, long *result) override;

    QList<TabletCanvas *> m_canvases;
};

#endif // TABLETWINDOW_H
