#include "additemundo.h"

#include "diagramitem.h"
#include "diagramscene.h"

AddItemUndo::AddItemUndo(DiagramScene *scene, DiagramItem *item, QUndoCommand *parent) :
    QUndoCommand("add person", parent),
    m_scene(scene),
    m_item(item),
    m_undone(false)
{

}

void AddItemUndo::undo()
{
    if (!m_undone)
    {
        m_scene->removeItem(m_item);
        m_undone = true;
    }
}

void AddItemUndo::redo()
{
    if (m_undone)
    {
        m_scene->addItem(m_item);
        m_undone = false;
    }
}
