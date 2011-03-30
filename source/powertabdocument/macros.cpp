#include "macros.h"
#include <algorithm> // std::transform

std::string ArabicToRoman(uint32_t number, bool upperCase)
{
    // Can only convert 1 to 5999
    CHECK_THAT(((number > 0) && (number < 6000)), "");

    std::string returnValue;
    while (number >= 1000)	{number -= 1000; returnValue += "m";}
    while (number >= 900)	{number -= 900; returnValue += "cm";}
    while (number >= 500)	{number -= 500; returnValue += "d";}
    while (number >= 400)	{number -= 400; returnValue += "cd";}
    while (number >= 100)	{number -= 100; returnValue += "c";}
    while (number >= 90)	{number -= 90; returnValue += "xc";}
    while (number >= 50)	{number -= 50; returnValue += "l";}
    while (number >= 40)	{number -= 40; returnValue += "xl";}
    while (number >= 10)	{number -= 10; returnValue += "x";}
    while (number >= 9)	    {number -= 9; returnValue += "ix";}
    while (number >= 5)	    {number -= 5; returnValue += "v";}
    while (number >= 4)	    {number -= 4; returnValue += "iv";}
    while (number >= 1)	    {number -= 1; returnValue += "i";}

    // Force the text to upper case
    if (upperCase)
    {
        std::transform(returnValue.begin(), returnValue.end(), returnValue.begin(), ::toupper);
    }

    return (returnValue);
}
