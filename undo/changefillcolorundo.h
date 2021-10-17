#ifndef CHANGEFILLCOLORUNDO_H
#define CHANGEFILLCOLORUNDO_H

#include <QColor>
#include <QList>
#include <QMap>
#include <QUndoCommand>

class DiagramItem;

class ChangeFillColorUndo : public QUndoCommand
{
public:
    ChangeFillColorUndo(DiagramItem *item, const QColor &newColor, QUndoCommand *parent = nullptr);

    void undo() override;
    void redo() override;

private:
    DiagramItem *m_item;
    QColor m_colorOld;
    QColor m_colorNew;
    bool m_undone;
};


#endif // CHANGEFILLCOLORUNDO_H
