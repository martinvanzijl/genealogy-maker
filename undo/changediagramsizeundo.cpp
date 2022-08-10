#include "changediagramsizeundo.h"

#include "diagramscene.h"

ChangeDiagramSizeUndo::ChangeDiagramSizeUndo(DiagramScene *scene, int width, int height, QUndoCommand *parent):
    QUndoCommand("change diagram size", parent),
    m_scene(scene),
    m_widthBefore(scene->width()),
    m_heightBefore(scene->height()),
    m_widthAfter(width),
    m_heightAfter(height),
    m_undone(false)
{

}

void ChangeDiagramSizeUndo::undo()
{
    if (!m_undone)
    {
        m_scene->setSceneRect(0, 0, m_widthBefore, m_heightBefore);
        m_undone = true;
    }
}

void ChangeDiagramSizeUndo::redo()
{
    if (m_undone)
    {
        m_scene->setSceneRect(0, 0, m_widthAfter, m_heightAfter);
        m_undone = false;
    }
}
