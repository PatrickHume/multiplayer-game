#ifndef TEXTURE_CLASS_H
#define TEXTURE_CLASS_H

#include<glad/glad.h>
#include<stb/stb_image.h>
#include<learnopengl/shader_t.h>

class Texture
{
    public:
        GLuint ID;
        const char* type;
        GLuint unit;
        Texture(const char* image, const char* texType, GLuint slot);
        ~Texture();
        
        void texUnit(std::shared_ptr<Shader>& shader, const char* uniform);
        void Bind();
        void Unbind();
        void Delete();
    private:
};

#endif