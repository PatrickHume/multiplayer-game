// Based heavily on https://learnopengl.com/Guest-Articles/2021/Tessellation/Tessellation
#ifndef HEIGHTMAP_CLASS_H
#define HEIGHTMAP_CLASS_H

#include<string>
#include<fstream>
#include<sstream>
#include<iostream>
#include<cerrno>
#include<vector>
#include<array>
#include <glad/glad.h>
#include <learnopengl/shader_t.h>
#include"VAO.h"
#include"camera.h"
#include"texture.h"
#include<stb/stb_image_write.h>

class Heightmap
{
    public:
        Heightmap(const char *filename);
        ~Heightmap();
        void Draw(Camera& camera);
    private:
        /* ----------------------------------- Hidden Buffers ----------------------------------- */
        // The hidden frame buffer where pixel heights are written to
        // in order to correctly set minimum tesselation levels for each patch.
        GLuint framebufId;
        // The hidden render buffer with pixel color values of 32 bits 
        // for storing the 32 bit integer Ids of objects.
        GLuint renderbufId;
        GLuint depthbufId;

        const unsigned int NUM_PATCH_PTS = 4;
        int width, height, nrChannels;
        std::vector<float> vertices;
        GLuint terrainVAO, terrainVBO;
        std::shared_ptr<Shader> tesselationShader;
        std::shared_ptr<Shader> setupShader;
        std::shared_ptr<Texture> texture;
        std::shared_ptr<Texture> tessLevelsTexture;
        // Set the number of patches across and down.
        // The total number is rez^2.
        static const unsigned int rez = 20;
        std::array<int, rez*rez> minTessLevels;
        std::array<float, rez*rez> tessLevelError;
};

#endif

