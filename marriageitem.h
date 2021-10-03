#ifndef MARRIAGEITEM_H
#define MARRIAGEITEM_H

#include <QGraphicsPixmapItem>
#include <QDate>

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

    QDate getDate() const;
    void setDate(const QDate &date);
    bool isDateKnown() const;

    QString getPlace() const;
    void setPlace(const QString &place);

protected:
    void contextMenuEvent(QGraphicsSceneContextMenuEvent *event) override;

private:
    DiagramItem *m_personLeft;
    DiagramItem *m_personRight;

    QDate m_date;
    QString m_place;

    static QMenu *m_contextMenu;
    static MarriageItem *m_selectedMarriage;
};

#endif // MARRIAGEITEM_H
