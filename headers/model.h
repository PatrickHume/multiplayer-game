#ifndef MODEL_CLASS_H
#define MODEL_CLASS_H

#include<json/json.h>
#include"mesh.h"

using json = nlohmann::json;

class Model
{
public:
    Model(const char* file);

    void Draw(Shader& shader, Camera& camera);
    void applyMatrix(glm::mat4 matrixMesh);

    void applyTranslation(glm::vec3 position);
    void applyRotation(glm::vec3 axisOrientation, float angle);
    void applyScale(glm::vec3 scale);

    void setPosition(glm::vec3 position);
    void setOrientation(glm::vec3 orientation, glm::vec3 up = glm::vec3(0.0f,0.0f,0.0f));
    void setScale(glm::vec3 scale);

    void setModelPosition(glm::vec3 position);
    void setModelOrientation(glm::vec3 orientation, glm::vec3 up = glm::vec3(0.0f,0.0f,0.0f));
    void setModelScale(glm::vec3 scale);

    glm::vec3 getPosition();
    glm::quat getQuaternion();
    glm::vec3 getScale();

    void updateLocal();
    void updateWorld();

private:
    const char* file;
    std::vector<unsigned char> data;
    json JSON;

    std::vector<Mesh> meshes;

    std::vector<std::string> loadedTexName;
    std::vector<Texture> loadedTex;

    void loadMesh(unsigned int indMesh);

    void traverseNode(unsigned int nextNode, glm::mat4 matrix = glm::mat4(1.0f));

    glm::vec3 position;    
    glm::quat quaternion;
    glm::vec3 scale;       

    glm::vec3 modelPosition;
    glm::quat modelQuaternion;
    glm::vec3 modelScale;

    glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);

    // local is a combination of modelPosition, modelScale, facing modelOrientation,
    // this should be the fixed and relative to the design of the model.
    glm::mat4 local = glm::mat4(1.0f);
    // world is a combination of position, scale and quaternion.
    // this will change throughout runtime - using quaternions prevents us from gimbal lock.
    glm::mat4 world = glm::mat4(1.0f);

    std::vector<glm::vec3> translationsMeshes;
    std::vector<glm::quat> rotationsMeshes;
    std::vector<glm::vec3> scalesMeshes;
    std::vector<glm::mat4> matricesMeshes;

    std::vector<unsigned char> getData();
    std::vector<float> getFloats(json accessor);
    std::vector<GLuint> getIndices(json accessor);
    std::vector<Texture> getTextures();

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