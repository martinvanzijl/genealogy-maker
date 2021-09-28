#include "deleteitemsundo.h"

#include "diagramitem.h"
#include "diagramscene.h"

DeleteItemsUndo::DeleteItemsUndo(DiagramScene *scene, QList<QGraphicsItem *> items, QUndoCommand *parent) :
    QUndoCommand("delete items", parent),
    m_scene(scene),
    m_items(items),
    m_undone(false)
{

}

void DeleteItemsUndo::undo()
{
    if (!m_undone)
    {
        for (auto item: m_items) {
            m_scene->addItem(item);
        }
        m_undone = true;
    }
}

void DeleteItemsUndo::redo()
{
    if (m_undone)
    {
        for (auto item: m_items) {
            m_scene->removeItem(item);
        }
        m_undone = false;
    }
}
