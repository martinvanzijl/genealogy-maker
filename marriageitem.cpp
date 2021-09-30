#include "marriageitem.h"

#include "diagramitem.h"

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
