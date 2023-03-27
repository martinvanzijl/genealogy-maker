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

#include "diagramtextitem.h"
#include "diagramscene.h"
#include "diagramitem.h"

#include <QTextCursor>
#include <QKeyEvent>

static QFont m_defaultFont;

DiagramTextItem::DiagramTextItem(DiagramItem *parent)
    : QGraphicsTextItem(parent)
{
    //setFlag(QGraphicsItem::ItemIsMovable);
    //setFlag(QGraphicsItem::ItemIsSelectable);

    m_person = parent;

    // Apply default font if valid.
    if(!m_defaultFont.family().isEmpty() && m_defaultFont.pointSize() >= 1) {
        setFont(m_defaultFont);
    }
}

QString DiagramTextItem::text() const
{
    return toPlainText();
}

void DiagramTextItem::startEditing()
{
    if (textInteractionFlags() == Qt::NoTextInteraction) {
        setTextInteractionFlags(Qt::TextEditorInteraction);
    }
    setFocus();
    auto cursor = textCursor();
    cursor.movePosition(QTextCursor::End);
    cursor.select(QTextCursor::Document);
    setTextCursor(cursor);
}

void DiagramTextItem::emitChangedSignal()
{
    emit textEdited(this);
}

void DiagramTextItem::setDefaultFont(const QFont &font)
{
    m_defaultFont = font;
}

QVariant DiagramTextItem::itemChange(GraphicsItemChange change,
                     const QVariant &value)
{
    if (change == QGraphicsItem::ItemSelectedHasChanged)
        emit selectedChange(this);
    return value;
}

void DiagramTextItem::focusOutEvent(QFocusEvent *event)
{
    // Clear selection.
    auto cursor = textCursor();
    cursor.clearSelection();
    setTextCursor(cursor);

    // Lose focus.
    setTextInteractionFlags(Qt::NoTextInteraction);
    emit lostFocus(this);
    QGraphicsTextItem::focusOutEvent(event);

    // Update person.
    if (m_person) {
        m_person->onTextEdited();
    }

    // Send signal.
    emit textEdited(this);
}

void DiagramTextItem::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event)
{
    if (textInteractionFlags() == Qt::NoTextInteraction) {
        setTextInteractionFlags(Qt::TextEditorInteraction);
    }
    QGraphicsTextItem::mouseDoubleClickEvent(event);
    setFocus();
}

void DiagramTextItem::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Return || event->key() == Qt::Key_Escape) {
        // Stop editing.
        clearFocus();
    }
    else {
        QGraphicsTextItem::keyPressEvent(event);

        // Align to center of parent if currently editing.
        if (textInteractionFlags() == Qt::TextEditorInteraction) {
            auto parentRect = parentItem();
            if (parentRect) {
                setPos(parentRect->boundingRect().center().x() - boundingRect().width() / 2, y());
            }
        }
    }
}
