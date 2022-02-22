#ifndef CHANGELINECOLORUNDO_H
#define CHANGELINECOLORUNDO_H

#include <QColor>
#include <QUndoCommand>

class Arrow;

class ChangeLineColorUndo : public QUndoCommand
{
public:
    ChangeLineColorUndo(Arrow *arrow, const QColor &newColor, QUndoCommand *parent = nullptr);

    void undo() override;
    void redo() override;

private:
    Arrow *m_arrow;
    QColor m_colorOld;
    QColor m_colorNew;
    bool m_undone;
};

#endif // CHANGELINECOLORUNDO_H
