#ifndef CHANGEBORDERCOLORUNDO_H
#define CHANGEBORDERCOLORUNDO_H

#include <QColor>
#include <QUndoCommand>

class DiagramItem;

class ChangeBorderColorUndo : public QUndoCommand
{
public:
    ChangeBorderColorUndo(DiagramItem *item, const QColor &newColor, QUndoCommand *parent = nullptr);

    void undo() override;
    void redo() override;

private:
    DiagramItem *m_item;
    QColor m_colorOld;
    QColor m_colorNew;
    bool m_undone;
};

#endif // CHANGEBORDERCOLORUNDO_H
