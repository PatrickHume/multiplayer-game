#include"../headers/object.h"
int Object::nextId = 1;

Object::Object(rp3d::PhysicsWorld* physicsWorld, Model* model, rp3d::BodyType bodyType){
    id = nextId++;
    Object::model = model;
    model->addInstance();

    Object::physicsWorld = physicsWorld;
    rp3d::Vector3       position(0, 0, 0); 
    rp3d::Quaternion    orientation = rp3d::Quaternion::identity(); 
    rp3d::Transform     transform(position, orientation);
    // create a rigidbody at (0,0,0)
    body = physicsWorld->createRigidBody(transform);
    body->setType(bodyType);
    body->setMass(model->getMass());
}

void Object::addBoxCollider(BoxCollider collider, rp3d::CollisionShape *shape){
    rp3d::Transform transform(collider.position, collider.orientation);
    collider.transform = transform;
    boxColliders.push_back(collider);
    body->addCollider(shape, transform);
}

const rp3d::Transform& Object::getRP3DTransform(){
    return body->getTransform();
}

glm::mat4& Object::getGLMTransform(){
    getRP3DTransform().getOpenGLMatrix(glm::value_ptr(glmTransform));    
    return glmTransform;
}

void Object::Draw(Shader& shader, Camera& camera){
    model->Draw(shader, camera);
}

Model* Object::getModel(){
    return model;
}

void Object::drawOutline(Shader& blankShader, Shader& outlineShader, Camera& camera){
    model->setTransform(    getGLMTransform());
    model->drawPrepOutline( blankShader, camera);
    model->drawOutline(     outlineShader, camera);
}

void Object::drawColliders(Shader& shader, Camera& camera, Model& cube){
    rp3d::Transform transform   = body->getTransform();
    for (int i = 0; i < boxColliders.size(); i++){
        rp3d::Transform newTransform = boxColliders[i].transform * transform;
        glm::mat4 glmTransform;
        newTransform.getOpenGLMatrix(glm::value_ptr(glmTransform));
        glm::mat4 scale = glm::mat4(0.0f);
        scale[0][0] = boxColliders[i].halfExtents.x*2;
        scale[1][1] = boxColliders[i].halfExtents.y*2;
        scale[2][2] = boxColliders[i].halfExtents.z*2;
        scale[3][3] = 1.0f;
        glmTransform = glmTransform * scale;
        cube.setTransform(glmTransform);
        cube.Draw(shader, camera);
    }
}

void Object::drawId(Shader& shader, Camera& camera){
    model->drawId(shader, camera, id);
}

int Object::getId(){
    return id;
}

void Object::Delete(){
    model->delInstance();
}
