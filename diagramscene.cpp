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

#include <QDebug>
#include <QDomDocument>
#include <QTextCursor>
#include <QTextStream>
#include <QGraphicsSceneMouseEvent>
#include <QMessageBox>

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

    QDomElement child = root.firstChildElement("item");
    while (!child.isNull()) {
        parseItemElement(child);
        child = child.nextSiblingElement("item");
    }

    child = root.firstChildElement("relationship");
    while (!child.isNull()) {
        parseArrowElement(child);
        child = child.nextSiblingElement("relationship");
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
//! [5]

//! [6]
void DiagramScene::mousePressEvent(QGraphicsSceneMouseEvent *mouseEvent)
{
    if (mouseEvent->button() != Qt::LeftButton)
        return;

    DiagramItem *item = nullptr;
    switch (myMode) {
    case InsertItem:
    {
        // Create person.
        item = new DiagramItem(myItemType, myItemMenu);
        item->setBrush(myItemColor);
        addItem(item);
        item->setPos(mouseEvent->scenePos());

        // Assign ID.
        auto id = QUuid::createUuid();
        item->setId(id);
        m_itemsDict[id] = item;
        emit itemInserted(item);

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
        if (draggedItem && draggedItem->type() == DiagramItem::Type) {
            auto pos = mouseEvent->scenePos();
            QList<QGraphicsItem *> list = items(pos);

            bool found = false;
            for (auto item: list) {
                if (item != draggedItem) {
                    if (item->type() == DiagramItem::Type) {
                        found = true;
                        highlight(qgraphicsitem_cast<DiagramItem *>(item));
                        break;
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

//! [14]
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
    item->setBrush(myItemColor);
    addItem(item);
    auto x = element.attribute("x").toDouble();
    auto y = element.attribute("y").toDouble();
    auto name = element.attribute("name");
    auto id = QUuid(element.attribute("id"));
    item->setPos(x, y);
    item->setName(name);
    item->setId(id);
    //emit itemInserted(item);

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

void DiagramScene::marry(DiagramItem *item1, DiagramItem *item2)
{
    item1->marryTo(item2);
}
//! [14]
