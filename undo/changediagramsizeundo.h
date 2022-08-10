#ifndef CHANGEDIAGRAMSIZEUNDO_H
#define CHANGEDIAGRAMSIZEUNDO_H

#include <QUndoCommand>

class DiagramScene;

class ChangeDiagramSizeUndo : public QUndoCommand
{
public:
    ChangeDiagramSizeUndo(DiagramScene *scene, int width, int height, QUndoCommand *parent = nullptr);

    void undo() override;
    void redo() override;

private:
    DiagramScene *m_scene;

    int m_widthBefore;
    int m_heightBefore;

    int m_widthAfter;
    int m_heightAfter;

    bool m_undone;
};

#endif // CHANGEDIAGRAMSIZEUNDO_H
