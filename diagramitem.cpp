/****************************************************************************
**
** Copyright (C) 2016 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** BSD License Usage
** Alternatively, you may use this file under the terms of the BSD license
** as follows:
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

#include "diagramitem.h"
#include "arrow.h"
#include "diagramtextitem.h"
#include "marriageitem.h"

#include <QGraphicsScene>
#include <QGraphicsSceneContextMenuEvent>
#include <QMenu>
#include <QPainter>
#include <QUuid>
#include <QGraphicsItemGroup>
#include <QDebug>

//! [0]
DiagramItem::DiagramItem(DiagramType diagramType, QMenu *contextMenu,
             QGraphicsItem *parent)
    : QGraphicsPolygonItem(parent)
{
    myDiagramType = diagramType;
    myContextMenu = contextMenu;

//    QPainterPath path;
    switch (myDiagramType) {
    //        case StartEnd:
    //            path.moveTo(200, 50);
    //            path.arcTo(150, 0, 50, 50, 0, 90);
    //            path.arcTo(50, 0, 50, 50, 90, 90);
    //            path.arcTo(50, 50, 50, 50, 180, 90);
    //            path.arcTo(150, 50, 50, 50, 270, 90);
    //            path.lineTo(200, 25);
    //            myPolygon = path.toFillPolygon();
    //            break;
    //        case Conditional:
    //            myPolygon << QPointF(-100, 0) << QPointF(0, 100)
    //                      << QPointF(100, 0) << QPointF(0, -100)
    //                      << QPointF(-100, 0);
    //            break;
    case Step:
    {
        int height = 25;
        int width = 100;
        myPolygon << QPointF(-width, -height) << QPointF(width, -height)
                  << QPointF(width, height) << QPointF(-width, height)
                  << QPointF(-width, -height);
        break;
    }
    default:
        myPolygon << QPointF(-120, -80) << QPointF(-70, 80)
                  << QPointF(120, 80) << QPointF(70, -80)
                  << QPointF(-120, -80);
        break;
    }
    setPolygon(myPolygon);
    setFlag(QGraphicsItem::ItemIsMovable, true);
    setFlag(QGraphicsItem::ItemIsSelectable, true);
    setFlag(QGraphicsItem::ItemSendsGeometryChanges, true);

    // Add child text.
    if (myDiagramType == Step) {
        m_textItem = new DiagramTextItem(this);
        m_textItem->setPlainText("New Person");
        m_textItem->setX(boundingRect().center().x() - m_textItem->boundingRect().width() / 2);
        m_textItem->setY(boundingRect().center().y() - m_textItem->boundingRect().height() / 2);
    }
    else {
        m_textItem = nullptr;
    }

    m_spouse = nullptr;
}
//! [0]

//! [1]
void DiagramItem::removeArrow(Arrow *arrow)
{
    int index = arrows.indexOf(arrow);

    if (index != -1)
        arrows.removeAt(index);
}
//! [1]

//! [2]
void DiagramItem::removeArrows()
{
    foreach (Arrow *arrow, arrows) {
        arrow->startItem()->removeArrow(arrow);
        arrow->endItem()->removeArrow(arrow);
        scene()->removeItem(arrow);
        delete arrow;
    }
}
//! [2]

//! [3]
void DiagramItem::addArrow(Arrow *arrow)
{
    arrows.append(arrow);
}
//! [3]

//! [4]
QPixmap DiagramItem::image() const
{
    QPixmap pixmap(250, 250);
    pixmap.fill(Qt::transparent);
    QPainter painter(&pixmap);
    painter.setPen(QPen(Qt::black, 8));
    painter.translate(125, 125);
    painter.drawPolyline(myPolygon);

    return pixmap;
}

QList<Arrow *> DiagramItem::getArrows() const
{
    return arrows;
}

QString DiagramItem::name() const
{
    if (m_textItem) {
        return m_textItem->text();
    }

    return "NO_NAME";
}

void DiagramItem::setName(QString value)
{
    if (m_textItem) {
        m_textItem->setPlainText(value);
    }
}

void DiagramItem::setHighlighted(bool value)
{
    if (value) {
        setPen(QPen(Qt::red, 2));
    }
    else {
        setPen(QPen(Qt::black, 1));
    }
}

void DiagramItem::marryTo(DiagramItem *spouse)
{
    // Set spouses.
    m_spouse = spouse;
    spouse->m_spouse = this;

    // Position together.
    m_spousePosition = SpouseToRight;
    spouse->m_spousePosition = SpouseToLeft;
    updateSpousePosition();

    // Move items together.
    //QList<QGraphicsItem *> list;
    //list << this << spouse;
    //scene()->createItemGroup(list);

    // Add "wedding ring".
    auto ring = new MarriageItem(this);
    ring->setX(boundingRect().width() / 2.0 - ring->boundingRect().width() / 2.0);
    ring->setY(-ring->boundingRect().height() / 2.0);

    // Move above spouse so that ring is always visible.
    if (zValue() <= spouse->zValue())
    {
        setZValue(spouse->zValue() + 0.1);
    }
}

QUuid DiagramItem::id() const
{
    return m_id;
}

void DiagramItem::setId(const QUuid& value)
{
    m_id = value;
}

DiagramTextItem *DiagramItem::textItem()
{
    return m_textItem;
}

void DiagramItem::editName()
{
    m_textItem->startEditing();
}

QString DiagramItem::bio() const
{
    return m_bio;
}

void DiagramItem::setBio(const QString &value)
{
    m_bio = value;
}

QStringList DiagramItem::photos() const
{
    return m_photos;
}

void DiagramItem::setPhotos(const QStringList &value)
{
    m_photos = value;
}

bool DiagramItem::isMarried() const
{
    return m_spouse != nullptr;
}
//! [4]

//! [5]
void DiagramItem::contextMenuEvent(QGraphicsSceneContextMenuEvent *event)
{
    scene()->clearSelection();
    setSelected(true);
    myContextMenu->exec(event->screenPos());
}
//! [5]


QVariant DiagramItem::itemChange(GraphicsItemChange change, const QVariant &value)
{
    if (change == QGraphicsItem::ItemPositionChange) {
        foreach (Arrow *arrow, arrows) {
            arrow->updatePosition();
        }
    }
    else if (change == QGraphicsItem::ItemPositionHasChanged) {
        updateSpousePosition();
    }

    return value;
}

void DiagramItem::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event)
{
    Q_UNUSED(event);

    //if (m_textItem->isUnderMouse()) {
        m_textItem->startEditing();
        //}
}

void DiagramItem::updateSpousePosition()
{
    if (m_spouse)
    {
        if (m_spousePosition == SpouseToLeft)
        {
            // TODO: Handle this while avoiding loop!
            //qDebug() << "Would set spouse (left) position.";
        }
        else
        {
            // Spouse is to right.
            auto spouseX = x() + boundingRect().width();
            auto spouseY = y();
            m_spouse->setPos(spouseX, spouseY);
        }
    }
}
