#include"../headers/user.h"

// this class is a bit odd and may be removed in a later refactor
User::User(){

}

void User::selectObject(Object* object){
    objectIsSelected = true;
    selectedObject = object;
}

Object* User::getSelectedObject(){
    return selectedObject;
}

bool User::hasSelectedObject(){
    return objectIsSelected;
}

void User::deselectObject(){
    objectIsSelected = false;
}
