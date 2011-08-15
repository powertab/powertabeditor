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
  
#ifndef TOGGLEPROPERTY_H
#define TOGGLEPROPERTY_H

#include <QUndoCommand>
#include <vector>
#include <boost/function.hpp>
#include <algorithm>

/// This class is designed to provide an easy way to create a QUndoCommand for the common situation of toggling a property
/// It takes a list of objects, a function for setting the property, a function for getting the value of the property,
/// and the name of the property (for use in the Undo/Redo menu)

template<class T>
class ToggleProperty : public QUndoCommand
{
    typedef boost::function<bool (T*, bool)> PropertySetter;
    typedef boost::function<bool (const T*)> PropertyGetter;

public:
    ToggleProperty(const std::vector<T*>& items, PropertySetter setPropertyFn,
                   PropertyGetter getPropertyFn, const QString& propertyName) :
        items(items),
        setPropertyFn(setPropertyFn)
    {
        // If at least one object does not have the property set, set it for all items
        setProperty = std::find_if(items.begin(), items.end(), std::not1(getPropertyFn)) != items.end();

        // set the text for the undo menu
        setText(setProperty ? "Set " + propertyName :
                              "Remove " + propertyName);
    }

    void redo()
    {
        // set or unset the property for each object
        std::for_each(items.begin(), items.end(), std::bind2nd(setPropertyFn, setProperty));
    }

    void undo()
    {
        std::for_each(items.begin(), items.end(), std::bind2nd(setPropertyFn, !setProperty));
    }

protected:
    std::vector<T*> items;
    PropertySetter setPropertyFn; ///< function for setting the property
    bool setProperty; ///< indicates whether we are going to add or remove the property
};

#endif // TOGGLEPROPERTY_H
