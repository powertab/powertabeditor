/*
  * Copyright (C) 2011 Cameron White
  *
  * This program is free software: you can redistribute it and/or modify
  * it under the terms of the GNU General Public License as published by
  * the Free Software Foundation, either version 3 of the License, or
  * (at your option) any later version.
  *
  * This program is distributed in the hope that it will be useful,
  * but WITHOUT ANY WARRANTY; without even the implied warranty of
  * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  * GNU General Public License for more details.
  *
  * You should have received a copy of the GNU General Public License
  * along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
  
#include "macros.h"
#include <algorithm> // std::transform
#include <iostream>

void logToDebug(const std::string& msg, const std::string& file, int line)
{
    std::cerr << msg << std::endl << file << std::endl << "Line: " <<  line << std::endl;
}

std::string ArabicToRoman(uint32_t number, bool upperCase)
{
    // Can only convert 1 to 5999
    PTB_CHECK_THAT(((number > 0) && (number < 6000)), "");

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
