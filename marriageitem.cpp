#include "marriageitem.h"

#include "diagramitem.h"

MarriageItem::MarriageItem(QGraphicsItem *parent) :
    QGraphicsEllipseItem(0, 0, 32, 32, parent)
{

}

int MarriageItem::type() const
{
    return DiagramItem::Marriage;
}
