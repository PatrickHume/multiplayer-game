#ifndef MESH_CLASS_H
#define MESH_CLASS_H

#include<string>

#include"VAO.h"
#include"EBO.h"
#include"camera.h"
#include"texture.h"

class Mesh
{
    public:
        std::vector <Vertex> vertices;
        std::vector <GLuint> indices;
        std::vector <Texture> textures;

        VAO VAO;

        Mesh(std::vector <Vertex>& vertices, std::vector <GLuint>& indices, std::vector <Texture>& textures);

        void Draw(
            Shader& shader, 
            Camera& camera,
            glm::mat4 world = glm::mat4(1.0f),
            glm::mat4 local = glm::mat4(1.0f),
            glm::mat4 matrix = glm::mat4(1.0f)
            );
};

#endif