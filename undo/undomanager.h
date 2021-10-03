#ifndef UNDOMANAGER_H
#define UNDOMANAGER_H

class QUndoCommand;
class QUndoStack;

class UndoManager
{
public:
    UndoManager();

    static void add(QUndoCommand *command);
    static void setStack(QUndoStack *stack);
};

#endif // UNDOMANAGER_H
