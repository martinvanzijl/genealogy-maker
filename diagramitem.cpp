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
#include <QStyleOptionGraphicsItem>
#include <QImageReader>

DiagramItem *DiagramItem::m_doubleClickedItem = nullptr;
static bool m_showThumbnailByDefault = false;

static int DEFAULT_WIDTH = 200;

DiagramItem::DiagramItem(DiagramType diagramType, QMenu *contextMenu,
             QGraphicsItem *parent)
    : QGraphicsPolygonItem(parent),
      m_spouse(nullptr),
      m_movedBySpouse(false),
      m_marriageItem(nullptr),
      m_thumbnail(nullptr)
{
    myDiagramType = diagramType;
    myContextMenu = contextMenu;

    // Create the polygon.
    switch (myDiagramType) {

    case Person:
    {
        int height = 25;
        int width = DEFAULT_WIDTH / 2;
        myPolygon << QPointF(-width, -height) << QPointF(width, -height)
                  << QPointF(width, height) << QPointF(-width, height)
                  << QPointF(-width, -height);
        break;
    }
    default:
        // This should not happen.
        qDebug() << "Unknown diagram item type:" << myDiagramType;
        break;
    }

    setPolygon(myPolygon);
    setFlag(QGraphicsItem::ItemIsMovable, true);
    setFlag(QGraphicsItem::ItemIsSelectable, true);
    setFlag(QGraphicsItem::ItemSendsGeometryChanges, true);

    // Add child text.
    if (myDiagramType == Person) {
        m_textItem = new DiagramTextItem(this);
        m_textItem->setPlainText("New Person");
        fitToText();

        // Set default name fields.
        m_firstName = "New";
        m_lastName = "Person";
    }
    else {
        m_textItem = nullptr;
    }

    // Set default dates.
    m_dateOfBirth = defaultDateOfBirth();
    m_dateOfDeath = defaultDateOfDeath();

    // Set flag.
    m_showThumbnail = m_showThumbnailByDefault;
}

void DiagramItem::removeArrow(Arrow *arrow)
{
    int index = arrows.indexOf(arrow);

    if (index != -1)
        arrows.removeAt(index);
}

void DiagramItem::removeArrows()
{
    foreach (Arrow *arrow, arrows) {
        arrow->startItem()->removeArrow(arrow);
        arrow->endItem()->removeArrow(arrow);
        scene()->removeItem(arrow);
        //delete arrow;
    }
}

void DiagramItem::addArrow(Arrow *arrow)
{
    arrows.append(arrow);
}

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
        fitToText();
        m_textItem->emitChangedSignal();
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

    // Add "wedding ring".
    auto ring = new MarriageItem(this);
    m_marriageItem = ring;
    setMarriageItemPosition();
    ring->setPersonLeft(this);
    ring->setPersonRight(spouse);
    m_spouse->m_marriageItem = ring;

    // Move above spouse so that ring is always visible.
    if (zValue() <= spouse->zValue())
    {
        setZValue(spouse->zValue() + 0.1);
    }

    // Update arrow positions.
    updateArrowPositions();
    m_spouse->updateArrowPositions();
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
    updateThumbnail();
}

bool DiagramItem::isMarried() const
{
    return m_spouse != nullptr;
}

void DiagramItem::removeMarriage()
{
    if (isMarried())
    {
        // Delete "wedding ring".
        if (m_marriageItem)
        {
            scene()->removeItem(m_marriageItem);
            //delete m_marriageItem; // Let undo stack handle this.
        }
        m_marriageItem = nullptr;
        m_spouse->m_marriageItem = nullptr;

        // Remove spouse connections.
        m_spouse->m_spouse = nullptr;
        m_spouse = nullptr;
    }
}

void DiagramItem::contextMenuEvent(QGraphicsSceneContextMenuEvent *event)
{
    scene()->clearSelection();
    setSelected(true);
    myContextMenu->exec(event->screenPos());
}

QVariant DiagramItem::itemChange(GraphicsItemChange change, const QVariant &value)
{
    if (change == QGraphicsItem::ItemPositionChange) {
        updateArrowPositions();
    }
    else if (change == QGraphicsItem::ItemPositionHasChanged) {
        updateSpousePosition();
        m_movedBySpouse = false;
    }

    return value;
}

void DiagramItem::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event)
{
    Q_UNUSED(event);

//    if (m_textItem->isUnderMouse()) {
//        m_textItem->startEditing();
//    }

    m_doubleClickedItem = this;
}

void DiagramItem::updateArrowPositions()
{
    foreach (Arrow *arrow, arrows) {
        arrow->updatePosition();
    }
}

//void DiagramItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
//{
//    // Highlight in green if selected.
//    if (isSelected()) {
//        QPen pen(Qt::darkGreen, 3);
//        painter->setPen(pen);
//        painter->drawRect(boundingRect().adjusted(-5, -5, 5, 5));
//    }

//    // Prevent the default "selection indicator".
//    QStyleOptionGraphicsItem myoption = (*option);
//    myoption.state &= !QStyle::State_Selected;
//    QGraphicsPolygonItem::paint(painter, &myoption, widget);
//}

void DiagramItem::updateSpousePosition()
{
    if (m_spouse && !m_movedBySpouse)
    {
        m_spouse->m_movedBySpouse = true; // Avoid infinite loop.

        int offset = (boundingRect().width() + m_spouse->boundingRect().width()) / 2;

        if (m_spousePosition == SpouseToLeft)
        {
            // Spouse is to left.
            auto spouseX = x() - offset;
            auto spouseY = y();
            m_spouse->setPos(spouseX, spouseY);
        }
        else
        {
            // Spouse is to right.
            auto spouseX = x() + offset;
            auto spouseY = y();
            m_spouse->setPos(spouseX, spouseY);
        }
    }
}

void DiagramItem::updateThumbnail()
{
    // Exit if there are no photos.
    if (m_photos.isEmpty()) {

        // Remove thumbnail if it exists.
        if (m_thumbnail) {
            delete m_thumbnail;
            m_thumbnail = nullptr;
        }

        // Exit.
        return;
    }

    // Get the first photo.
    QString fileName = m_photos.first();

    // Create image reader.
    QImageReader reader(fileName);
    reader.setAutoTransform(true);

    // Read image.
    const QImage image = reader.read();
    if (image.isNull()) {
        qDebug() << "Could not read thumbnail image:" << fileName;
        return;
    }

    // Scale image to thumbnail size.
    QPixmap pixmap = QPixmap::fromImage(image).scaled(32, 32);

    // Create thumbnail item if required
    if (!m_thumbnail) {
        m_thumbnail = new QGraphicsPixmapItem(this);
    }

    // Set the picture.
    m_thumbnail->setPixmap(pixmap);

    // Set the position.
    m_thumbnail->setX(boundingRect().left() + 8);
    m_thumbnail->setY(boundingRect().center().y() - m_thumbnail->boundingRect().height() / 2);

    // Set visibility.
    m_thumbnail->setVisible(m_showThumbnail);
}

DiagramItem::SpousePosition DiagramItem::getSpousePosition() const
{
    return m_spousePosition;
}

/**
 * @brief DiagramItem::getWidthIncludingSpouse
 * @return The width of the person, including the spouse if married.
 */
int DiagramItem::getWidthIncludingSpouse() const
{
    // Get self width.
    int width = boundingRect().width();

    // Get spouse width.
    if (isMarried()) {
        width += m_spouse->boundingRect().width();
    }

    // Return.
    return width;
}

DiagramItem *DiagramItem::getSpouse() const
{
    return m_spouse;
}

QString DiagramItem::getGender() const
{
    return m_gender;
}

void DiagramItem::setGender(const QString &gender)
{
    m_gender = gender;
}

bool DiagramItem::isGenderKnown() const
{
    return !m_gender.isEmpty();
}

void DiagramItem::onShapeChanged()
{
    // Move "wedding ring".
    if (m_marriageItem && m_marriageItem->parentItem() == this) {
        setMarriageItemPosition();
    }

    // Move spouse.
    m_movedBySpouse = false;
    updateSpousePosition();

    // Update arrow positions.
    updateArrowPositions();
}

void DiagramItem::setMarriageItemPosition()
{
    if (!m_marriageItem) {
        qDebug() << "Marriage item is null. Cannot set position.";
        return;
    }

    m_marriageItem->setX(boundingRect().width() / 2.0 - m_marriageItem->boundingRect().width() / 2.0);
    m_marriageItem->setY(-m_marriageItem->boundingRect().height() / 2.0);
}

QString DiagramItem::getLastName() const
{
    return m_lastName;
}

void DiagramItem::setLastName(const QString &lastName)
{
    m_lastName = lastName;
}

void DiagramItem::onTextEdited()
{
    // Make sure the text fits in the box.
    fitToText();

    // Extract first and last name.
    QString fullName = m_textItem->text();
    int firstSpacePos = fullName.indexOf(" ");

    if (firstSpacePos == -1) {
        // No spaces. Use full name as first name.
        m_firstName = fullName;
    }
    else {
        // There is a space. Split the parts.
        m_firstName = fullName.mid(0, firstSpacePos);
        m_lastName = fullName.mid(firstSpacePos + 1);
    }
}

QString DiagramItem::getFirstName() const
{
    return m_firstName;
}

void DiagramItem::setFirstName(const QString &firstName)
{
    m_firstName = firstName;
}

QString DiagramItem::getCountryOfBirth() const
{
    return m_countryOfBirth;
}

void DiagramItem::setCountryOfBirth(const QString &countryOfBirth)
{
    m_countryOfBirth = countryOfBirth;
}

bool DiagramItem::hasParent() const
{
    for (auto arrow : arrows) {
        if (arrow->endItem() == this) {
            return true;
        }
    }

    return false;
}

QList<DiagramItem *> DiagramItem::getParents() const
{
    QList<DiagramItem *> parents;

    for (auto arrow : arrows) {
        if (arrow->endItem() == this) {
            parents << arrow->startItem();
        }
    }

    return parents;
}

QList<DiagramItem *> DiagramItem::getChildren() const
{
    QList<DiagramItem *> children;

    for (auto arrow : arrows) {
        if (arrow->startItem() == this) {
            children << arrow->endItem();
        }
    }

    return children;
}

void DiagramItem::fitToText()
{
    // Calculate minimum size.
    int margin = 16;
    int minWidth = m_textItem->boundingRect().width() + (margin * 2);

    // Calculate best size.
    int bestWidth = qMax(DEFAULT_WIDTH, minWidth);

    // Expand box if required.
    if (boundingRect().width() != bestWidth) {
        int height = 25;
        int width = bestWidth / 2;

        myPolygon.clear();
        myPolygon << QPointF(-width, -height) << QPointF(width, -height)
                  << QPointF(width, height) << QPointF(-width, height)
                  << QPointF(-width, -height);

        setPolygon(myPolygon);

        onShapeChanged();
    }

    // Center the text.
    m_textItem->setX(boundingRect().center().x() - m_textItem->boundingRect().width() / 2);
    m_textItem->setY(boundingRect().center().y() - m_textItem->boundingRect().height() / 2);
}

MarriageItem *DiagramItem::getMarriageItem() const
{
    return m_marriageItem;
}

QPointF DiagramItem::getMarriageItemPos() const
{
    int offset = boundingRect().width() / 2.0;
    int resultX = 0;
    int resultY = y() + boundingRect().height() / 2.0;

    if (m_spousePosition == SpouseToLeft)
    {
        // Marriage item is to left.
        resultX = x() - offset;
    }
    else
    {
        // Marriage item is to right.
        resultX = x() + offset;
    }

    return QPointF(resultX, resultY);
}

DiagramItem *DiagramItem::getDoubleClickedItem()
{
    return m_doubleClickedItem;
}

void DiagramItem::resetDoubleClickedItem()
{
    m_doubleClickedItem = nullptr;
}

QDate DiagramItem::defaultDateOfBirth()
{
    return QDate(1900, 1, 1);
}

QDate DiagramItem::defaultDateOfDeath()
{
    return QDate(7999, 12, 31);
}

void DiagramItem::setShowThumbnail(bool value)
{
    m_showThumbnail = value;

    if (m_thumbnail) {
        m_thumbnail->setVisible(m_showThumbnail);
    }
}

void DiagramItem::setShowThumbnailByDefault(bool value)
{
    m_showThumbnailByDefault = value;
}

QString DiagramItem::getPlaceOfDeath() const
{
    return m_placeOfDeath;
}

void DiagramItem::setPlaceOfDeath(const QString &placeOfDeath)
{
    m_placeOfDeath = placeOfDeath;
}

QString DiagramItem::getPlaceOfBirth() const
{
    return m_placeOfBirth;
}

void DiagramItem::setPlaceOfBirth(const QString &placeOfBirth)
{
    m_placeOfBirth = placeOfBirth;
}

QDate DiagramItem::getDateOfDeath() const
{
    return m_dateOfDeath;
}

void DiagramItem::setDateOfDeath(const QDate &dateOfDeath)
{
    m_dateOfDeath = dateOfDeath;
}

QDate DiagramItem::getDateOfBirth() const
{
    return m_dateOfBirth;
}

void DiagramItem::setDateOfBirth(const QDate &dateOfBirth)
{
    m_dateOfBirth = dateOfBirth;
}
