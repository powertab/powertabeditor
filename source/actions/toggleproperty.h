#ifndef TOGGLEPROPERTY_H
#define TOGGLEPROPERTY_H

#include <QUndoCommand>
#include <vector>
#include <functional>
#include <algorithm>

/// This class is designed to provide an easy way to create a QUndoCommand for the common situation of toggling a property
/// It takes a list of objects, a function for setting the property, a function for getting the value of the property,
/// and the name of the property (for use in the Undo/Redo menu)

template<class T>
class ToggleProperty : public QUndoCommand
{
    typedef std::function<bool (T*, bool)> PropertySetter;
    typedef std::function<bool (const T*)> PropertyGetter;

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
