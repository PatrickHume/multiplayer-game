#ifndef VBO_H
#define VBO_H

#include<glm/glm.hpp>
#include<glad/glad.h>
#include<vector>
#include<iostream>

struct Vertex
{
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec3 color;
    glm::vec2 texUV;
};

class VBO
{
    public:
        GLuint ID;
        VBO();
        ~VBO();
        void addVertices(std::vector<Vertex>& vertices);
        void UpdateInstanceMatrices(std::vector<glm::mat4>& matrixes);

        void Bind();
        void Unbind();
};

#endif