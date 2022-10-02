/*
#include"../headers/material.h"

Material::Material(
    std::string name,
    bool        doubleSided,
    glm::vec4   baseColorFactor,
    float       metallicFactor,
    float       roughnessFactor)
{
    Material::name = name;
    // all of these values are passed to the fragment shader on Bind();
    Material::doubleSided       = doubleSided;
    Material::baseColorFactor   = baseColorFactor;
    Material::metallicFactor    = metallicFactor;
    Material::roughnessFactor   = roughnessFactor;
}

void Material::setBaseColorTexture(Texture& baseColorTexture)
{
    Material::baseColorTexture = baseColorTexture;
    hasBaseColorTexture = true;
}
void Material::setMetallicRoughnessTexture(Texture& metallicRoughnessTexture)
{
    metallicRoughnessTexture = metallicRoughnessTexture;
    hasMetallicRoughnessTexture = true;
}

        void Bind();
        void Unbind();
        void Delete();
};
*/