#include"../headers/object.h"
int Object::nextId = 0;

Object::Object(rp3d::PhysicsWorld* physicsWorld, Model* model, rp3d::BodyType bodyType){
    id = nextId++;
    Object::model = model;

    Object::physicsWorld = physicsWorld;
    rp3d::Vector3       position(0, 0, 0); 
    rp3d::Quaternion    orientation = rp3d::Quaternion::identity(); 
    rp3d::Transform     transform(position, orientation);
    // create a rigidbody at (0,0,0)
    body = physicsWorld->createRigidBody(transform);
    body->setType(bodyType);
}

void Object::addBoxCollider(BoxCollider collider, rp3d::CollisionShape *shape){
    rp3d::Transform transform(collider.position, collider.orientation);
    collider.transform = transform;
    boxColliders.push_back(collider);
    body->addCollider(shape, transform);
}

void Object::Draw(Shader& shader, Camera& camera, DrawType drawType){
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

    // if the model is selected, prepare the stencil buffer
    // for any draw calls with OUTLINE
    if (drawType == DrawType::REGULAR && isSelected){
        drawType = DrawType::PREPARE_OUTLINE;
    }

    model->Draw(shader, camera, drawType);
}

void Object::DrawColliders(Shader& shader, Camera& camera, Model& cube, DrawType drawType){
    rp3d::Transform transform   = body->getTransform();
    for (int i = 0; i < boxColliders.size(); i++){
        rp3d::Transform newTransform = boxColliders[i].transform * transform;

        rp3d::Vector3       pos     = newTransform.getPosition();
        rp3d::Quaternion    ori     = newTransform.getOrientation();

        glm::vec3 position      = glm::vec3(pos.x,pos.y,pos.z);
        glm::quat quaternion    = glm::quat(
            ori.w,// glm quaternions are stored w,x,y,z
            ori.x,// whereas rp3d ones are stored x,y,z,w.
            ori.y,
            ori.z);

        cube.setPosition(position);
        cube.setQuaternion(quaternion);
        cube.setScale(glm::vec3(
            boxColliders[i].halfExtents.x*2,
            boxColliders[i].halfExtents.y*2,
            boxColliders[i].halfExtents.z*2));

        cube.Draw(shader, camera, DrawType::REGULAR);
    }
}
