/*
 * This file is part of the Genealogy Maker program (https://github.com/martinvanzijl/genealogy-maker).
 * Copyright (c) 2023 Martin van Zijl.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, version 3.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include "arrow.h"
#include "marriageitem.h"

#include <math.h>

#include <QPen>
#include <QPainter>
#include <QDebug>

#include <QtMath>

const qreal Pi = 3.14;

static int mDefaultLineWidth = 2;

//! [0]
Arrow::Arrow(DiagramItem *startItem, DiagramItem *endItem, QGraphicsItem *parent)
    : QGraphicsLineItem(parent)
{
    myStartItem = startItem;
    myEndItem = endItem;
    setFlag(QGraphicsItem::ItemIsSelectable, true);
    myColor = Qt::black;
    int lineWidth = getDefaultLineWidth();
    setPen(QPen(myColor, lineWidth, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
}
//! [0]

//! [1]
QRectF Arrow::boundingRect() const
{
    qreal extra = (pen().width() + 20) / 2.0;

    return QRectF(line().p1(), QSizeF(line().p2().x() - line().p1().x(),
                                      line().p2().y() - line().p1().y()))
        .normalized()
        .adjusted(-extra, -extra, extra, extra);
}
//! [1]

//! [2]
QPainterPath Arrow::shape() const
{
    // Get the original path.
    QPainterPath path = QGraphicsLineItem::shape();
    path.addPolygon(arrowHead);
    return path;

    // TEST: The code below makes the line click arrow wider,
    // but does not detect clicks on the arrowhead.

//    // Get translation coordinates.
//    auto translateX = path.boundingRect().center().x();
//    auto translateY = path.boundingRect().center().y();

//    // Translate the path to the origin.
//    path.translate(-translateX, -translateY);

//    // Scale the path.
//    QTransform transform;
//    transform.scale(100, 100);
//    path = transform.map(path);

//    // Translate the path back to its original position.
//    path.translate(translateX, translateY);

//    // Return the path.
//    return path;
}

QColor Arrow::getColor() const
{
    return myColor;
}

//static QPainterPath pathScaled(QPainterPath path, double scale) {
//    // Get translation coordinates.
//    auto translateX = path.boundingRect().center().x();
//    auto translateY = path.boundingRect().center().y();

//    // Translate the path to the origin.
//    path.translate(-translateX, -translateY);

//    // Scale the path.
//    QTransform transform;
//    transform.scale(scale, scale);
//    path = transform.map(path);

//    // Translate the path back to its original position.
//    path.translate(translateX, translateY);

//    // Return the path.
//    return path;
//}

void Arrow::updatePosition()
{
    QGraphicsItem *startItem = myStartItem;
    if (myStartItem->isMarried()) {
        startItem = myStartItem->getMarriageItem();
    }

    QLineF line(mapFromItem(startItem, 0, 0), mapFromItem(myEndItem, 0, 0));
    setLine(line);
}

void Arrow::setLineWidth(int width)
{
    QPen myPen = pen();
    myPen.setWidth(width);
    setPen(myPen);
}

int Arrow::getDefaultLineWidth()
{
    return mDefaultLineWidth;
}

void Arrow::setDefaultLineWidth(int width)
{
    mDefaultLineWidth = width;
}

void Arrow::paint(QPainter *painter, const QStyleOptionGraphicsItem *,
          QWidget *)
{
    if (myStartItem->collidesWithItem(myEndItem))
        return;

    QPen myPen = pen();
    myPen.setColor(myColor);
    qreal arrowSize = 20;
    painter->setPen(myPen);
    painter->setBrush(myColor);

    QPointF startPos = myStartItem->pos();
    if (myStartItem->isMarried()) {
        startPos = myStartItem->getMarriageItemPos();
    }

    QLineF centerLine(startPos, myEndItem->pos());
    QPolygonF endPolygon = myEndItem->polygon();
    QPointF p1 = endPolygon.first() + myEndItem->pos();
    QPointF p2;
    QPointF intersectPoint;
    QLineF polyLine;
    for (int i = 1; i < endPolygon.count(); ++i) {
        p2 = endPolygon.at(i) + myEndItem->pos();
        polyLine = QLineF(p1, p2);
        QLineF::IntersectType intersectType =
            polyLine.intersect(centerLine, &intersectPoint);
        if (intersectType == QLineF::BoundedIntersection)
            break;
        p1 = p2;
    }

    setLine(QLineF(intersectPoint, startPos));

    double angle = ::acos(line().dx() / line().length());
    if (line().dy() >= 0)
        angle = (Pi * 2) - angle;

    QPointF arrowP1 = line().p1() + QPointF(sin(angle + Pi / 3) * arrowSize,
                                    cos(angle + Pi / 3) * arrowSize);
    QPointF arrowP2 = line().p1() + QPointF(sin(angle + Pi - Pi / 3) * arrowSize,
                                    cos(angle + Pi - Pi / 3) * arrowSize);

    arrowHead.clear();
    arrowHead << line().p1() << arrowP1 << arrowP2;

    painter->drawLine(line());
    painter->drawPolygon(arrowHead);
    if (isSelected()) {
        // Get the original line.
        QLineF myLine = line();

        if (myPen.width() > 2) {
            // Draw a white dotted line along the middle if the line width is large.
            painter->setPen(QPen(Qt::white, 1, Qt::DashLine));
            painter->drawLine(myLine);
        }
        else
        {
            // Draw two dotted lines beside the arrow if the line width is small.
            painter->setPen(QPen(myColor, 1, Qt::DashLine));

            // Calculate line angle.
            auto angle = qDegreesToRadians(myLine.angle());

            // Distance between main line and highlighting lines.
            double delta = 4.0;

            // Draw the lines.
            QLineF highlightLine = myLine;

            highlightLine.translate(delta * sin(angle), delta * cos(angle));
            painter->drawLine(highlightLine);

            highlightLine = myLine;
            myLine.translate(-delta * sin(angle), -delta * cos(angle));

            painter->drawLine(myLine);
        }

        // Draw the scaled dotted line in red.
        // This does not quite work.
//        painter->setBrush(QBrush());
//        painter->setPen(QPen(Qt::red, 1, Qt::DashLine));
//        auto scaledPath = pathScaled(shape(), 1.2);
//        painter->drawPath(scaledPath);
    }
}

