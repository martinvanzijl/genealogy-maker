#ifndef UNDOMANAGER_H
#define UNDOMANAGER_H

class QUndoCommand;
class QUndoStack;

class UndoBlock;

class UndoManager
{
public:
    UndoManager();

    static void add(QUndoCommand *command);
    static void addBlock(UndoBlock *block);
    static void setStack(QUndoStack *stack);

private:
    static QUndoStack *m_stack;
};

#endif // UNDOMANAGER_H
