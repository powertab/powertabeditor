#ifndef NOTESTEM_H
#define NOTESTEM_H

#include <vector>

class Position;

class NoteStem
{
public:
    enum StemDirection
    {
        StemUp,
        StemDown
    };

    NoteStem(const Position* position, int xPosition, const std::vector<int>& noteLocations);

    const Position* position;
    int xPosition;
    int stemTop;
    int stemBottom;
    StemDirection stemDirection;

    float stemSize() const;
    int stemEdge() const;
    bool canDrawFlag() const;

    static NoteStem::StemDirection findDirectionForGroup(const std::vector<NoteStem>& stems);
};

bool compareStemTopPositions(const NoteStem& stem1, const NoteStem& stem2);
bool compareStemBottomPositions(const NoteStem& stem1, const NoteStem& stem2);

#endif // NOTESTEM_H
