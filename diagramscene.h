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

#ifndef DIAGRAMSCENE_H
#define DIAGRAMSCENE_H

#include "diagramitem.h"
#include "diagramtextitem.h"

#include <QDir>
#include <QGraphicsScene>

QT_BEGIN_NAMESPACE
class QGraphicsSceneMouseEvent;
class QMenu;
class QPointF;
class QGraphicsLineItem;
class QFont;
class QGraphicsTextItem;
class QColor;
class QDomElement;
class QTimer;
QT_END_NAMESPACE

//! [0]
class DiagramScene : public QGraphicsScene
{
    Q_OBJECT

public:
    enum Mode { InsertItem, InsertLine, InsertText, MoveItem };

    explicit DiagramScene(QMenu *itemMenu, QObject *parent = 0);
    QFont font() const { return myFont; }
    QColor textColor() const { return myTextColor; }
    QColor itemColor() const { return myItemColor; }
    QColor lineColor() const { return myLineColor; }
    void setLineColor(const QColor &color);
    void setTextColor(const QColor &color);
    void setItemColor(const QColor &color);
    void setFont(const QFont &font);
    bool open(QIODevice *device, const QString &photosFolderPath);
    void print();
    void save(QIODevice *device, const QString &photosFolderPath);
    DiagramItem *itemWithId(const QUuid &id);
    bool isEmpty() const;
    QGraphicsItem *firstItem() const;
    void selectAll();
    void addPersonFromUndo(DiagramItem *item);
    void removePersonFromUndo(DiagramItem *item);
    void marry(DiagramItem *item1, DiagramItem *item2, bool fromUndo = false);
    void removeMarriage(DiagramItem *person1, DiagramItem *person2);
    bool isDrawingArrow() const;
    void loadPreferences();
    void autoLayout();
    int autoLayoutRow(const QList<DiagramItem *> &items, int startY);
    void highlightForSearch(DiagramItem *item);
    int personCount() const;
    int relationshipCount() const;

    QWidget *window() const;
    void setWindow(QWidget *window);

public slots:
    void setMode(Mode mode);
    void setItemType(DiagramItem::DiagramType type);
    void editorLostFocus(DiagramTextItem *item);

//private slots:
//    void onSelectionChanged();

signals:
    void itemInserted(DiagramItem *item, bool fromLoad);
    void itemRemoved(DiagramItem *item);
    void textInserted(QGraphicsTextItem *item);
    void itemSelected(QGraphicsItem *item);
    void mouseReleased();
    void arrowAdded(Arrow *arrow);
    void itemsAboutToMove();
    void itemsFinishedMoving();
    void peopleMarried(DiagramItem *person1, DiagramItem *person2);
    void cleared();
    void personDoubleClicked(DiagramItem *person);

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *mouseEvent) override;
    void mouseMoveEvent(QGraphicsSceneMouseEvent *mouseEvent) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *mouseEvent) override;
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event) override;

    void dragEnterEvent(QGraphicsSceneDragDropEvent *event) override;
    void dragMoveEvent(QGraphicsSceneDragDropEvent *event) override;
    void dropEvent(QGraphicsSceneDragDropEvent *event) override;

private:
    bool isItemChange(int type);
    void parseItemElement(const QDomElement& element, const QString &photosFolderPath);
    void parseArrowElement(const QDomElement& element);
    void parseMarriageElement(const QDomElement& element);
    void highlight(DiagramItem *item);
    void unHighlightAll();

private slots:
    void removeSearchHighlight();

private:
    DiagramItem::DiagramType myItemType;
    QMenu *myItemMenu;
    Mode myMode;
    bool leftButtonDown;
    QPointF startPoint;
    QGraphicsLineItem *line;
    QFont myFont;
    DiagramTextItem *textItem;
    QColor myTextColor;
    QColor myItemColor;
    QColor myLineColor;

    QMap<QUuid, DiagramItem *> m_itemsDict;
    QMap<QString, DiagramItem *> m_pointerDict;
    DiagramItem *m_highlightedItem;
    long m_nextId;
    bool m_busyMoving;
    QTimer *m_searchHighlightTimer;
    QWidget *m_window;
    DiagramItem *createPerson(const QPointF &pos);

    /**
     * @brief copyPhotosForPerson Copy the photos for the person to a photo directory.
     * @param diagramItem The person.
     * @param photosDir The project photos directory. The person will have a subdirectory created here, if required.
     */
    void copyPhotosForPerson(DiagramItem *diagramItem, const QDir &photosDir);
};
//! [0]

#endif // DIAGRAMSCENE_H
