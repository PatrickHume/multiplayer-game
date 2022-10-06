#include"../headers/user.h"

// this class is a bit odd and may be removed in a later refactor
User::User(){

}

void User::selectObject(Object* object){
    objectIsSelected = true;
    selectedObject = object;
    selectedObject->isSelected = true;
}

void User::drawSelected(Shader& shader, Camera& camera){
    if(objectIsSelected){
        selectedObject->Draw(shader, camera, DrawType::OUTLINE);
    }
}

void User::deselectAll(){
    objectIsSelected = false;
    selectedObject->isSelected = false;
}
