#ifndef USER_CLASS_H
#define USER_CLASS_H

#include"object.h"

// this keeps track of which game objects are selected
class User
{
public:
	User();

    void selectObject(Object* object);
    Object* getSelectedObject();
    void deselectObject();
    bool hasSelectedObject();
private:
    Object* selectedObject;
    bool objectIsSelected = false;
};

#endif
