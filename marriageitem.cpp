#include "marriageitem.h"

#include "diagramitem.h"

#include <QGraphicsScene>
#include <QMenu>
#include <QGraphicsSceneContextMenuEvent>

QMenu *MarriageItem::m_contextMenu = nullptr;
MarriageItem *MarriageItem::m_selectedMarriage = nullptr;

MarriageItem::MarriageItem(QGraphicsItem *parent) :
    QGraphicsEllipseItem(0, 0, 32, 32, parent),
    m_personLeft(nullptr),
    m_personRight(nullptr)
{
    // Set default date.
    m_date = QDate(1900, 1, 1);
}

void MarriageItem::setPersonLeft(DiagramItem *item)
{
    m_personLeft = item;
}

void MarriageItem::setPersonRight(DiagramItem *item)
{
    m_personRight = item;
}

DiagramItem *MarriageItem::personLeft() const
{
    return m_personLeft;
}

DiagramItem *MarriageItem::personRight() const
{
    return m_personRight;
}

int MarriageItem::type() const
{
    return DiagramItem::Marriage;
}

void MarriageItem::setContextMenu(QMenu *menu)
{
    m_contextMenu = menu;
}

MarriageItem *MarriageItem::getSelectedMarriage()
{
    return m_selectedMarriage;
}

void MarriageItem::contextMenuEvent(QGraphicsSceneContextMenuEvent *event)
{
    m_selectedMarriage = this;
    m_contextMenu->exec(event->screenPos());
}

QString MarriageItem::getPlace() const
{
    return m_place;
}

void MarriageItem::setPlace(const QString &place)
{
    m_place = place;
}

QDate MarriageItem::getDate() const
{
    return m_date;
}

void MarriageItem::setDate(const QDate &date)
{
    m_date = date;
}

//bool MarriageItem::isDateKnown() const
//{
//    return m_date.isValid();
//}
