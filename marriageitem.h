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
    static void setContextMenu(QMenu *menu);
    static MarriageItem *getSelectedMarriage();

protected:
    void contextMenuEvent(QGraphicsSceneContextMenuEvent *event) override;

private:
    DiagramItem *m_personLeft;
    DiagramItem *m_personRight;

    static QMenu *m_contextMenu;
    static MarriageItem *m_selectedMarriage;
};

#endif // MARRIAGEITEM_H
