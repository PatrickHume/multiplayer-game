#ifndef TEXTURE_CLASS_H
#define TEXTURE_CLASS_H

#include<glad/glad.h>
#include<stb/stb_image.h>
#include<learnopengl/shader_t.h>

class Texture
{
    public:
        GLuint ID;
        GLuint unit;
        Texture(int width, int height, int channels = 1, GLuint slot = 0);
        Texture(const char* image, GLuint slot = 0);
        ~Texture();
        void Load();
        
        void texUnit(std::shared_ptr<Shader>& shader, const char* uniform);
        void Bind();
        void Unbind();
        void Delete();
        int getHeight();
        int getWidth();
        int getNumColCh();
    private:
        // Stores the width, height, and the number of color channels of the image
        int widthImg, heightImg, numColCh;
        unsigned char* bytes;
};

#endif