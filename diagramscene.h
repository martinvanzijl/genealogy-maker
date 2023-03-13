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
    int marriageCount() const;
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
