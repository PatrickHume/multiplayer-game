#ifndef WORLD_CLASS_H
#define WORLD_CLASS_H

#include"../headers/user.h"
#include"../headers/textRenderer.h"
#include"../headers/stringSwitch.h"
#include"../headers/shader.h"
#include"../headers/interface.h"
#include"../headers/camera.h"

class World
{
    public:
        World(const unsigned int width, const unsigned int height);
        void ProcessInput(GLFWwindow *window);
        void Update();
        void Draw();  
        void Delete();    
        void createObject(ObjectID objectId);
        void createObjectAtPos(ObjectID objectId, glm::vec3 pos);
    private:
        bool keyIsHeld[260];
        bool firstPress(GLFWwindow *window, int key);

        // Generates Shader object using shaders defualt.vert and default.frag
        Shader defaultShader;
        // This shader is used to highlight selected objects with an outline
        Shader outlineShader;
        Shader textShader;

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

        int width;
        int height;

        std::vector<Object> objects;
};

#endif
