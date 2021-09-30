#ifndef MARRIAGEUNDO_H
#define MARRIAGEUNDO_H

#include <QUndoCommand>

class DiagramItem;
class DiagramScene;

class MarriageUndo : public QUndoCommand
{
public:
    MarriageUndo(DiagramScene *scene, DiagramItem *person1, DiagramItem *person2, QUndoCommand *parent = nullptr);

    void undo() override;
    void redo() override;

private:
    DiagramScene *m_scene;
    DiagramItem *m_person1;
    DiagramItem *m_person2;
    bool m_undone;
};

#endif // MARRIAGEUNDO_H
