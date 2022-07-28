#include "undoblock.h"

void UndoBlock::updateText()
{
    if (childCount() > 0) {
        setText(child(0)->text());
    }
}
