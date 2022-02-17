#ifndef CHANGETEXTCOLORUNDO_H
#define CHANGETEXTCOLORUNDO_H

#include <QColor>
#include <QUndoCommand>

class DiagramItem;

class ChangeTextColorUndo : public QUndoCommand
{
public:
    ChangeTextColorUndo(DiagramItem *item, const QColor &newColor, QUndoCommand *parent = nullptr);

    void undo() override;
    void redo() override;

private:
    DiagramItem *m_item;
    QColor m_colorOld;
    QColor m_colorNew;
    bool m_undone;
};

#endif // CHANGETEXTCOLORUNDO_H
