#ifndef STAFFDATA_H
#define STAFFDATA_H

// Structure to hold data about a staff, and provide common layout-related calculations

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
    static const int staffBorderSpacing;

    StaffData();

    int getTabLineHeight(int stringNum, bool absolutePos = true) const;
    int getTopTabLine(bool absolutePos = true) const;
    int getBottomTabLine(bool absolutePos = true) const;
    int getTabStaffSize() const;

    int getStdNotationLineHeight(int lineNumber, bool absolutePos = true) const;
    double getStdNotationSpaceHeight(int spaceNumber, bool absolutePos = true) const;
    int getTopStdNotationLine(bool absolutePos = true) const;
    int getBottomStdNotationLine(bool absolutePos = true) const;

    int getStdNotationStaffSize() const;

    void calculateHeight();

    int getTabStaffOffset() const;
};

#endif // STAFFDATA_H
