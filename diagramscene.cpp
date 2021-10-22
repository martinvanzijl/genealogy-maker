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

#include "diagramscene.h"
#include "arrow.h"
#include "marriageitem.h"
#include "undo/changefillcolorundo.h"
#include "undo/undomanager.h"

#include <QDebug>
#include <QDomDocument>
#include <QTextCursor>
#include <QTextStream>
#include <QGraphicsSceneMouseEvent>
#include <QMessageBox>
#include <QGraphicsSceneDragDropEvent>
#include <QMimeData>
#include <QSettings>

///
/// \brief parseXmlDate Parse the string from an XML file into a date.
/// \param string The attribute value.
/// \return The date object.
///
static QDate parseXmlDate(const QString &string)
{
    // Try parsing as a normal date.
    QDate date = QDate::fromString(string);

    if (!date.isValid())
    {
        // Try parsing as a GEDCOM date.
        date = QDate::fromString(string, "d MMM yyyy");
    }

    if (!date.isValid())
    {
        qDebug() << "Invalid date of birth:" << string;
    }

    return date;
}

//! [0]
DiagramScene::DiagramScene(QMenu *itemMenu, QObject *parent)
    : QGraphicsScene(parent)
{
    myItemMenu = itemMenu;
    myMode = MoveItem;
    myItemType = DiagramItem::Step;
    line = 0;
    textItem = 0;
    myItemColor = Qt::white;
    myTextColor = Qt::black;
    myLineColor = Qt::black;
    m_highlightedItem = nullptr;
    m_nextId = 1;
}
//! [0]

//! [1]
void DiagramScene::setLineColor(const QColor &color)
{
    myLineColor = color;
    if (isItemChange(Arrow::Type)) {
        Arrow *item = qgraphicsitem_cast<Arrow *>(selectedItems().first());
        item->setColor(myLineColor);
        update();
    }
}
//! [1]

//! [2]
void DiagramScene::setTextColor(const QColor &color)
{
    myTextColor = color;
    if (isItemChange(DiagramTextItem::Type)) {
        DiagramTextItem *item = qgraphicsitem_cast<DiagramTextItem *>(selectedItems().first());
        item->setDefaultTextColor(myTextColor);
    }
}
//! [2]

//! [3]
void DiagramScene::setItemColor(const QColor &color)
{
    myItemColor = color;
    if (isItemChange(DiagramItem::Type)) {
        DiagramItem *item = qgraphicsitem_cast<DiagramItem *>(selectedItems().first());
        UndoManager::add(new ChangeFillColorUndo(item, color));
        item->setBrush(myItemColor);
    }
}
//! [3]

//! [4]
void DiagramScene::setFont(const QFont &font)
{
    myFont = font;

    if (isItemChange(DiagramTextItem::Type)) {
        QGraphicsTextItem *item = qgraphicsitem_cast<DiagramTextItem *>(selectedItems().first());
        //At this point the selection can change so the first selected item might not be a DiagramTextItem
        if (item)
            item->setFont(myFont);
    }
}

void DiagramScene::open(QIODevice *device)
{
    QString errorStr;
    int errorLine;
    int errorColumn;

    QDomDocument domDocument;
    if (!domDocument.setContent(device, true, &errorStr, &errorLine,
                                &errorColumn)) {
//        QMessageBox::information(window(), tr("DOM Bookmarks"),
//                                 tr("Parse error at line %1, column %2:\n%3")
//                                 .arg(errorLine)
//                                 .arg(errorColumn)
//                                 .arg(errorStr));
//        return false;
    }

    QDomElement root = domDocument.documentElement();
    if (root.tagName() != "genealogy") {
//        QMessageBox::information(window(), tr("DOM Bookmarks"),
//                                 tr("The file is not an genealogy file."));
        return;
    }

    clear();
    emit cleared();

    // Load diagram size.
    int diagramWidth = root.attribute("height", "5000").toInt();
    int diagramHeight = root.attribute("width", "5000").toInt();
    setSceneRect(0, 0, diagramWidth, diagramHeight);

    // Load persons.
    QDomElement child = root.firstChildElement("item");
    while (!child.isNull()) {
        parseItemElement(child);
        child = child.nextSiblingElement("item");
    }

    // Load relationships.
    child = root.firstChildElement("relationship");
    while (!child.isNull()) {
        parseArrowElement(child);
        child = child.nextSiblingElement("relationship");
    }

    // Load marriages.
    child = root.firstChildElement("marriage");
    while (!child.isNull()) {
        parseMarriageElement(child);
        child = child.nextSiblingElement("marriage");
    }
}

void DiagramScene::print()
{
    qDebug() << "Total items:" << items().count();
    for (auto item: items()) {
        qDebug() << "   " << item;
    }
}

void DiagramScene::save(QIODevice *device)
{
    const int indentSize = 4;

    QTextStream out(device);
    QDomDocument domDocument;
    QDomElement rootElement = domDocument.createElement("genealogy");
    QList<Arrow *> arrows;

    //
    // Save diagram information.
    //
    auto diagramRect = sceneRect();
    rootElement.setAttribute("width", diagramRect.width());
    rootElement.setAttribute("height", diagramRect.height());

    //
    // Save persons.
    //
    for (auto item: items()) {
        DiagramItem *diagramItem = dynamic_cast<DiagramItem*> (item);

        if (diagramItem) {
            QDomElement itemElement = domDocument.createElement("item");
            itemElement.setAttribute("x", item->pos().x());
            itemElement.setAttribute("y", item->pos().y());
            itemElement.setAttribute("name", diagramItem->name());
            itemElement.setAttribute("id", diagramItem->id().toString());
            itemElement.setAttribute("bio", diagramItem->bio());
//            if (diagramItem->isDateOfBirthKnown()) {
                itemElement.setAttribute("date_of_birth", diagramItem->getDateOfBirth().toString());
//            }
            itemElement.setAttribute("place_of_birth", diagramItem->getPlaceOfBirth());
//            if (diagramItem->isDateOfDeathKnown()) {
                itemElement.setAttribute("date_of_death", diagramItem->getDateOfDeath().toString());
//            }
            itemElement.setAttribute("place_of_death", diagramItem->getPlaceOfDeath());
            itemElement.setAttribute("fill_color", diagramItem->brush().color().name());

            for (auto photo: diagramItem->photos()) {
                QDomElement photoElement = domDocument.createElement("photo");
                photoElement.setAttribute("path", photo);
                itemElement.appendChild(photoElement);
            }

            rootElement.appendChild(itemElement);

            arrows << diagramItem->getArrows();
        }
    }

    //
    // Save relationships.
    //
    auto arrowSet = QSet<Arrow *>::fromList(arrows);
    for (auto arrow: arrowSet) {
        QDomElement element = domDocument.createElement("relationship");
        element.setAttribute("from", arrow->startItem()->id().toString());
        element.setAttribute("to", arrow->endItem()->id().toString());
        rootElement.appendChild(element);
    }

    //
    // Save marriages.
    //
    for (auto item: items()) {
        if (item->type() == MarriageItem::Type) {
            MarriageItem *marriage = dynamic_cast<MarriageItem*> (item);
            QDomElement element = domDocument.createElement("marriage");
            element.setAttribute("x", marriage->pos().x());
            element.setAttribute("y", marriage->pos().y());
            element.setAttribute("person_left", marriage->personLeft()->id().toString());
            element.setAttribute("person_right", marriage->personRight()->id().toString());
//            if (marriage->isDateKnown()) {
                element.setAttribute("date", marriage->getDate().toString());
//            }
            element.setAttribute("place", marriage->getPlace());

            rootElement.appendChild(element);
        }
    }

    domDocument.appendChild(rootElement);
    domDocument.save(out, indentSize);
}

DiagramItem *DiagramScene::itemWithId(const QUuid& id)
{
    return m_itemsDict[id];
}

bool DiagramScene::isEmpty() const
{
    return items().isEmpty();
}

QGraphicsItem *DiagramScene::firstItem() const
{
    return items().first();
}

void DiagramScene::selectAll()
{
    for (auto item: items()) {
        item->setSelected(true);
    }
}

void DiagramScene::addPersonFromUndo(DiagramItem *item)
{
    addItem(item);
    m_itemsDict[item->id()] = item;
    emit itemInserted(item, false);
}

void DiagramScene::removePersonFromUndo(DiagramItem *item)
{
    removeItem(item);
    m_itemsDict.remove(item->id());
    emit itemRemoved(item);
}

void DiagramScene::removeMarriage(DiagramItem *person1, DiagramItem *person2)
{
    Q_UNUSED(person2);

    person1->removeMarriage();
}

bool DiagramScene::isDrawingArrow() const
{
    return myMode == InsertLine;
}

void DiagramScene::loadPreferences()
{
    QSettings settings;

    int arrowLineWidth = settings.value("diagram/arrowLineWidth", 2).toInt();
    for (auto item: items()) {
        if (item->type() == Arrow::Type) {
            Arrow *arrow = qgraphicsitem_cast<Arrow*> (item);
            arrow->setLineWidth(arrowLineWidth);
        }
    }

    Arrow::setDefaultLineWidth(arrowLineWidth);
}

void DiagramScene::autoLayout()
{
    // Place the persons.
    double startX = 128;
    double startY = 64;

    double x = startX;
    double y = startY;

    double horizontalSpacing = 16;
    double verticalSpacing = 16;

    for (auto item: items()) {
        if (item->type() == DiagramItem::Type) {
            // Place the person.
            DiagramItem *person = qgraphicsitem_cast<DiagramItem*> (item);
            person->setPos(x, y);

            // Prepare the next position.
            x += person->boundingRect().width() + horizontalSpacing;

            // Wrap to next line if required.
            if (x > sceneRect().right()) {
                y += person->boundingRect().height() + verticalSpacing;
                x = startX;
            }
        }
    }
}

//! [4]

void DiagramScene::setMode(Mode mode)
{
    myMode = mode;
}

void DiagramScene::setItemType(DiagramItem::DiagramType type)
{
    myItemType = type;
}

//! [5]
void DiagramScene::editorLostFocus(DiagramTextItem *item)
{
    QTextCursor cursor = item->textCursor();
    cursor.clearSelection();
    item->setTextCursor(cursor);

    if (item->toPlainText().isEmpty()) {
        removeItem(item);
        item->deleteLater();
    }
}

//void DiagramScene::onSelectionChanged()
//{
//    // Highlight selected items.
//    for (auto item: selectedItems()) {
//        if (item->type() == DiagramItem::Type) {
//            auto diagramItem = qgraphicsitem_cast<DiagramItem *> (item);
//            diagramItem->setHighlighted(true);
//        }
//    }
//}
//! [5]

//! [6]
DiagramItem *DiagramScene::createPerson(const QPointF &pos)
{
    // Create the item.
    DiagramItem *item = new DiagramItem(myItemType, myItemMenu);
//        item->setBrush(myItemColor);
    item->setBrush(Qt::white);
    addItem(item);
    item->setPos(pos);

    // Assign ID.
    auto id = QUuid::createUuid();
    item->setId(id);
    m_itemsDict[id] = item;
    emit itemInserted(item, false);

    // Return the item.
    return item;
}

void DiagramScene::mousePressEvent(QGraphicsSceneMouseEvent *mouseEvent)
{
    if (mouseEvent->button() != Qt::LeftButton)
        return;

    DiagramItem *item = nullptr;
    switch (myMode) {
    case InsertItem:
    {
        // Create person.
        item = createPerson(mouseEvent->scenePos());

        break;
    }

        case InsertLine:
            line = new QGraphicsLineItem(QLineF(mouseEvent->scenePos(),
                                        mouseEvent->scenePos()));
            line->setPen(QPen(myLineColor, 2));
            addItem(line);
            break;

//        case InsertText:
//            textItem = new DiagramTextItem();
//            textItem->setFont(myFont);
//            textItem->setTextInteractionFlags(Qt::TextEditorInteraction);
//            textItem->setZValue(1000.0);
//            connect(textItem, SIGNAL(lostFocus(DiagramTextItem*)),
//                    this, SLOT(editorLostFocus(DiagramTextItem*)));
//            connect(textItem, SIGNAL(selectedChange(QGraphicsItem*)),
//                    this, SIGNAL(itemSelected(QGraphicsItem*)));
//            addItem(textItem);
//            textItem->setDefaultTextColor(myTextColor);
//            textItem->setPos(mouseEvent->scenePos());
//            emit textInserted(textItem);

//    case MoveItem:
//    {
//        auto draggedItem = mouseGrabberItem();
//        if (draggedItem)
//        {
//            emit itemsAboutToMove();
//        }
//        break;
//    }

    default:
        ;
    }
    QGraphicsScene::mousePressEvent(mouseEvent);

    // If we added an item, then immediately edit the name.
    if (item)
    {
        item->editName();
    }
}
//! [9]

//! [10]
void DiagramScene::mouseMoveEvent(QGraphicsSceneMouseEvent *mouseEvent)
{
    if (myMode == InsertLine && line != 0) {
        QLineF newLine(line->line().p1(), mouseEvent->scenePos());
        line->setLine(newLine);
    } else if (myMode == MoveItem) {

        // Handle undo.
        auto draggedItem = mouseGrabberItem();
        if (draggedItem && !m_busyMoving) {
            m_busyMoving = true;
            emit itemsAboutToMove();
        }

        // Normal event.
        QGraphicsScene::mouseMoveEvent(mouseEvent);

        // Check for moving text: move rectangle also.
        if (draggedItem && draggedItem->type() == DiagramTextItem::Type) {
            draggedItem->parentItem()->setSelected(true);
        }

        // Check for marriage.
        bool checkForMarriage = false;
        if (draggedItem && draggedItem->type() == DiagramItem::Type)
        {
            auto draggedDiagramItem = qgraphicsitem_cast<DiagramItem *>(draggedItem);
            if (!draggedDiagramItem->isMarried()) {
                checkForMarriage = true;
            }
        }

        if (checkForMarriage) {
//            auto pos = mouseEvent->scenePos();
//            QList<QGraphicsItem *> list = items(pos);
            QList<QGraphicsItem *> list = draggedItem->collidingItems();

            bool found = false;
            for (auto item: list) {
                if (item != draggedItem) {
                    if (item->type() == DiagramItem::Type) {
                        auto diagramItem = qgraphicsitem_cast<DiagramItem *>(item);
                        if (!diagramItem->isMarried()) {
                            found = true;
                            highlight(diagramItem);
                            break;
                        }
                    }
                }
            }

            // Nothing found, so unlighlight all.
            if (!found) {
                unHighlightAll();
            }
        }
    }
}
//! [10]

void DiagramScene::mouseReleaseEvent(QGraphicsSceneMouseEvent *mouseEvent)
{
    if (line != 0 && myMode == InsertLine) {
        QList<QGraphicsItem *> startItems = items(line->line().p1());
        if (startItems.count() && startItems.first() == line)
            startItems.removeFirst();
        QList<QGraphicsItem *> endItems = items(line->line().p2());
        if (endItems.count() && endItems.first() == line)
            endItems.removeFirst();

        removeItem(line);
        delete line;

        if (startItems.count() > 0 && endItems.count() > 0 &&
            startItems.first() != endItems.first()) {

            // If we release on a text item, use the parent rectangle item instead.
            auto firstItemToCast = startItems.first();
            if (firstItemToCast->type() == DiagramTextItem::Type) {
                firstItemToCast = firstItemToCast->parentItem();
            }
            auto endItemToCast = endItems.first();
            if (endItemToCast->type() == DiagramTextItem::Type) {
                endItemToCast = endItemToCast->parentItem();
            }

            // Now create the arrow.
            if (firstItemToCast->type() == DiagramItem::Type &&
                endItemToCast->type() == DiagramItem::Type)
            {
                DiagramItem *startItem = qgraphicsitem_cast<DiagramItem *>(firstItemToCast);
                DiagramItem *endItem = qgraphicsitem_cast<DiagramItem *>(endItemToCast);
                Arrow *arrow = new Arrow(startItem, endItem);
                arrow->setColor(myLineColor);
                startItem->addArrow(arrow);
                endItem->addArrow(arrow);
                arrow->setZValue(-1000.0);
                addItem(arrow);
                arrow->updatePosition();

                emit arrowAdded(arrow);
            }
        }
    }
    else if (myMode == MoveItem) {
        if (m_busyMoving) {
            m_busyMoving = false;
            emit itemsFinishedMoving();
        }

        if (m_highlightedItem) {
            auto draggedItem = mouseGrabberItem();
            if (draggedItem && draggedItem->type() == DiagramItem::Type) {
                auto item = qgraphicsitem_cast<DiagramItem *>(draggedItem);
                auto name1 = item->name();
                auto name2 = m_highlightedItem->name();

                QMessageBox msgBox;
                msgBox.setWindowTitle("Confirm");
                msgBox.setText(QString("Marry %1 and %2?").arg(name1, name2));
                msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
                msgBox.setDefaultButton(QMessageBox::Yes);
                int ret = msgBox.exec();

                if (ret == QMessageBox::Yes) {
                    marry(item, m_highlightedItem);
                }
            }
        }

        unHighlightAll();
    }

    line = 0;
    QGraphicsScene::mouseReleaseEvent(mouseEvent);

    emit mouseReleased();
}

void DiagramScene::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event)
{
    // Reset double-clicked item.
    DiagramItem::resetDoubleClickedItem();

    // Call parent method.
    QGraphicsScene::mouseDoubleClickEvent(event);

    // Check if item was double-clicked.
    auto item = DiagramItem::getDoubleClickedItem();
    if (item) {
        emit personDoubleClicked(item);
    }
}

void DiagramScene::dragEnterEvent(QGraphicsSceneDragDropEvent *event)
{
    if (event->mimeData()->hasFormat("application/x-dnditemdata")) {
        event->acceptProposedAction();
    } else {
        event->ignore();
    }
}

void DiagramScene::dragMoveEvent(QGraphicsSceneDragDropEvent *event)
{
    if (event->mimeData()->hasFormat("application/x-dnditemdata")) {
        event->acceptProposedAction();
    } else {
        event->ignore();
    }
}

void DiagramScene::dropEvent(QGraphicsSceneDragDropEvent *event)
{
    if (event->mimeData()->hasFormat("application/x-dnditemdata")) {
        QByteArray itemData = event->mimeData()->data("application/x-dnditemdata");
        QDataStream dataStream(&itemData, QIODevice::ReadOnly);
        QString action;
        dataStream >> action;

        event->acceptProposedAction();

        DiagramItem *item = createPerson(event->scenePos());
        item->setSelected(true);
//        item->editName(); // This does not work yet.

        // TODO: Simulate mouse click event on the text.
        // I hope this lets the user start editing the text.
    } else {
        event->ignore();
    }
}

bool DiagramScene::isItemChange(int type)
{
    foreach (QGraphicsItem *item, selectedItems()) {
        if (item->type() == type)
            return true;
    }
    return false;
}

void DiagramScene::parseItemElement(const QDomElement &element)
{
    auto item = new DiagramItem(DiagramItem::Step, myItemMenu);
    item->setBrush(Qt::white);
    addItem(item);
    auto x = element.attribute("x").toDouble();
    auto y = element.attribute("y").toDouble();
    auto name = element.attribute("name");
    auto id = QUuid(element.attribute("id"));
    auto bio = element.attribute("bio");
    auto placeOfBirth = element.attribute("place_of_birth");
    auto placeOfDeath = element.attribute("place_of_death");
    item->setPos(x, y);
    item->setName(name);
    item->setId(id);
    item->setBio(bio);
    item->setPlaceOfBirth(placeOfBirth);
    item->setPlaceOfDeath(placeOfDeath);

    if (element.hasAttribute("date_of_birth")) {
//        item->setDateOfBirth(QDate::fromString(element.attribute("date_of_birth")));
        item->setDateOfBirth(parseXmlDate(element.attribute("date_of_birth")));
    }
    if (element.hasAttribute("date_of_death")) {
        item->setDateOfDeath(QDate::fromString(element.attribute("date_of_death")));
    }

    if (element.hasAttribute("fill_color")) {
        QColor color;
        color.setNamedColor(element.attribute("fill_color"));
        item->setBrush(color);
    }

    QStringList photos;
    QDomElement photoElement = element.firstChildElement("photo");
    while (!photoElement.isNull()) {
        photos << photoElement.attribute("path");
        photoElement = photoElement.nextSiblingElement("photo");
    }
    item->setPhotos(photos);

    emit itemInserted(item, true);
    m_itemsDict[id] = item;
}

void DiagramScene::parseArrowElement(const QDomElement &element)
{
    auto fromId = element.attribute("from");
    auto toId = element.attribute("to");
    auto startItem = m_itemsDict[fromId];
    auto endItem = m_itemsDict[toId];

    if (startItem && endItem) {
        Arrow *arrow = new Arrow(startItem, endItem);
        arrow->setColor(myLineColor);
        startItem->addArrow(arrow);
        endItem->addArrow(arrow);
        arrow->setZValue(-1000.0);
        addItem(arrow);
        arrow->updatePosition();
    }
}

void DiagramScene::parseMarriageElement(const QDomElement &element)
{
    auto leftId = element.attribute("person_left");
    auto rightId = element.attribute("person_right");
    auto personLeft = m_itemsDict[leftId];
    auto personRight = m_itemsDict[rightId];

    if (personLeft && personRight) {
        personLeft->marryTo(personRight);
        MarriageItem *marriage = personLeft->getMarriageItem();

        if (element.hasAttribute("date")) {
            marriage->setDate(QDate::fromString(element.attribute("date")));
        }
        marriage->setPlace(element.attribute("place"));
    }
}

void DiagramScene::highlight(DiagramItem *item)
{
    unHighlightAll();

    m_highlightedItem = item;
    if (m_highlightedItem) {
        m_highlightedItem->setHighlighted(true);
    }
}

void DiagramScene::unHighlightAll()
{
    if (m_highlightedItem) {
        m_highlightedItem->setHighlighted(false);
    }

    m_highlightedItem = nullptr;
}

void DiagramScene::marry(DiagramItem *item1, DiagramItem *item2, bool fromUndo)
{
//    item1->marryTo(item2);

    if (!fromUndo) {
        // Make sure to use "left->marryTo(right)".
        if (item2->x() < item1->x()) {
            item2->marryTo(item1);
            item1->moveBy(-1, 0); // Hack to avoid positioning bug.
            emit peopleMarried(item2, item1);
        }
        else {
            item1->marryTo(item2);
            emit peopleMarried(item1, item2);
        }
    }
    else {
        item1->marryTo(item2);
    }
}
