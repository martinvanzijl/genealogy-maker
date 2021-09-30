#ifndef MARRIAGEITEM_H
#define MARRIAGEITEM_H

#include <QGraphicsPixmapItem>

class DiagramItem;

class MarriageItem : public QGraphicsEllipseItem
{
public:
    MarriageItem(QGraphicsItem *parent = 0);

    void setPersonLeft(DiagramItem *item);
    void setPersonRight(DiagramItem *item);
    DiagramItem *personLeft() const;
    DiagramItem * personRight() const;

    int type() const override;

private:
    DiagramItem *m_personLeft;
    DiagramItem *m_personRight;
};

#endif // MARRIAGEITEM_H
