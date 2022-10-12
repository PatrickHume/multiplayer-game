#ifndef MODEL_CLASS_H
#define MODEL_CLASS_H

#include<json/json.h>
#include <reactphysics3d/reactphysics3d.h>
#include"mesh.h"
#include"shader.h"
#include"texture.h"

using json = nlohmann::json;

class Model
{
public:
    Model();

    void Load(const char* file);

    bool readyToInstance();
    void drawInstanced(Shader& shader, Camera& camera);
    void drawBatch(Shader& shader, Camera& camera);
    void Draw(Shader& shader, Camera& camera);
    void drawOutline(Shader& outlineShader, Camera& camera);
    void drawId(Shader& shader, Camera& camera, int id);
    void drawPrepOutline(Shader& shader, Camera& camera);
    void setTransform(glm::mat4& transform);

    // these are used to correct for the model we imported
    void setModelPosition(glm::vec3 position);
    void setModelOrientation(glm::vec3 orientation, glm::vec3 up = glm::vec3(0.0f,0.0f,0.0f));
    void setModelScale(glm::vec3 scale);
    void updateLocal();
    void addInstance();
    void delInstance();
    void prepareInstance(glm::mat4& transform);
    void setMass(rp3d::decimal mass);
    rp3d::decimal getMass();
private:
    const char* file;
    std::vector<unsigned char> data;
    json JSON;

    const unsigned int instancingThreshold = 50;
    unsigned int numInstances = 0;
    int instanceIndex = 0;
    std::vector<glm::mat4> instanceMatrices;

    float mass = 10.0f;

    void setUniforms(Shader& shader, Camera& camera);

    std::string fileStr;
    std::string fileDirectory;

    std::vector<Mesh> meshes;
    std::vector<struct Material> materials;

    std::vector<std::string> loadedTexName;
    std::vector<Texture> textures;

    void loadMesh(unsigned int indMesh, glm::mat4 matrix);

    void traverseNode(unsigned int nextNode, glm::mat4 matrix = glm::mat4(1.0f));

    //glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f);   
    //glm::quat quaternion = glm::mat4(1.0f);
    //glm::vec3 scale = glm::vec3(1.0f, 1.0f, 1.0f);       

    glm::vec3 modelPosition = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::quat modelQuaternion = glm::mat4(1.0f);
    glm::vec3 modelScale = glm::vec3(1.0f, 1.0f, 1.0f);

    glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);

    // modelTransform is a combination of modelPosition, modelScale, facing modelOrientation,
    // this should be the fixed and relative to the design of the model.
    glm::mat4 modelTransform = glm::mat4(1.0f);
    // world is a combination of position, scale and quaternion.
    // this will change throughout runtime - using quaternions prevents us from gimbal lock.
    glm::mat4 transform = glm::mat4(1.0f);

    std::vector<unsigned char> getData();
    std::vector<float> getFloats(json accessor);
    std::vector<GLuint> getIndices(json accessor);
    std::vector<Texture> getTextures();
    std::vector<struct Material> getMaterials();

    std::vector<Vertex> assembleVertices(
        std::vector<glm::vec3> positions,
        std::vector<glm::vec3> normals,
        std::vector<glm::vec2> texUVs
    );

    std::vector<glm::vec2> groupFloatsVec2(std::vector<float> floatVec);
    std::vector<glm::vec3> groupFloatsVec3(std::vector<float> floatVec);
    std::vector<glm::vec4> groupFloatsVec4(std::vector<float> floatVec);
};

#endif