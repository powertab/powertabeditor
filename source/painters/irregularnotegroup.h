#ifndef IRREGULARNOTEGROUP_H
#define IRREGULARNOTEGROUP_H

#include "notestem.h"

#include <vector>
#include <cstdint>

class QGraphicsItem;

class IrregularNoteGroup
{
public:
    IrregularNoteGroup(const std::vector<NoteStem>& noteStems);

    void draw(QGraphicsItem* parent);

    static const uint8_t GROUP_HEIGHT = 6;
    static const uint8_t GROUP_SPACING = 3;

private:
    std::vector<NoteStem> noteStems;
};

#endif // IRREGULARNOTEGROUP_H
