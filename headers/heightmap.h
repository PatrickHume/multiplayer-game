// Based heavily on https://learnopengl.com/Guest-Articles/2021/Tessellation/Tessellation
#ifndef HEIGHTMAP_CLASS_H
#define HEIGHTMAP_CLASS_H

#include<string>
#include<fstream>
#include<sstream>
#include<iostream>
#include<cerrno>
#include<vector>
#include<stb/stb_image.h>
#include <glad/glad.h>
#include <learnopengl/shader_t.h>
#include"VAO.h"
#include"camera.h"
#include"texture.h"

class Heightmap
{
    public:
        Heightmap(const char *filename);
        ~Heightmap();
        void Draw(Camera& camera);
    private:
        const unsigned int NUM_PATCH_PTS = 4;
        int width, height, nrChannels;
        std::vector<float> vertices;
        GLuint terrainVAO, terrainVBO;
        std::shared_ptr<Shader> tesselationShader;
        std::shared_ptr<Texture> texture;
        // Set the number of patches across and down.
        // The total number is rez^2.
        unsigned int rez = 32;
};

#endif

