#ifndef SCOREARRANGER_H
#define SCOREARRANGER_H

#include <memory>
#include <vector>

class Barline;
class Position;
class Score;
class AlternateEnding;

/// Stores data for a single bar - barline, an (optional) alternate ending,
/// and a list of positions (one for each staff)
struct BarData
{
    std::shared_ptr<Barline> barline;
    std::shared_ptr<AlternateEnding> altEnding;
    std::vector<std::vector<Position*> > positionLists;
};

void arrangeScore(Score* score, const std::vector<BarData>& bars);

#endif // SCOREARRANGER_H
