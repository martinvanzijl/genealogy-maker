#include "marriageitem.h"

#include "diagramitem.h"

#include <QMenu>
#include <QGraphicsSceneContextMenuEvent>

QMenu *MarriageItem::m_contextMenu = nullptr;
MarriageItem *MarriageItem::m_selectedMarriage = nullptr;

MarriageItem::MarriageItem(QGraphicsItem *parent) :
    QGraphicsEllipseItem(0, 0, 32, 32, parent),
    m_personLeft(nullptr),
    m_personRight(nullptr)
{

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
