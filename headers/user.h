#ifndef USER_CLASS_H
#define USER_CLASS_H

#include"object.h"

// this keeps track of which game objects are selected
class User
{
public:
	User();

    void selectObject(Object* object);
    void drawSelected(Shader& shader, Camera& camera);
    void deselectAll();
private:
    Object* selectedObject;
    bool objectIsSelected = false;
};

#endif
