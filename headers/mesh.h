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
        std::vector<glm::mat4> instanceMatrices;
        struct Material material;
        
        glm::mat4 matrix;

        glm::vec3 scale;
        glm::mat4 rotation;
        glm::vec3 translation;
        glm::vec3 skew;
        glm::vec4 perspective;

        glm::quat quaternion;

        VAO VAO;
        VBO vbo;
        EBO EBO;
        VBO instanceVBO;

        Mesh(
            std::vector <Vertex>& vertices, 
            std::vector <GLuint>& indices, 
            struct Material& material,
            glm::mat4 matrix);
        ~Mesh();

        void drawSimple(
            std::shared_ptr<Shader>& shader, 
            Camera& camera);

        void drawInstanced(
            std::shared_ptr<Shader>& shader, 
            Camera& camera,
            std::vector<std::shared_ptr<Texture>>& textures,
            unsigned int numInstances);

        void Draw(
            std::shared_ptr<Shader>& shader, 
            Camera& camera,
            std::vector<std::shared_ptr<Texture>>& textures);

        void setInstanceMatrices(std::vector<glm::mat4>& matrices);
    private:
};

#endif
