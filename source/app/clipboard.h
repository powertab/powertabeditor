#ifndef CLIPBOARD_H
#define CLIPBOARD_H

#include <vector>

class Position;
class Caret;
class UndoManager;

namespace Clipboard
{

void copySelection(const std::vector<Position*>& selectedPositions);
void paste(UndoManager* undoManager, const Caret* caret);

}
#endif // CLIPBOARD_H
