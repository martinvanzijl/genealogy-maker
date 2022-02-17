#include "changetextcolorundo.h"

#include "diagramitem.h"
#include "diagramscene.h"

ChangeTextColorUndo::ChangeTextColorUndo(DiagramItem *item,
                                         const QColor &newColor,
                                         QUndoCommand *parent) :
    QUndoCommand("set text color", parent),
    m_item(item),
    m_colorNew(newColor),
    m_undone(false)
{
    m_colorOld = item->getTextColor();
}

void ChangeTextColorUndo::undo()
{
    if (!m_undone)
    {
        m_item->setTextColor(m_colorOld);
        m_undone = true;
    }
}

void ChangeTextColorUndo::redo()
{
    if (m_undone)
    {
        m_item->setTextColor(m_colorNew);
        m_undone = false;
    }
}
