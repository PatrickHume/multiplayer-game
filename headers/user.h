#ifndef USER_CLASS_H
#define USER_CLASS_H

#include"object.h"

// this keeps track of which game objects are selected
class User
{
public:
	User();

    void selectObject(std::shared_ptr<Object> object);
    std::shared_ptr<Object> getSelectedObject();
    void deselectObject();
    bool hasSelectedObject();
    // A bool determining whether or not to show object colliders.
    // Can be set by the user with the 'show colliders' and 'hide colliders' commands.
    bool viewingColliders = false;
    bool editingColliders = false;
    int  selectedCollider = 0;
private:
    std::shared_ptr<Object> selectedObject;
    bool objectIsSelected = false;
};

#endif
