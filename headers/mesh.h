#ifndef MESH_CLASS_H
#define MESH_CLASS_H

#include<string>

#include<glm/gtx/matrix_decompose.hpp>
#include"VAO.h"
#include"EBO.h"
#include"camera.h"
#include"texture.h"
#include"material.h"

class Mesh
{
    public:
        std::vector <Vertex> vertices;
        std::vector <GLuint> indices;
        std::vector <Texture> textures;
        struct Material material;
        
        glm::mat4 matrix;

        glm::vec3 scale;
        glm::mat4 rotation;
        glm::vec3 translation;
        glm::vec3 skew;
        glm::vec4 perspective;

        glm::quat quaternion;

        VAO VAO;

        Mesh(
            std::vector <Vertex>& vertices, 
            std::vector <GLuint>& indices, 
            struct Material& material,
            glm::mat4 matrix);

        void drawSimple(
            Shader& shader, 
            Camera& camera);

        void Draw(
            Shader& shader, 
            Camera& camera,
            std::vector<Texture>& textures);

        void drawOutline(
            Shader& outlineShader, 
            Camera& camera,
            std::vector<Texture>& textures);
};

#endif
