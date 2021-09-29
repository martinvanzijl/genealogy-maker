#ifndef MARRIAGEITEM_H
#define MARRIAGEITEM_H

#include <QGraphicsPixmapItem>

class MarriageItem : public QGraphicsEllipseItem
{
public:
    MarriageItem(QGraphicsItem *parent = 0);

    int type() const override;
};

#endif // MARRIAGEITEM_H
