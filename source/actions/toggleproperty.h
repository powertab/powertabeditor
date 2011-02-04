#ifndef TOGGLEPROPERTY_H
#define TOGGLEPROPERTY_H

#include <QUndoCommand>

// This class is designed to provide an easy way to create a QUndoCommand for the common situation of toggling a property
// It takes an object and a pointer to a member function of that object.
// The member function must take a boolean parameter (i.e. for setting/removing a property)
// The member function is called, using the setProperty variable to specify whether the property is being set or removed

template<class T>
class ToggleProperty : public QUndoCommand
{
    typedef bool (T::*SetPropertyFn)(bool set);

public:
    ToggleProperty(T *item, SetPropertyFn setPropertyFn, bool setProperty, const QString& text) :
        item(item),
        setPropertyFn(setPropertyFn),
        setProperty(setProperty)
    {
        setText(text);
    }

    void undo()
    {
        // ugly syntax, but this just calls the member function, using the pointer-to-member-function provided
        (item->*setPropertyFn)(!setProperty);
    }

    void redo()
    {
        (item->*setPropertyFn)(setProperty);
    }

protected:
    T* item;
    SetPropertyFn setPropertyFn;  // pointer to a member function of the class
    bool setProperty; // whether we are adding or removing the property
};

#endif // TOGGLEPROPERTY_H
