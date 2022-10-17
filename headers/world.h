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
        World();
        ~World();
        void ProcessInput(GLFWwindow *window);
        void Update();
        void Draw();  
        void createObject(std::shared_ptr<Model> model);
        void createObjectAtPos(std::shared_ptr<Model> model, glm::vec3 pos, glm::vec3 vel = glm::vec3(0.0,0.0,0.0));
        void fireObject(std::shared_ptr<Model> model, float speed = 10.0f);
        void Resize(GLFWwindow *window);
    private:
        /* ----------------------------------- Shaders ----------------------------------- */
        // The default shader used for rendering with diffuse and specular lighting.
        Shader defaultShader;
        // Similar to the default shader, but used for instanced rendering.
        // The transform values are passed to this shader via a VBO in the VAO.
        Shader instancedShader;
        // A blank shader which draws all objects as white.
        // Used for setting the stencil values.
        Shader blankShader;
        // A shader used to outline selected objects.
        // Draws the object in a block color with its surfaces extended outwards, 
        // making it appear larger. Used in conjunction with the 'blankShader' to stencil-away 
        // the center of the larger block color object, leaving just the its outline.
        Shader outlineShader;
        // A shader used for rendering text with freeType.
        Shader textShader;
        // The shader used to write object Id values to the a hidden buffer,
        // referenced in the selectObject() function.
        Shader idShader;
        
        /* ----------------------------------- ReactPhysics3D ----------------------------------- */
        // This is a factory module that you can use to create physics 
        // world and other objects. It is also responsible for 
        // logging and memory management.
        rp3d::PhysicsCommon physicsCommon; 
        rp3d::PhysicsWorld::WorldSettings physicsWorldSettings;
        // The physics world responsible for handling all physics events.
        rp3d::PhysicsWorld* physicsWorld; 
        // The fixed length of each physics frame in seconds.
        // If the visual framerate is too slow, multiple phyisics calls
        // can happen in one frame to make up for it.
        // It is important that this value is fixed.
        const rp3d::decimal timeStep = 1.0f / 60.0f;
        // The physics time value at the current frame.
        double currentPhysicsTime;
        // The physics time value of the previous frame.
        double lastPhysicsTime;
        // The physics time over several frames.
        // This allows the physics engine to catch up by processing
        // multiple physics frames per visual frame.
        rp3d::decimal accumulatedPhysicsTime = 0;

        /* ----------------------------------- Models ----------------------------------- */
        // A model of a ruined car (Lada Samara 1500).
        // It should be about 4 meters long if resized properly.
        // Weight should be 945kg (with a full tank of petrol).
        std::shared_ptr<Model> ladaModel;
        // A cube used for development.
        // It should be resized to 1x1x1 metres.
        std::shared_ptr<Model> cubeModel;
        // A flat plane.
        std::shared_ptr<Model> floorModel;
        // A vector holding the models to render during the draw loop.
        std::vector<std::shared_ptr<Model>> models;
        // A map pointing from string names to Model pointers,
        // used to enable the 'summonObject' related user commands.
        std::map<std::string, std::shared_ptr<Model>> mapModels =  
        {{"cube", cubeModel},
        {"lada", ladaModel}};
        // A function mapping strings to models with mapModels
        // Note that it also handles situations where the string entered
        // does not exist in mapModels.
        std::shared_ptr<Model> stringToModel(std::string& name);
        // A vector of model names, automatically populated in the constructor
        // with each key of mapModels. Used to display the model names to the
        // user in repsonse to the 'list objects' command.
        std::vector<std::string> modelNames;

        /* ----------------------------------- Objects ----------------------------------- */
        // A vector populated with every object created.
        std::vector<std::shared_ptr<Object>> objects;

        /* ----------------------------------- Hidden Buffers ----------------------------------- */
        // The hidden frame buffer where object Ids are written to
        // in order for selectObject() to function.
        GLuint framebufId;
        // The hidden render buffer with pixel color values of 32 bits 
        // for storing the 32 bit integer Ids of objects.
        GLuint renderbufId1;
        // The depth buffer used to ensure the nearest object Id is selected
        // in selectObject().
        GLuint depthbufId;

        /* ----------------------------------- UI ----------------------------------- */
        // Stores the object selected by the user.
        User user;
        // Renders text to the screen.
        TextRenderer textRenderer;
        // The object responsible for the in game command line interface.
        Interface interface;
        // The object storing the position and orientation of the player's view.
        // It also takes some inputs from the mouse and keyboard.
        Camera camera;
        // A counter for accumulating the number of frames drawn in a second.
        int frames = 0;
        // A timer for measuring the framerate, stores the time value at the current frame.
        double currentTime;
        // A timer for measuring the framerate, stores the time value of the previous frame.
        double lastTime;

        /* ----------------------------------- User Inputs ----------------------------------- */
        // An array of bools initially set to false
        // used to record which keys are being held down 
        // by indexing the array with their keyId.
        bool keyIsHeld[260] = { 0 };
        // Returns a true if the key pressed has only just been pressed,
        // as opposed to having been held over a number of frames.
        bool firstPress(GLFWwindow *window, int key);
        // Takes a vector of strings 'params' and an int offset 'i' and returns a glm::vec3,
        // using 'i' as the index of the first float in the vector, 'i'+ 1 as the 
        // index of the second and 'i'+ 2 as the index of the third.
        // Each value in params must be castable as a float.
        glm::vec3 stringVecToGlmVec3(std::vector<std::string>& params, int i);
        // Take an int 'id' and loop through 'objects' until a match is found and returned.
        std::shared_ptr<Object> getObjectById(int id);

        /* ----------------------------------- User Commands ----------------------------------- */
        // A typedef for a pointer to a function which takes a string and returns void.
        typedef void (World::*functionPointer)(std::vector<std::string>);
        // The set of methods potentially callable by the user with in game commands.
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
        // Takes a string and uses mapCommands to return the associated function.
        World::functionPointer stringToCommand(std::string& name);
        // A map containing the relationship between every user command and a method in World.
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
};

#endif
