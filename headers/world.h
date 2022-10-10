#ifndef WORLD_CLASS_H
#define WORLD_CLASS_H

#include"../headers/user.h"
#include"../headers/textRenderer.h"
#include"../headers/stringSwitch.h"
#include"../headers/shader.h"
#include"../headers/interface.h"
#include"../headers/camera.h"
#include"../headers/screen.h"

class World
{
    public:
        World(GLFWwindow *window);
        void ProcessInput(GLFWwindow *window);
        void Update();
        void Draw();  
        void Delete();    
        void createObject(Model* model);
        void createObjectAtPos(Model* model, glm::vec3 pos, glm::vec3 vel = glm::vec3(0.0,0.0,0.0));
        void fireObject(Model* model, float scalar = 10.0f);
        void Resize(GLFWwindow *window);
    private:
        GLuint renderbufId0;
        GLuint renderbufId1;
        GLuint depthbufId;
        GLuint framebufId;

        bool keyIsHeld[260] = { 0 };
        bool firstPress(GLFWwindow *window, int key);

        // Generates Shader object using shaders defualt.vert and default.frag
        Shader defaultShader;
        Shader instancedShader;
        // This shader is used to highlight selected objects with an outline
        Shader outlineShader;
        Shader textShader;

        Shader idShader;

        // This is a factory module that you can use to create physics 
        // world and other objects. It is also responsible for 
        // logging and memory management 
        rp3d::PhysicsCommon physicsCommon; 
        rp3d::PhysicsWorld::WorldSettings physicsWorldSettings; 

        // Create a physics world 
        rp3d::PhysicsWorld* physicsWorld; 

        User user;
        TextRenderer textRenderer;

        Model ladaModel;
        Model cubeModel;
        Model floorModel;

        Camera camera;
        Interface interface;

        // length of physics frame in seconds
        const rp3d::decimal timeStep = 1.0f / 60.0f;

        //timer for physics
        double lastPhysicsTime;
        double currentPhysicsTime;

        double currentTime;

        rp3d::decimal elapsedPhysicsTime = 0;
        rp3d::decimal accumulatedPhysicsTime = 0;
        //timer for framerate
        double lastTime;
        int frames = 0;

        bool collidersAreVisible = false;

        std::vector<Object> objects;

        glm::vec3 stringVecToGlmVec3(std::vector<std::string>& params, int i);
        Object* getObjectById(short int id);

        //make models string referenceable
        std::map<std::string, Model*> mapModels =  
        {{"cube", &cubeModel},
        {"lada", &ladaModel}};
        Model* stringToModel(std::string& name);
        //this is automatically populated with the key names in mapModels
        std::vector<std::string> modelNames;

        //make functions string referenceable
        typedef void (World::*functionPointer)(std::vector<std::string>);
        World::functionPointer stringToCommand(std::string& name);
        std::map<std::string, functionPointer> mapCommands =  
        {{"summonObject", &World::summonObject}, 
        {"summonManyObjects", &World::summonManyObjects}, 
        {"summonObjectAtPos",  &World::summonObjectAtPos}, 
        {"summonManyObjectsAtPos",  &World::summonManyObjectsAtPos}, 
        {"listObjects",  &World::listObjects},
        {"showColliders",  &World::showColliders},
        {"hideColliders",  &World::hideColliders},
        {"saveColliders",  &World::saveColliders},
        {"editColliders",  &World::editColliders},
        {"selectObject",  &World::selectObject},
        {"deselectObject",  &World::deselectObject},
        {"addCollider",  &World::addCollider},
        {"delCollider",  &World::delCollider},
        {"nextCollider",  &World::nextCollider},
        {"prevCollider",  &World::prevCollider}, 
        {"getColliderPosition",  &World::getColliderPosition},
        {"getColliderRotation",  &World::getColliderRotation},
        {"getColliderScale",  &World::getColliderScale},
        {"setColliderPosition",  &World::setColliderPosition},
        {"setColliderRotation",  &World::setColliderRotation},
        {"setColliderScale",  &World::setColliderScale}};
        
        void summonObject(          std::vector<std::string> params);
        void summonManyObjects(     std::vector<std::string> params);
        void summonObjectAtPos(     std::vector<std::string> params);
        void summonManyObjectsAtPos(std::vector<std::string> params);
        void listObjects(           std::vector<std::string> params);
        void showColliders(         std::vector<std::string> params);
        void hideColliders(         std::vector<std::string> params);
        void saveColliders(         std::vector<std::string> params);
        void editColliders(         std::vector<std::string> params);
        void selectObject(          std::vector<std::string> params);
        void deselectObject(        std::vector<std::string> params);
        void addCollider(           std::vector<std::string> params);
        void delCollider(           std::vector<std::string> params);
        void nextCollider(          std::vector<std::string> params);
        void prevCollider(          std::vector<std::string> params);
        void getColliderPosition(   std::vector<std::string> params);
        void getColliderRotation(   std::vector<std::string> params);
        void getColliderScale(      std::vector<std::string> params);
        void setColliderPosition(   std::vector<std::string> params);
        void setColliderRotation(   std::vector<std::string> params);
        void setColliderScale(      std::vector<std::string> params);
};

#endif
