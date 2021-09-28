#ifndef MOVEITEMSUNDO_H
#define MOVEITEMSUNDO_H

#include <QList>
#include <QMap>
#include <QPointF>
#include <QUndoCommand>

class QGraphicsItem;
class DiagramScene;

class MoveItemsUndo : public QUndoCommand
{
public:
    MoveItemsUndo(DiagramScene *scene, QList<QGraphicsItem *> items, QUndoCommand *parent = nullptr);

    void undo() override;
    void redo() override;

    void storeAfterState();

private:
    DiagramScene *m_scene;
    QList<QGraphicsItem *> m_items;
    QMap<QGraphicsItem *, QPointF> m_posOld;
    QMap<QGraphicsItem *, QPointF> m_posNew;
    bool m_undone;
};

#endif // MOVEITEMSUNDO_H
