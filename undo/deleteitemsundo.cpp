#include "deleteitemsundo.h"

#include "arrow.h"
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
            if (item->type() == DiagramItem::Type) {
                auto diagramItem = qgraphicsitem_cast<DiagramItem *> (item);
                m_scene->addPersonFromUndo(diagramItem);
            }
            else if(item->type() == Arrow::Type) {
                auto arrow = qgraphicsitem_cast<Arrow *> (item);
                arrow->startItem()->addArrow(arrow);
                arrow->endItem()->addArrow(arrow);
                m_scene->addItem(arrow);
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
            else if(item->type() == Arrow::Type) {
                auto arrow = qgraphicsitem_cast<Arrow *> (item);
                arrow->startItem()->removeArrow(arrow);
                arrow->endItem()->removeArrow(arrow);
                m_scene->removeItem(arrow);
            }
        }
        m_undone = false;
    }
}
