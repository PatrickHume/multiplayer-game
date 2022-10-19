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
        Texture(const char* image, GLuint slot);
        ~Texture();
        
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
};

#endif