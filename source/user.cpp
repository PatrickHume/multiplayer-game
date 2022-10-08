#include"../headers/user.h"

// this class is a bit odd and may be removed in a later refactor
User::User(){

}

void User::selectObject(Object* object){
    if(objectIsSelected){
        selectedObject->isSelected = false;
    }
    objectIsSelected = true;
    selectedObject = object;
    selectedObject->isSelected = true;
}

void User::drawSelected(Shader& outlineShader, Camera& camera){
    if(objectIsSelected){
        selectedObject->drawOutline(outlineShader, camera);
    }
}

void User::deselectObject(){
    if(objectIsSelected){
        selectedObject->isSelected = false;
        objectIsSelected = false;
    }
}
