#include "marriageundo.h"

#include "moveitemsundo.h"

#include "diagramitem.h"
#include "diagramscene.h"

MarriageUndo::MarriageUndo(DiagramScene *scene, DiagramItem *person1, DiagramItem *person2, QUndoCommand *parent) :
    QUndoCommand("marriage", parent),
    m_scene(scene),
    m_person1(person1),
    m_person2(person2),
    m_undone(false)
{
    m_person1Pos = person1->pos();
    m_person2Pos = person2->pos();
}

void MarriageUndo::undo()
{
    if (!m_undone)
    {
        m_scene->removeMarriage(m_person1, m_person2);
        m_undone = true;
    }
}

void MarriageUndo::redo()
{
    if (m_undone)
    {
        m_scene->marry(m_person1, m_person2, true);
        m_person1->setPos(m_person1Pos);
        m_person2->setPos(m_person2Pos);
        m_undone = false;
    }
}

