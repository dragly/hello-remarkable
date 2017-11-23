#ifndef TABLETWINDOW_H
#define TABLETWINDOW_H

#include <QQuickWindow>
#include "tabletcanvas.h"

class TabletWindow : public QQuickWindow
{
    Q_OBJECT
    Q_PROPERTY(TabletCanvas* canvas READ canvas WRITE setCanvas NOTIFY canvasChanged)

public:
    explicit TabletWindow(QWindow *parent = nullptr);

    QQmlListProperty<TabletCanvas> canvases();
    TabletCanvas* canvas() const;

public slots:
    void setCanvas(TabletCanvas* canvas);

signals:
    void penEvent(QPointF position, qreal pressure);
    void canvasChanged(TabletCanvas* canvas);

protected:
    virtual void tabletEvent(QTabletEvent *) override;

    // QWindow interface
protected:
    TabletCanvas* m_canvas = nullptr;
};

#endif // TABLETWINDOW_H
