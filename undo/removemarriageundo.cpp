#include "removemarriageundo.h"

#include "marriageitem.h"
#include "diagramitem.h"

RemoveMarriageUndo::RemoveMarriageUndo(DiagramScene *scene, MarriageItem *item, QUndoCommand *parent) :
    QUndoCommand("remove marriage", parent),
    m_scene(scene),
    m_item(item),
    m_undone(false)
{

}

void RemoveMarriageUndo::undo()
{
    if (!m_undone)
    {
        m_item->personLeft()->marryTo(m_item->personRight());

        auto newMarriageItem = m_item->personLeft()->getMarriageItem();
        newMarriageItem->setDate(m_item->getDate());
        newMarriageItem->setPlace(m_item->getPlace());

        delete m_item;
        m_item = newMarriageItem;

        m_undone = true;
    }
}

void RemoveMarriageUndo::redo()
{
    if (m_undone)
    {
        m_item->personLeft()->removeMarriage();
        m_undone = false;
    }
}
