#include"../headers/object.h"
int Object::nextId = 0;

Object::Object(rp3d::PhysicsWorld* physicsWorld, Model* model){
    id = nextId++;
    Object::model = model;

    Object::physicsWorld = physicsWorld;
    rp3d::Vector3       position(0, 0, 0); 
    rp3d::Quaternion    orientation = rp3d::Quaternion::identity(); 
    rp3d::Transform     transform(position, orientation);
    // create a rigidbody at (0,0,0)
    Object::body = physicsWorld->createRigidBody(transform);

    
}

void Object::Draw(Shader& shader, Camera& camera){
    rp3d::Transform transform   = body->getTransform();
    rp3d::Vector3       pos     = transform.getPosition();
    rp3d::Quaternion    ori     = transform.getOrientation();

    glm::vec3 position      = glm::vec3(pos.x,pos.y,pos.z);
    glm::quat quaternion    = glm::quat(
        ori.w,// glm quaternions are stored w,x,y,z
        ori.x,// whereas rp3d ones are stored x,y,z,w.
        ori.y,
        ori.z);

    model->setPosition(position);
    model->setQuaternion(quaternion);
    
    model->Draw(shader, camera);
}
