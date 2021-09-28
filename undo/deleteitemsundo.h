#ifndef UNDODELETEITEMS_H
#define UNDODELETEITEMS_H

#include <QList>
#include <QUndoCommand>

class QGraphicsItem;
class DiagramScene;

class DeleteItemsUndo : public QUndoCommand
{
public:
    DeleteItemsUndo(DiagramScene *scene, QList<QGraphicsItem *> items, QUndoCommand *parent = nullptr);

    void undo() override;
    void redo() override;

private:
    DiagramScene *m_scene;
    QList<QGraphicsItem *> m_items;
    bool m_undone;
};

#endif // UNDODELETEITEMS_H
