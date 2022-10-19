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

class Heightmap
{
    public:
        Heightmap(const char *filename);
        ~Heightmap();
        void Draw(Camera& camera);
    private:
        const unsigned int NUM_PATCH_PTS = 4;
        unsigned int texture;
        int width, height, nrChannels;
        std::vector<float> vertices;
        GLuint terrainVAO, terrainVBO;
        std::shared_ptr<Shader> tesselationShader;
        // Set the number of patches across and down.
        // The total number is rez^2.
        unsigned rez = 20;
};

#endif

