#include "changebordercolorundo.h"

#include "diagramitem.h"
#include "diagramscene.h"

ChangeBorderColorUndo::ChangeBorderColorUndo(DiagramItem *item,
                                             const QColor &newColor,
                                             QUndoCommand *parent) :
    QUndoCommand("set line color", parent),
    m_item(item),
    m_colorNew(newColor),
    m_undone(false)
{
    m_colorOld = item->getBorderColor();
}

void ChangeBorderColorUndo::undo()
{
    if (!m_undone)
    {
        m_item->setBorderColor(m_colorOld);
        m_undone = true;
    }
}

void ChangeBorderColorUndo::redo()
{
    if (m_undone)
    {
        m_item->setBorderColor(m_colorNew);
        m_undone = false;
    }
}

