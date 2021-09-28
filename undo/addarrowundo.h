#ifndef ADDARROWUNDO_H
#define ADDARROWUNDO_H

#include <QUndoCommand>

class Arrow;
class DiagramScene;

class AddArrowUndo : public QUndoCommand
{
public:
    AddArrowUndo(DiagramScene *scene, Arrow *arrow, QUndoCommand *parent = nullptr);

    void undo() override;
    void redo() override;

private:
    DiagramScene *m_scene;
    Arrow *m_arrow;
    bool m_undone;
};

#endif // ADDARROWUNDO_H
