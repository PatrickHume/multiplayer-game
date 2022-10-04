#include"../headers/user.h"

User::User(){

}

void User::selectObject(Object* object){
    objectIsSelected = true;
    selectedObject = object;
    selectedObject->isSelected = true;
}

void User::drawSelected(Shader& shader, Camera& camera){
    if(objectIsSelected){
        selectedObject->Draw(shader, camera, DRAW_OUTLINE);
    }
}

void User::deselectAll(){
    objectIsSelected = false;
    selectedObject->isSelected = false;
}
