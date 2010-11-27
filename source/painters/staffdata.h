#ifndef STAFFDATA_H
#define STAFFDATA_H

// Structure to hold data about a score, and provide common layout-related calculations

class StaffData
{
public:
    int height;
    int topEdge;
    int leftEdge;
    int width;
    int tabLineSpacing;
    static const int stdNotationLineSpacing = 7;
    int numOfStrings;
    static const int numOfStdNotationLines = 5;
    int stdNotationStaffAboveSpacing;
    int stdNotationStaffBelowSpacing;
    int symbolSpacing;
    int positionWidth;
    int tabStaffBelowSpacing;

    StaffData();

    int getTabLineHeight(int stringNum) const
    {
        return (topEdge + height - tabStaffBelowSpacing - (numOfStrings - stringNum) * tabLineSpacing);
    }

    int getTopTabLine() const
    {
        return getTabLineHeight(1);
    }

    int getBottomTabLine() const
    {
        return getTabLineHeight(numOfStrings);
    }

    int getTabStaffSize() const
    {
        return getBottomTabLine() - getTopTabLine();
    }

    int getStdNotationLineHeight(int lineNumber) const
    {
        return (topEdge + stdNotationStaffAboveSpacing + (lineNumber - 1) * stdNotationLineSpacing);
    }

    double getStdNotationSpaceHeight(int spaceNumber) const
    {
        return (getStdNotationLineHeight(spaceNumber) + getStdNotationLineHeight(spaceNumber + 1)) / 2;
    }

    int getTopStdNotationLine() const
    {
        return getStdNotationLineHeight(1);
    }

    int getBottomStdNotationLine() const
    {
        return getStdNotationLineHeight(numOfStdNotationLines);
    }

    int getStdNotationStaffSize() const
    {
        return getBottomStdNotationLine() - getTopStdNotationLine();
    }

    void calculateHeight()
    {
        height = stdNotationStaffAboveSpacing + stdNotationStaffBelowSpacing + tabStaffBelowSpacing + symbolSpacing +
                 numOfStrings * tabLineSpacing + numOfStdNotationLines * stdNotationLineSpacing;
    }
};

#endif // STAFFDATA_H
