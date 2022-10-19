#include"../headers/object.h"
int Object::nextId = 1;

Object::Object(rp3d::PhysicsWorld* physicsWorld, std::shared_ptr<Model> model, rp3d::BodyType bodyType){
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

    //body->setMass(model->getMass());

    std::vector<rp3d::ConvexMeshShape*>& shapes = model->getCollisionShapes();
    for(const auto& shape : shapes){
        rp3d::Transform transform = rp3d::Transform::identity(); 
        body->addCollider(shape, transform);
    }
    body->updateLocalCenterOfMassFromColliders();
    body->updateLocalInertiaTensorFromColliders();
    body->updateMassFromColliders();
}

Object::~Object(){
    std::cout << "Deleted Object" << std::endl;
}

void Object::addBoxCollider(BoxCollider collider, rp3d::CollisionShape *shape){
    //glm::vec3 modelPosition = glm::vec3(0.0f, 0.0f, 0.0f);
    //glm::quat modelQuaternion = glm::mat4(1.0f);
    //glm::vec3 modelScale = glm::vec3(1.0f, 1.0f, 1.0f);

    rp3d::Transform transform(collider.position, collider.orientation);
    collider.RP3DTransform = transform;
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

void Object::Draw(std::shared_ptr<Shader>& shader, Camera& camera){
    model->Draw(shader, camera);
}

std::shared_ptr<Model> Object::getModel(){
    return model;
}

void Object::drawOutline(std::shared_ptr<Shader>& blankShader, std::shared_ptr<Shader>& outlineShader, Camera& camera){
    model->setTransform(    getGLMTransform());
    model->drawPrepOutline( blankShader, camera);
    model->drawOutline(     outlineShader, camera);
}

/*
Get the colliders
Loop through them
combine collider.getGLMTransform with object.getGLMTransform
If num matches user collider selection index
    Draw with flashing outline
Otherwise
    Draw regular

At this point colliders are related to objects, not to models
The model just gives the object some colliders on its creation.
*/
/*std::vector<glm::mat4>& Object::getColliderTransforms(){
    std::vector<glm::mat4> colliderTransforms(boxColliders.size());
    rp3d::Transform transform = body->getTransform();
    for (int i = 0; i < boxColliders.size(); i++){
        rp3d::Transform newTransform = transform * boxColliders[i].transform;
        glm::mat4 glmTransform;
        newTransform.getOpenGLMatrix(glm::value_ptr(glmTransform));
        glm::mat4 scale = glm::mat4(0.0f);
        scale[0][0] = boxColliders[i].halfExtents.x*2;
        scale[1][1] = boxColliders[i].halfExtents.y*2;
        scale[2][2] = boxColliders[i].halfExtents.z*2;
        scale[3][3] = 1.0f;
        glmTransform = glmTransform * scale;
        colliderTransforms[i] = glmTransform;
    }
    return colliderTransforms;
}*/

void Object::drawColliders(std::shared_ptr<Shader>& shader, Camera& camera, Model& cube){
    rp3d::Transform transform   = body->getTransform();
    for (int i = 0; i < boxColliders.size(); i++){
        rp3d::Transform newTransform = boxColliders[i].RP3DTransform * transform;
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

void Object::drawId(std::shared_ptr<Shader>& shader, Camera& camera){
    model->drawId(shader, camera, id);
}

int Object::getId(){
    return id;
}

void Object::Delete(){
    model->delInstance();
}
