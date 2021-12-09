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

    enum SpousePosition { SpouseToLeft, SpouseToRight };

    SpousePosition m_spousePosition;
    bool m_movedBySpouse;
    MarriageItem *m_marriageItem;

    QGraphicsPixmapItem *m_thumbnail;

    static DiagramItem *m_doubleClickedItem;

    bool m_showThumbnail;
    QString m_firstName;
    QString m_lastName;
    QString m_gender;
};

#endif // DIAGRAMITEM_H
