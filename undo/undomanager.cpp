#include "undomanager.h"

#include <QUndoCommand>
#include <QUndoStack>

QUndoStack *UndoManager::m_stack = nullptr;

UndoManager::UndoManager()
{

}

void UndoManager::add(QUndoCommand *command)
{
    if (m_stack) {
        m_stack->push(command);
    }
}

void UndoManager::setStack(QUndoStack *stack)
{
    m_stack = stack;
}
