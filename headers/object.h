#ifndef OBJECT_CLASS_H
#define OBJECT_CLASS_H

#include <reactphysics3d/reactphysics3d.h>

#include"model.h"
#include<memory>

struct BoxCollider{
    rp3d::Vector3       halfExtents;
    rp3d::Vector3       position;
    rp3d::Quaternion    orientation;
    rp3d::Transform     transform;
};

class Object
{
    public:
        bool isSelected = false;
        rp3d::RigidBody* body;

        Object(rp3d::PhysicsWorld* physicsWorld, Model* model, rp3d::BodyType bodyType);
        void Draw(Shader& shader, Camera& camera);
        void drawOutline(Shader& shader, Camera& camera);
        void drawColliders(Shader& shader, Camera& camera, Model& cube);
        void drawId(Shader& shader, Camera& camera);
        void addBoxCollider(BoxCollider collider, rp3d::CollisionShape *shape);
        int getId();
    private:
        static int nextId;
        int id;

        rp3d::PhysicsWorld* physicsWorld;
        Model* model;

        std::vector<BoxCollider> boxColliders;
        bool showColliders = false;
        int selectedCollider = 0;

        void setModelTransform();
};

#endif