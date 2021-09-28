#ifndef ADDITEMUNDO_H
#define ADDITEMUNDO_H

#include <QUndoCommand>

class DiagramItem;
class DiagramScene;

class AddItemUndo : public QUndoCommand
{
public:
    AddItemUndo(DiagramScene *scene, DiagramItem *item, QUndoCommand *parent = nullptr);

    void undo() override;
    void redo() override;

private:
    DiagramScene *m_scene;
    DiagramItem *m_item;
    bool m_undone;
};

#endif // ADDITEMUNDO_H
