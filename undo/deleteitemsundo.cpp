#include "deleteitemsundo.h"

#include "diagramitem.h"
#include "genealogymaker.h"

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
            if (item->type() == DiagramItem::Type) {
                auto diagramItem = qgraphicsitem_cast<DiagramItem *> (item);
                m_scene->addPersonFromUndo(diagramItem);
            }
        }
        m_undone = true;
    }
}

void DeleteItemsUndo::redo()
{
    if (m_undone)
    {
        for (auto item: m_items) {
            if (item->type() == DiagramItem::Type) {
                auto diagramItem = qgraphicsitem_cast<DiagramItem *> (item);
                m_scene->removePersonFromUndo(diagramItem);
            }
        }
        m_undone = false;
    }
}
