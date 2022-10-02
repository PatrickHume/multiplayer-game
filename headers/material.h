#ifndef MATERIAL_CLASS_H
#define MATERIAL_CLASS_H

#include<glm/glm.hpp>
#include<vector>
#include<string>

struct Material
{
    std::string     name                = std::string("Initial string");
    bool            doubleSided         = true;
    glm::vec4       baseColorFactor     = glm::vec4(1.0f,1.0f,1.0f,1.0f);
    float           metallicFactor      = 1.0f;
    float           roughnessFactor     = 1.0f;
    bool            hasBaseColorTex          = false;
    int             baseColorTexture         = -1;
    int             metallicRoughnessTexture = -1;
};

/* Material as a class
class Material
{
public:
    Material(
        std::vector<Texture>& textures,
        std::string     name                = std::string("Initial string"),
        bool            doubleSided         = true,
        glm::vec4       baseColorFactor     = glm::vec4(1.0f,1.0f,1.0f,1.0f),
        float           metallicFactor      = 1.0f,
        float           roughnessFactor     = 1.0f,
        int             baseColorTexture         = -1,
        int             metallicRoughnessTexture = -1);
private:
    std::string name;
    // all of these values are passed to the fragment shader on Bind();
    bool            doubleSided;
    glm::vec4       baseColorFactor;
    float           metallicFactor;
    float           roughnessFactor;
    unsigned int    baseColorTexture;
    unsigned int    metallicRoughnessTexture;
    std::vector<Texture>& textures;

    void Bind();
    void Unbind();
    void Delete();
};
*/
#endif