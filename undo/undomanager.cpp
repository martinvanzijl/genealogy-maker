#include "undomanager.h"

#include <QUndoCommand>
#include <QUndoStack>

#include "undoblock.h"

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

void UndoManager::addBlock(UndoBlock *block)
{
    // Do not add empty blocks.
    if (block->childCount() == 0) {
        delete block;
        return;
    }

    // Add block.
    block->updateText();
    add(block);
}

void UndoManager::setStack(QUndoStack *stack)
{
    m_stack = stack;
}
