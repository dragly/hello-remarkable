/****************************************************************************
**
** Copyright (C) 2015 The Qt Company Ltd.
** Contact: http://www.qt.io/licensing/
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** You may use this file under the terms of the BSD license as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of The Qt Company Ltd nor the names of its
**     contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "tabletcanvas.h"

#include <QtMath>
#include <QCursor>
#include <QPainter>
#include <QDebug>

//! [0]
TabletCanvas::TabletCanvas(QQuickItem *parent)
  : QQuickItem (parent)
  , m_alphaChannelValuator(TangentialPressureValuator)
  , m_colorSaturationValuator(NoValuator)
  , m_lineWidthValuator(PressureValuator)
  , m_color(Qt::red)
  , m_brush(m_color)
  , m_pen(m_brush, 1.0, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin)
  , m_deviceDown(false)
{
    initPixmap();

    m_canvasWidth = 1404 / m_columns;
    m_canvasHeight = 1872 / m_rows;
    for (int i = 0; i < m_rows; i++) {
        m_paintedCanvasMap.append(QVector<PaintedCanvas*>());
        for (int j = 0; j < m_columns; j++) {
            qreal x = j * m_canvasWidth;
            qreal y = i * m_canvasHeight;
            PaintedCanvas *canvas = new PaintedCanvas(QRectF(x, y, m_canvasWidth, m_canvasHeight), &m_pixmap, this);
            m_paintedCanvases.append(canvas);
            m_paintedCanvasMap[i].append(canvas);
        }
    }
}

void TabletCanvas::initPixmap()
{
    QPixmap newPixmap = QPixmap(1404, 1872);
    newPixmap.fill(Qt::transparent);
    QPainter painter(&newPixmap);
    if (!m_pixmap.isNull())
        painter.drawPixmap(0, 0, m_pixmap);
    painter.end();
    m_pixmap = newPixmap;
}
//! [0]

//! [3]
void TabletCanvas::tabletEvent(QTabletEvent *event)
{
    switch (event->type()) {
        case QEvent::TabletPress:
            if (!m_deviceDown) {
                m_deviceDown = true;
                lastPoint.pos = event->posF();
                lastPoint.rotation = event->rotation();
            }
            break;
        case QEvent::TabletMove:
            if (event->device() == QTabletEvent::RotationStylus)
                updateCursor(event);
            if (m_deviceDown) {
                updateBrush(event);
                QPainter painter(&m_pixmap);
                paintPixmap(painter, event);
                lastPoint.pos = event->posF();
                lastPoint.rotation = event->rotation();
            }
            break;
        case QEvent::TabletRelease:
            if (m_deviceDown && event->buttons() == Qt::NoButton)
                m_deviceDown = false;
            break;
        default:
            break;
    }
    int i = event->posF().y() / m_canvasHeight;
    int j = event->posF().x() / m_canvasWidth;

    if (m_paintedCanvasMap.size() < 1) {
        qWarning() << "Error: no canvases in map" << event->posF();
        return;
    }
    if (i >= m_paintedCanvasMap.size()) {
        qWarning() << "Error: event outside all canvases" << event->posF();
        return;
    }
    if (j >= m_paintedCanvasMap[i].size()) {
        qWarning() << "Error: event outside all canvases" << event->posF();
        return;
    }

    PaintedCanvas *canvas = m_paintedCanvasMap[i][j];
    canvas->update();
}

void TabletCanvas::paintPixmap(QPainter &painter, QTabletEvent *event)
{
//    painter.setRenderHint(QPainter::Antialiasing);

    switch (event->device()) {
//! [6]
        case QTabletEvent::Airbrush:
            {
                painter.setPen(Qt::NoPen);
                QRadialGradient grad(lastPoint.pos, m_pen.widthF() * 10.0);
                QColor color = m_brush.color();
                color.setAlphaF(color.alphaF() * 0.25);
                grad.setColorAt(0, m_brush.color());
                grad.setColorAt(0.5, Qt::transparent);
                painter.setBrush(grad);
                qreal radius = grad.radius();
                painter.drawEllipse(event->posF(), radius, radius);
            }
            break;
        case QTabletEvent::RotationStylus:
            {
                m_brush.setStyle(Qt::SolidPattern);
                painter.setPen(Qt::NoPen);
                painter.setBrush(m_brush);
                QPolygonF poly;
                qreal halfWidth = m_pen.widthF();
                QPointF brushAdjust(qSin(qDegreesToRadians(lastPoint.rotation)) * halfWidth,
                                    qCos(qDegreesToRadians(lastPoint.rotation)) * halfWidth);
                poly << lastPoint.pos + brushAdjust;
                poly << lastPoint.pos - brushAdjust;
                brushAdjust = QPointF(qSin(qDegreesToRadians(event->rotation())) * halfWidth,
                                      qCos(qDegreesToRadians(event->rotation())) * halfWidth);
                poly << event->posF() - brushAdjust;
                poly << event->posF() + brushAdjust;
                painter.drawConvexPolygon(poly);
            }
            break;
        default:
            {
                const QString error(tr("Unknown tablet device - treating as stylus"));
            }
            // FALL-THROUGH
        case QTabletEvent::Stylus:
            painter.setPen(m_pen);
            painter.drawLine(lastPoint.pos, event->posF());
            break;
    }
}
//! [5]

//! [7]
void TabletCanvas::updateBrush(const QTabletEvent *event)
{
    int hue, saturation, value, alpha;
    m_color.getHsv(&hue, &saturation, &value, &alpha);

    int vValue = int(((event->yTilt() + 60.0) / 120.0) * 255);
    int hValue = int(((event->xTilt() + 60.0) / 120.0) * 255);
//! [7] //! [8]

    switch (m_alphaChannelValuator) {
        case PressureValuator:
            m_color.setAlphaF(event->pressure());
            break;
        case TangentialPressureValuator:
            if (event->device() == QTabletEvent::Airbrush)
                m_color.setAlphaF(qMax(0.01, (event->tangentialPressure() + 1.0) / 2.0));
            else
                m_color.setAlpha(255);
            break;
        case TiltValuator:
            m_color.setAlpha(maximum(abs(vValue - 127), abs(hValue - 127)));
            break;
        default:
            m_color.setAlpha(255);
    }

//! [8] //! [9]
    switch (m_colorSaturationValuator) {
        case VTiltValuator:
            m_color.setHsv(hue, vValue, value, alpha);
            break;
        case HTiltValuator:
            m_color.setHsv(hue, hValue, value, alpha);
            break;
        case PressureValuator:
            m_color.setHsv(hue, int(event->pressure() * 255.0), value, alpha);
            break;
        default:
            ;
    }

//! [9] //! [10]
    switch (m_lineWidthValuator) {
        case PressureValuator:
            m_pen.setWidthF(event->pressure() * 10 + 1);
            break;
        case TiltValuator:
            m_pen.setWidthF(maximum(abs(vValue - 127), abs(hValue - 127)) / 12);
            break;
        default:
            m_pen.setWidthF(1);
    }

//! [10] //! [11]
    if (event->pointerType() == QTabletEvent::Eraser) {
        m_brush.setColor(Qt::white);
        m_pen.setColor(Qt::white);
        m_pen.setWidthF(event->pressure() * 10 + 1);
    } else {
        m_brush.setColor(m_color);
        m_pen.setColor(m_color);
    }
}
//! [11]

//! [12]
void TabletCanvas::updateCursor(const QTabletEvent *event)
{
    QCursor cursor;
    if (event->type() != QEvent::TabletLeaveProximity) {
        if (event->pointerType() == QTabletEvent::Eraser) {
            cursor = QCursor(QPixmap(":/images/cursor-eraser.png"), 3, 28);
        } else {
            switch (event->device()) {
            case QTabletEvent::Stylus:
                cursor = QCursor(QPixmap(":/images/cursor-pencil.png"), 0, 0);
                break;
            case QTabletEvent::Airbrush:
                cursor = QCursor(QPixmap(":/images/cursor-airbrush.png"), 3, 4);
                break;
            case QTabletEvent::RotationStylus: {
                QImage origImg(QLatin1String(":/images/cursor-felt-marker.png"));
                QImage img(32, 32, QImage::Format_ARGB32);
                QColor solid = m_color;
                solid.setAlpha(255);
                img.fill(solid);
                QPainter painter(&img);
                QTransform transform = painter.transform();
                transform.translate(16, 16);
                transform.rotate(-event->rotation());
                painter.setTransform(transform);
                painter.setCompositionMode(QPainter::CompositionMode_DestinationIn);
                painter.drawImage(-24, -24, origImg);
                painter.setCompositionMode(QPainter::CompositionMode_HardLight);
                painter.drawImage(-24, -24, origImg);
                painter.end();
                cursor = QCursor(QPixmap::fromImage(img), 16, 16);
            } break;
            default:
                break;
            }
        }
    }
    setCursor(cursor);
}

PaintedCanvas::PaintedCanvas(QRectF rect, QPixmap *pixmap, QQuickItem *parent)
    : QQuickPaintedItem(parent)
    , m_pixmap(pixmap)
{
    setFillColor(Qt::transparent);
    setX(rect.x());
    setY(rect.y());
    setWidth(rect.width());
    setHeight(rect.height());
}

void PaintedCanvas::paint(QPainter *painter)
{
    painter->drawText(0, 0, QString::number(x()) + " " + QString::number(y()));
    painter->drawPixmap(boundingRect(), *m_pixmap, QRect(x(), y(), width(), height()));
}
