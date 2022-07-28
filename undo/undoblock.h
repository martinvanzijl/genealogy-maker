#ifndef UNDOBLOCK_H
#define UNDOBLOCK_H

#include <QUndoCommand>

class UndoBlock : public QUndoCommand
{
 public:
    void updateText();
};

#endif // UNDOBLOCK_H
