#ifndef TEXTRENDERER_CLASS_H
#define TEXTRENDERER_CLASS_H

#include<string>
#include<fstream>
#include<sstream>
#include<iostream>
#include<cerrno>

#include<map>
#include<glad/glad.h>
#include<GLFW/glfw3.h>
#include<glm/glm.hpp>

#include"shader.h"

#include <freetype2/ft2build.h>
#include FT_FREETYPE_H

//https://learnopengl.com/In-Practice/Text-Rendering

struct Character {
    unsigned int textureID;  // ID handle of the glyph texture
    glm::ivec2   size;       // Size of glyph
    glm::ivec2   bearing;    // Offset from baseline to left/top of glyph
    unsigned int advance;    // Offset to advance to next glyph
};

class TextRenderer
{
    public:
        TextRenderer();
        ~TextRenderer();
        void renderText(Shader &s, std::string text, float x, float y, float scale, glm::vec3 color);
    private:
        std::map<char, Character> characters;
        unsigned int VAO, VBO;
        unsigned int texture;
};

#endif