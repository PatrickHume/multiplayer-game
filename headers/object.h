#ifndef OBJECT_CLASS_H
#define OBJECT_CLASS_H

#include <reactphysics3d/reactphysics3d.h>

#include"model.h"
#include<memory>

class Object
{
    public:
        bool isSelected = false;
        rp3d::RigidBody* body;

        Object(rp3d::PhysicsWorld* physicsWorld, Model* model, rp3d::BodyType bodyType);
        void Draw(Shader& shader, Camera& camera, DrawType type = DRAW_REGULAR);
    private:
        static int nextId;
        int id;

        rp3d::PhysicsWorld* physicsWorld;
        Model* model;
};

#endif