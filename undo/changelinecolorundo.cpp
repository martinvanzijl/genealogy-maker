#include "changelinecolorundo.h"

#include "arrow.h"

ChangeLineColorUndo::ChangeLineColorUndo(Arrow *arrow,
                                         const QColor &newColor,
                                         QUndoCommand *parent) :
    QUndoCommand("set line color", parent),
    m_arrow(arrow),
    m_colorNew(newColor),
    m_undone(false)
{
    m_colorOld = arrow->getColor();
}

void ChangeLineColorUndo::undo()
{
    if (!m_undone)
    {
        m_arrow->setColor(m_colorOld);
        m_arrow->update();
        m_undone = true;
    }
}

void ChangeLineColorUndo::redo()
{
    if (m_undone)
    {
        m_arrow->setColor(m_colorNew);
        m_arrow->update();
        m_undone = false;
    }
}
