#include "addarrowundo.h"

#include "arrow.h"
#include "diagramitem.h"
#include "genealogymaker.h"

AddArrowUndo::AddArrowUndo(DiagramScene *scene, Arrow *arrow, QUndoCommand *parent) :
    QUndoCommand("add arrow", parent),
    m_scene(scene),
    m_arrow(arrow),
    m_undone(false)
{

}

void AddArrowUndo::undo()
{
    if (!m_undone)
    {
        m_scene->removeItem(m_arrow);
        m_arrow->startItem()->removeArrow(m_arrow);
        m_arrow->endItem()->removeArrow(m_arrow);
        m_undone = true;
    }
}

void AddArrowUndo::redo()
{
    if (m_undone)
    {
        m_scene->addItem(m_arrow);
        m_arrow->startItem()->addArrow(m_arrow);
        m_arrow->endItem()->addArrow(m_arrow);
        m_undone = false;
    }
}
