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
    int numOfStrings;
    static const int numOfStdNotationLines = 5;
    int stdNotationStaffAboveSpacing;
    int stdNotationStaffBelowSpacing;
    int symbolSpacing;
    int positionWidth;
    int tabStaffBelowSpacing;
    static const int staffBorderSpacing;

    StaffData();

    int getTabLineHeight(int stringNum) const;
    int getTopTabLine() const;
    int getBottomTabLine() const;
    int getTabStaffSize() const;

    int getStdNotationLineHeight(int lineNumber) const;
    double getStdNotationSpaceHeight(int spaceNumber) const;
    int getTopStdNotationLine() const;
    int getBottomStdNotationLine() const;

    int getStdNotationStaffSize() const;

    double getNoteHeadRightEdge() const;

    void calculateHeight();

    int getTabStaffOffset() const;
};

#endif // STAFFDATA_H
