#include "changefillcolorundo.h"

#include "diagramitem.h"
#include "diagramscene.h"

ChangeFillColorUndo::ChangeFillColorUndo(DiagramItem *item,
                                         const QColor &newColor,
                                         QUndoCommand *parent) :
    QUndoCommand("set fill color", parent),
    m_item(item),
    m_colorNew(newColor),
    m_undone(false)
{
    m_colorOld = item->brush().color();
}

void ChangeFillColorUndo::undo()
{
    if (!m_undone)
    {
        m_item->setBrush(m_colorOld);
        m_undone = true;
    }
}

void ChangeFillColorUndo::redo()
{
    if (m_undone)
    {
        m_item->setBrush(m_colorNew);
        m_undone = false;
    }
}
