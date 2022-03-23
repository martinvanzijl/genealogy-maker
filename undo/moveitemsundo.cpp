#include "moveitemsundo.h"

#include "diagramitem.h"
#include "diagramscene.h"

#include <QGraphicsView>

MoveItemsUndo::MoveItemsUndo(DiagramScene *scene, QList<QGraphicsItem *> items, QUndoCommand *parent) :
    QUndoCommand("move items", parent),
    m_scene(scene),
    m_items(items),
    m_moveView(false),
    m_undone(false)
{
    for (auto item: m_items) {
        m_posOld[item] = item->pos();
    }
}

void MoveItemsUndo::storeAfterState()
{
    for (auto item: m_items) {
        m_posNew[item] = item->pos();
    }
}

void MoveItemsUndo::setMoveView(bool moveView)
{
    m_moveView = moveView;
}

void MoveItemsUndo::moveViewsIfRequired()
{
    if (m_moveView) {
        for (auto view: m_scene->views()) {
            view->centerOn(m_scene->firstItem());
        }
    }
}

void MoveItemsUndo::undo()
{
    if (!m_undone)
    {
        for (QGraphicsItem* item: m_items) {
            item->setPos(m_posOld[item]);
        }
        moveViewsIfRequired();
        m_undone = true;
    }
}

void MoveItemsUndo::redo()
{
    if (m_undone)
    {
        for (auto item: m_items) {
            item->setPos(m_posNew[item]);
        }
        moveViewsIfRequired();
        m_undone = false;
    }
}

