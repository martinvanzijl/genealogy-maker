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

#ifndef DIAGRAMITEM_H
#define DIAGRAMITEM_H

#include <QGraphicsPixmapItem>
#include <QList>
#include <QUuid>
#include <QDate>

QT_BEGIN_NAMESPACE
class QPixmap;
class QGraphicsItem;
class QGraphicsScene;
class QTextEdit;
class QGraphicsSceneMouseEvent;
class QMenu;
class QGraphicsSceneContextMenuEvent;
class QPainter;
class QStyleOptionGraphicsItem;
class QWidget;
class QPolygonF;
QT_END_NAMESPACE

class Arrow;
class DiagramTextItem;
class MarriageItem;

class DiagramItem : public QGraphicsPolygonItem
{
public:
    enum { Type = UserType + 15 };
    enum DiagramType { Person };
    enum SpousePosition { SpouseToLeft, SpouseToRight };

    DiagramItem(DiagramType diagramType, QMenu *contextMenu, QGraphicsItem *parent = 0);

    void removeArrow(Arrow *arrow);
    void removeArrows();
    DiagramType diagramType() const { return myDiagramType; }
    QPolygonF polygon() const { return myPolygon; }
    void addArrow(Arrow *arrow);
    QPixmap image() const;
    int type() const override { return Type;}
    QList<Arrow *> getArrows() const;
    QString name() const;
    void setName(QString value);
    void setHighlighted(bool value);
    void marryTo(DiagramItem *spouse);
    bool canMarry(DiagramItem *potentialSpouse) const;
    QUuid id() const;
    void setId(const QUuid& value);
    DiagramTextItem *textItem();
    void editName();
    QString bio() const;
    void setBio(const QString& value);
    QStringList photos() const;
    void setPhotos(const QStringList& value);
    bool isMarried() const;
    void removeMarriage();

    QDate getDateOfBirth() const;
    void setDateOfBirth(const QDate &dateOfBirth);

    QDate getDateOfDeath() const;
    void setDateOfDeath(const QDate &dateOfDeath);

    QString getPlaceOfBirth() const;
    void setPlaceOfBirth(const QString &placeOfBirth);

    QString getPlaceOfDeath() const;
    void setPlaceOfDeath(const QString &placeOfDeath);

    MarriageItem *getMarriageItem() const;
    QPointF getMarriageItemPos() const;

    void fitToText();

    static DiagramItem *getDoubleClickedItem();
    static void resetDoubleClickedItem();

    static QDate defaultDateOfBirth();
    static QDate defaultDateOfDeath();

    void setShowThumbnail(bool value);
    static void setShowThumbnailByDefault(bool value);

    QString getCountryOfBirth() const;
    void setCountryOfBirth(const QString &countryOfBirth);

    bool hasParent() const;
    QList<DiagramItem *> getParents() const;
    QList<DiagramItem *> getChildren() const;

    QString getFirstName() const;
    void setFirstName(const QString &firstName);

    QString getLastName() const;
    void setLastName(const QString &lastName);

    /// Callback for after the diagram text is edited by the user.
    void onTextEdited();

    QString getGender() const;
    void setGender(const QString &gender);
    bool isGenderKnown() const;

    void onShapeChanged();
    void setMarriageItemPosition();

    DiagramItem *getSpouse() const;

    SpousePosition getSpousePosition() const;

    int getWidthIncludingSpouse() const;

    QColor getTextColor() const;
    void setTextColor(const QColor &color);

    void selectDescendants();

    QColor getBorderColor() const;
    void setBorderColor(const QColor &color);

protected:
    void contextMenuEvent(QGraphicsSceneContextMenuEvent *event) override;
    QVariant itemChange(GraphicsItemChange change, const QVariant &value) override;
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event) override;
//    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;

private:
    void updateArrowPositions();
    void updateSpousePosition();
    void updateThumbnail();

    DiagramType myDiagramType;
    QPolygonF myPolygon;
    QMenu *myContextMenu;
    QList<Arrow *> arrows;
    DiagramTextItem *m_textItem;
    QUuid m_id;
    QString m_bio;
    QStringList m_photos;
    QDate m_dateOfBirth;
    QDate m_dateOfDeath;
    QString m_placeOfBirth;
    QString m_countryOfBirth;
    QString m_placeOfDeath;
    DiagramItem *m_spouse;

    SpousePosition m_spousePosition;
    bool m_movedBySpouse;
    MarriageItem *m_marriageItem;

    QGraphicsPixmapItem *m_thumbnail;

    static DiagramItem *m_doubleClickedItem;

    bool m_showThumbnail;
    QString m_firstName;
    QString m_lastName;
    QString m_gender;

    QColor m_borderColor;
};

#endif // DIAGRAMITEM_H
