#ifndef OBJECT_CLASS_H
#define OBJECT_CLASS_H

#include <reactphysics3d/reactphysics3d.h>

#include"model.h"
#include<memory>

struct BoxCollider{
    rp3d::Vector3       halfExtents;
    rp3d::Vector3       position;
    rp3d::Quaternion    orientation;
    rp3d::Transform     RP3DTransform;
    glm::mat4           GLMTransform;
};

class Object
{
    public:
        rp3d::RigidBody* body;

        Object(rp3d::PhysicsWorld* physicsWorld, std::shared_ptr<Model> model, rp3d::BodyType bodyType);
        ~Object();
        void    Draw(std::shared_ptr<Shader>& shader, Camera& camera);
        void    drawOutline(std::shared_ptr<Shader>& blankShader, std::shared_ptr<Shader>& outlineShader, Camera& camera);
        void    drawColliders(std::shared_ptr<Shader>& shader, Camera& camera, Model& cube);
        void    drawId(std::shared_ptr<Shader>& shader, Camera& camera);
        //                                           this could be created inside the method...
        void    addBoxCollider(BoxCollider collider, rp3d::CollisionShape *shape);
        void    prepareInstance();
        int     getId();
        void    Delete();
        std::shared_ptr<Model>  getModel();
        const rp3d::Transform&      getRP3DTransform();
        glm::mat4&                  getGLMTransform();
        //std::vector<glm::mat4&>     getColliderTransforms();
    private:
        static int nextId;
        int id;

        rp3d::PhysicsWorld* physicsWorld;
        std::shared_ptr<Model> model;

        std::vector<BoxCollider> boxColliders;
        bool showColliders = false;
        int selectedCollider = 0;

        glm::mat4 glmTransform;
};

#endif