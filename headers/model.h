#ifndef MODEL_CLASS_H
#define MODEL_CLASS_H

#include<json/json.h>
#include <fstream>
#include <reactphysics3d/reactphysics3d.h>
#include <learnopengl/shader_t.h>
#include"mesh.h"
#include"texture.h"
#include"stringSwitch.h"

using json = nlohmann::json;

struct Collider{
    int numVertices;
    int numIndices;
    int numFaces;
    float* vertices;
    int* indices;

    rp3d::PolygonVertexArray::PolygonFace* faces;
    rp3d::PolygonVertexArray* polygonArray;
    rp3d::PolyhedronMesh* mesh;
    rp3d::ConvexMeshShape* shape;
};

class Model
{
public:
    Model(const char* file, rp3d::PhysicsCommon& physicsCommon);
    ~Model();

    bool readyToInstance();
    void drawInstanced(std::shared_ptr<Shader>& shader, Camera& camera);
    void drawBatch(std::shared_ptr<Shader>& shader, Camera& camera);
    void Draw(std::shared_ptr<Shader>& shader, Camera& camera);
    void drawOutline(std::shared_ptr<Shader>& outlineShader, Camera& camera);
    void drawId(std::shared_ptr<Shader>& shader, Camera& camera, int id);
    void drawPrepOutline(std::shared_ptr<Shader>& shader, Camera& camera);
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

    std::vector<rp3d::ConvexMeshShape*>& getCollisionShapes();
private:
    const char* file;
    std::vector<unsigned char> data;
    json JSON;

    const unsigned int instancingThreshold = 50;
    unsigned int numInstances = 0;
    int instanceIndex = 0;
    std::vector<glm::mat4> instanceMatrices;

    float mass = 10.0f;

    void setUniforms(std::shared_ptr<Shader>& shader, Camera& camera);

    std::string fileStr;
    std::string fileDirectory;

    std::vector<Mesh*> meshes;
    std::vector<struct Material> materials;

    std::vector<std::string> loadedTexName;
    std::vector<std::shared_ptr<Texture>> textures;

    std::vector<Collider> colliders;
    std::vector<rp3d::ConvexMeshShape*> collisionShapes;

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

    std::vector<unsigned char> loadData();
    std::vector<float> getFloats(json accessor);
    std::vector<GLuint> getIndices(json accessor);
    std::vector<std::shared_ptr<Texture>> loadTextures();
    std::vector<struct Material> loadMaterials();
    std::vector<Collider> loadColliders(rp3d::PhysicsCommon& physicsCommon);

    std::vector<float> colliderVertices;
    std::vector<int> colliderIndices;

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