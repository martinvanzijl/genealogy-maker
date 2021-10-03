#ifndef REMOVEMARRIAGEUNDO_H
#define REMOVEMARRIAGEUNDO_H

#include <QUndoCommand>

class MarriageItem;
class DiagramScene;

class RemoveMarriageUndo : public QUndoCommand
{
public:
    RemoveMarriageUndo(DiagramScene *scene, MarriageItem *item, QUndoCommand *parent = nullptr);

    void undo() override;
    void redo() override;

private:
    DiagramScene *m_scene;
    MarriageItem *m_item;
    bool m_undone;
};

#endif // REMOVEMARRIAGEUNDO_H
